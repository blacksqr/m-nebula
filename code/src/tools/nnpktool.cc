//------------------------------------------------------------------------------
//  nnpktool.cc
//
//  Manage npk files.
//
//  Usage:
//
//  nnpktool [-help]
//           [-pack dirname]
//           [-list dirname]
//           [-out filename]
//           [-diff]
//           [-listdiff]
//           [-old]
//           [-new]
//           [-unpack]
//
//  -help           show help
//  -pack           pack this input directory
//  -list           list contents of npk files
//  -out            name of output file (def pack.npk)
//  -diff           build upgrade file of two npk files
//  -listdiff       list difference of two npk files
//  -old            the 'older' npk file to compare
//  -new            the 'newer' npk file to compare
//  -unpack         unpack given npk file
//
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/wftools.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndirectory.h"
#include "kernel/nfile.h"
#include "file/nnpktoc.h"
#include "file/nnpkfilewrapper.h"

class FileEntry : public nNode
{
public:

    enum { IDENTICAL, NOT_FOUND, DIFFERENT, SIZE_CHANGED };
    
    FileEntry()
    {
        this->reason   = IDENTICAL;
        this->testFlag = false;
    }
    
    int             reason;
    nString         name;
    bool            testFlag;
    nNpkTocEntry*   entry;
    int             numDiff;
    int             firstDiff;
    int             fileSize;
};

//------------------------------------------------------------------------------
/**
    Strip the parent path from a path name, returning a pointer to the
    last component.
*/
const char*
stripParentPath(const char* path)
{
    n_assert(path);
    const char* lastPart = strrchr(path, '/');
    if (!lastPart)
    {
        return path;
    }

    lastPart += 1;
    n_assert(lastPart != 0);
    return lastPart;
}

//------------------------------------------------------------------------------
/**
    Get the directory name from a path name.
*/
const char*
getDirectoryName(const char* path, char* buf, int bufSize)
{
    n_assert(path);
    n_assert(bufSize > (int)strlen(path));

    const char* tail = stripParentPath(path);
    n_assert(tail);

    const char* from = path;
    char* to   = buf;
    while (from != tail)
    {
        *to++ = *from++;
    }
    *to = 0;
    return buf;
}

//------------------------------------------------------------------------------
/**
    Convert string to lower case.
*/
const char*
convertToLower(const char* path, char* buf, int bufSize)
{
    n_assert((strlen(path) + 1) < (unsigned int) bufSize);

    const char* from = path;
    char* to = buf;     
    char c;
    while ((c = *from++))
    {
        *to++ = tolower(c);
    }
    *to = 0;
    return buf;
}

//------------------------------------------------------------------------------
/**
    Recursively parse source directory and fill a nNpkToc object with the
    data.

    @param  dir             opened nDirectory object containing source dir
    @param  dirName         stripped directory name (NOT the full path!)
    @param  tocObject       the toc object to fill with data
    @param  curFileOffset   [in/out] file offset tracker
*/
bool
generateToc(nFileServer2* fs, nDirectory* dir, const char* dirName, nNpkToc& tocObject, int& curFileOffset)
{
    // add the directory itself to the tocObject
    tocObject.BeginDirEntry(dirName);

    // for each directory entry...
    if (!dir->IsEmpty()) do
    {
        nDirectory::nEntryType entryType = dir->GetEntryType();
        const char* fullEntryName = dir->GetEntryName();
        char buf[N_MAXPATH];
        fullEntryName = convertToLower(fullEntryName, buf, sizeof(buf));

        // strip the parent path from the entry
        const char* entryName = stripParentPath(fullEntryName);

        // add entry to toc
        if (nDirectory::FILE == entryType)
        {
            // get length of file
            nFile* file = fs->NewFileObject();
            n_assert(file);
            int fileLength = 0;
            bool fileOk = false;
            if (file->Open(fullEntryName, "rb"))
            {
                file->Seek(0, nFile::END);
                fileLength = file->Tell();
                file->Close();
                fileOk = true;
            }
            delete file;

            if (fileOk)
            {
//                n_printf("-> adding file '%s' at %d len %d\n", entryName, curFileOffset, fileLength);
                tocObject.AddFileEntry(entryName, curFileOffset, fileLength);
                curFileOffset += fileLength;
            }
            else
            {
                n_printf("*** ERROR: Could not open file '%s', skipping...\n", fullEntryName);
            }
        }
        else if (nDirectory::DIRECTORY == entryType)
        {
//            n_printf("-> recursing into dir '%s'\n", entryName);

            // start a new subdirectory entry
            nDirectory* subDir = fs->NewDirectoryObject();
            n_assert(subDir);
            bool subDirOk = false;
            if (subDir->Open(fullEntryName))
            {
                generateToc(fs, subDir, entryName, tocObject, curFileOffset);
                subDir->Close();
                subDirOk = true;
            }
            delete subDir;

            if (!subDirOk)
            {
                n_printf("*** ERROR: Could not open directory '%s', skipping...\n", entryName);
            }
        }
    } while (dir->SetToNextEntry());

    // finish the directory
    tocObject.EndDirEntry();
    return true;
}

//------------------------------------------------------------------------------
/**
    Write the header, leave the dataOffset field empty, this will be
    written later by the fixDataOffset function.
*/
bool
writeHeader(nFile* file)
{
    file->PutInt('NPK0');       // magic number
    file->PutInt(4);            // block len
    file->PutInt(0);            // dataOffset (fixed later)
    return true;
}

//------------------------------------------------------------------------------
/**
    Fix the data offset in the file header
*/
bool
writeDataOffset(nFile* file, int dataOffset)
{
    file->Seek(8, nFile::START);
    file->PutInt(dataOffset);
    return true;
}

//------------------------------------------------------------------------------
/**
    Write a single toc entry to the file, recurse if necessary.
*/
void
writeTocEntry(nFile* file, nNpkTocEntry* tocEntry)
{
    n_assert(file);
    n_assert(tocEntry);
    
    nNpkTocEntry::Type entryType = tocEntry->GetType();
    const char* entryName = tocEntry->GetName();
    int entryNameLen = strlen(entryName);
    int entryFileOffset = tocEntry->GetFileOffset();
    int entryFileLength = tocEntry->GetFileLength();

    if (nNpkTocEntry::DIR == entryType)
    {
//        n_printf("=> writing dir entry '%s'\n", entryName);

        // write a directory entry, and recurse
        int blockLen = sizeof(short) + entryNameLen;
        file->PutInt('DIR_');
        file->PutInt(blockLen);
        file->PutShort(entryNameLen);
        file->Write(entryName, entryNameLen);

        nNpkTocEntry* curSubEntry = tocEntry->GetFirstEntry();
        while (curSubEntry)
        {
            writeTocEntry(file, curSubEntry);
            curSubEntry = tocEntry->GetNextEntry(curSubEntry);
        }

        // write a final directory end marker
        file->PutInt('DEND');
        file->PutInt(0);
    }
    else if (nNpkTocEntry::FILE == entryType)
    {
//        n_printf("=> writing file entry '%s', offset %d len %d\n", entryName, entryFileOffset, entryFileLength);

        // write a file entry
        int blockLen = 2 * sizeof(int) + sizeof(short) + entryNameLen;
        file->PutInt('FILE');
        file->PutInt(blockLen);
        file->PutInt(entryFileOffset);
        file->PutInt(entryFileLength);
        file->PutShort(entryNameLen);
        file->Write(entryName, entryNameLen);
    }
}

//------------------------------------------------------------------------------
/**
    Recursively write the table of contents to the file.
*/
bool
writeToc(nFile* file, nNpkToc& tocObject)
{
    if (writeHeader(file))
    {
        writeTocEntry(file, tocObject.GetRootEntry());
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Write data for a toc file into nFile, recurse on directory entries.
*/
bool
writeEntryData(nFileServer2* fs, nFile* file, nNpkTocEntry* tocEntry, int dataBlockOffset, int& dataLen)
{
    n_assert(file);
    n_assert(tocEntry);

    nNpkTocEntry::Type entryType = tocEntry->GetType();
    const char* entryName = tocEntry->GetName();
    int entryFileOffset = tocEntry->GetFileOffset();
    int entryFileLength = tocEntry->GetFileLength();

    if (nNpkTocEntry::DIR == entryType)
    {
        n_printf("=> enter dir '%s'\n", entryName);

        // a dir entry, just recurse
        nNpkTocEntry* curSubEntry = tocEntry->GetFirstEntry();
        while (curSubEntry)
        {
            if (!writeEntryData(fs, file, curSubEntry, dataBlockOffset, dataLen))
            {
                return false;
            }
            curSubEntry = tocEntry->GetNextEntry(curSubEntry);
        }
    }
    else if (nNpkTocEntry::FILE == entryType)
    {
        // make sure the file is still consistent with the toc data
        n_assert(file->Tell() == (dataBlockOffset + entryFileOffset));

        // get the full source path name
        char fileName[N_MAXPATH];
        tocEntry->GetFullName(fileName, sizeof(fileName));

        // read source file data
        nFile* srcFile = fs->NewFileObject();
        n_assert(srcFile);
        if (srcFile->Open(fileName, "rb"))
        {
            // allocate buffer for file and file contents
            char* buffer = n_new char[entryFileLength];
            int bytesRead = srcFile->Read(buffer, entryFileLength);
            srcFile->Close();
            delete srcFile;
            if (bytesRead != entryFileLength)
            {
                n_delete buffer;
                n_printf("Error reading file '%s'!\n", fileName);
                return false;
            }

            // write buffer to target file
            int bytesWritten = file->Write(buffer, entryFileLength);
            n_delete buffer;
            if (bytesWritten != entryFileLength)
            {
                n_printf("Error writing to target file!\n");
                return false;
            }
            dataLen += entryFileLength;
        }
        else
        {
            n_printf("Failed to open source file '%s'!\n", fileName);
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Write the data block to the file, and fix the data start offset in the
    file header...
*/
bool
writeData(nFileServer2* fs, nFile* file, nNpkToc& tocObject)
{
    int dataBlockStart = file->Tell();
    file->PutInt('DATA');
    int dataLenOffset = file->Tell();
    file->PutInt(0);    // fix later

    int dataSize = 0;
    if (!writeEntryData(fs, file, tocObject.GetRootEntry(), dataBlockStart + 8, dataSize))
    {
        return false;
    }

    // fix block lengths
    writeDataOffset(file, dataBlockStart);

    file->Seek(dataLenOffset, nFile::START);
    file->PutInt(dataSize);
    file->Seek(0, nFile::END);
    return true;
}

//------------------------------------------------------------------------------
/**
    The global pack function.
*/
bool
packIt(nFileServer2* fs, const char* dirName, const char* outName)
{
    char absDirName[N_MAXPATH];
    char absOutName[N_MAXPATH];

    // absolutize filenames
    fs->MakeAbsolute(dirName, absDirName, sizeof(absDirName));
    fs->MakeAbsolute(outName, absOutName, sizeof(absOutName));

    // create directory and file objects
    nDirectory* dir = fs->NewDirectoryObject();
    nFile* file = fs->NewFileObject();
    n_assert(dir);
    n_assert(file);

    // open source directory and target file...
    if (!dir->Open(absDirName))
    {
        n_printf("Could not open directory '%s' for reading!\n", dirName);
        delete file;
        delete dir;
        return false;
    }
    if (!file->Open(absOutName, "w"))
    {
        n_printf("Could not open file '%s' for writing!\n", outName);
        delete file;
        delete dir;
        return false;
    }

    // create table of content
    nNpkToc tocObject;
    tocObject.SetRootPath(fs->GetCwd());
    
    bool retval = true;
    n_printf("-> building table of contents...\n");
    int fileOffset = 0;
    if (generateToc(fs, dir, stripParentPath(absDirName), tocObject, fileOffset))
    {
        n_printf("-> done\n");

        // write header and toc to file
        n_printf("-> writing toc...\n");
        if (writeToc(file, tocObject))
        {
            n_printf("-> done\n");

            // write data block
            n_printf("-> writing data block...\n");
            if (writeData(fs, file, tocObject))
            {
                n_printf("-> all done\n");
            }
            else
            {
                n_printf("*** ERROR WRITING DATA BLOCK\n");
                retval = false;
            }
        }
        else
        {
            n_printf("*** ERROR WRITING TOC\n");
            retval = false;
        }
    }
    else
    {
        n_printf("*** ERROR PARSING DIRECTORY\n");
        retval = false;
    }

    // close everything and finish
    file->Close();
    dir->Close();
    delete file;
    delete dir;
    return retval;
}

//------------------------------------------------------------------------------
/**
    Print out contents of a single toc entry, and recurse.
*/
void
printTocEntry(nNpkTocEntry* entry, int& recursionDepth)
{
    n_assert(entry);

    const char* name = entry->GetName();
    nNpkTocEntry::Type type = entry->GetType();
    int length = entry->GetFileLength();

    // print recursion depth spaces...
    int i = 0;
    for (i = 0; i < recursionDepth; i++)
    {
        n_printf("  ");
    }

    // show actual entry info
    n_printf("%s\t%s\t%d\n", (nNpkTocEntry::DIR == type) ? "dir" : "file",
		             name, length);

    // if its a dir entry, recurse
    if (nNpkTocEntry::DIR == type)
    {
        recursionDepth++;

        nNpkTocEntry* childEntry = entry->GetFirstEntry();
        if (childEntry) do
        {
            printTocEntry(childEntry, recursionDepth);
        } while ((childEntry = entry->GetNextEntry(childEntry)));

        recursionDepth--;
    }
}

//------------------------------------------------------------------------------
/**
    Build "internal path" (path inside pack file), without "data.n",
    because thuis is the name of the file and has nothing to do with
    the content.
*/
void
BuildInternalPath(nNpkTocEntry* entry, char* buffer, int bufSize)
{
    static const int maxDepth = 16;
    nNpkTocEntry* traceStack[maxDepth];

    // fill trace stack with path to parent...
    int depth = 0;
    nNpkTocEntry* curEntry = entry;
    while (curEntry && (depth < maxDepth))
    {
        traceStack[depth++] = curEntry;
        curEntry = curEntry->GetParent();
    }
    
    // fill path string while stepping down
    int curBufIndex = 0;
    depth--;    // because incrementing in loop above
    depth--;    // because ignoring data.n
    for (; depth >= 0; --depth)
    {
        const char* curName = traceStack[depth]->GetName();
        int curNameLen = strlen(curName);

        n_assert((curBufIndex + curNameLen + 1) < bufSize);

        strcpy(&(buffer[curBufIndex]), curName);
        curBufIndex += curNameLen;
        if (depth > 0)
        {
            buffer[curBufIndex++] = '/';
        }
    }
    n_assert(curBufIndex < bufSize);
    buffer[curBufIndex] = 0;
}

//-----------------------------------------------------------------------------
/**
*/
bool
IsFileInList(nList* list, const char* internalName)
{
    nString intName(internalName);
    FileEntry* entry;

    for (entry = (FileEntry*) list->GetHead();
         entry != 0;
         entry = (FileEntry*) entry->GetSucc())
    {
        // test entries they were not found yet
        if (entry->name == intName)
        {
            return true;
        }
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
KillEmptyDirectories(nFileServer2* fs, const char* base)
{
    nDirectory* dir = fs->NewDirectoryObject();
    n_assert(0 != dir);
    
    char curDir[N_MAXPATH];
    strcpy(curDir, fs->GetCwd());
    
    if (dir->Open(base))
    {
        // if this one is empty, return immediatly and let kill
        // directory outside
        if (dir->IsEmpty())
        {
            dir->Close();
            return true;
        }
        
        fs->ChangeDir(base);
        
        // step through all elements
        bool doLoop = dir->SetToFirstEntry(); 
        while (doLoop)
        {
            nDirectory::nEntryType t = dir->GetEntryType();
            if (nDirectory::DIRECTORY == t)
            {
                bool empty = KillEmptyDirectories(fs, dir->GetEntryName());
                if (empty)
                {
                    rmdir(dir->GetEntryName());
                }
            }
    
            doLoop = dir->SetToNextEntry();
        }
    
        dir->Close();
        fs->ChangeDir(curDir);
    }
    else
    {
        n_printf("Cannot open directory %s for reading...\n", base);
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
RemoveDir(nFileServer2* fs, const char* dirName)
{
    // step recursive through all subdirs, kill files and then
    // kill the empty directory
    nDirectory* dir = fs->NewDirectoryObject();
    n_assert(0 != dir);
    
    char curDir[N_MAXPATH];
    strcpy(curDir, fs->GetCwd());
    
    if (dir->Open(dirName))
    {
        fs->ChangeDir(dirName);
        
        // seems to be no problem deleting files while stepping 
        // through subdir...
        bool doLoop = dir->SetToFirstEntry(); 
        while (doLoop)
        {
            nDirectory::nEntryType t = dir->GetEntryType();
            if (nDirectory::DIRECTORY == t)
            {
                RemoveDir(fs, dir->GetEntryName());
            }
    
            if (nDirectory::FILE == t)
            {
                remove(dir->GetEntryName());
            }
    
            doLoop = dir->SetToNextEntry();
        }
    
        dir->Close();
        
        fs->ChangeDir(curDir);
    }
    
    rmdir(dirName);
}

//------------------------------------------------------------------------------
/**
*/
bool
AreFilesIdentical(FileEntry* oldEntry, FileEntry* newEntry, 
                  int& reason, int& numDiff, int& firstDiff)
{
    nNpkTocEntry* oldNpkEntry = oldEntry->entry;
    nNpkTocEntry* newNpkEntry = newEntry->entry;
    
    if (oldNpkEntry->GetFileLength() != newNpkEntry->GetFileLength())
    {
        // different size --> must be different
        reason = FileEntry::SIZE_CHANGED;
        return false;
    }
    
    nNpkFileWrapper* oldWrapper = oldNpkEntry->GetFileWrapper();
    nNpkFileWrapper* newWrapper = newNpkEntry->GetFileWrapper();
    n_assert(0 != oldWrapper);
    n_assert(0 != newWrapper);
    
    nFile* oldFile = oldWrapper->GetBinaryFile();
    nFile* newFile = newWrapper->GetBinaryFile();
    n_assert(0 != oldFile);
    n_assert(0 != newFile);
    
    int size = oldNpkEntry->GetFileLength();
    char* oldArray = new char[size];
    char* newArray = new char[size];
    oldFile->Seek(oldNpkEntry->GetFileOffset(), nFile::START);
    newFile->Seek(newNpkEntry->GetFileOffset(), nFile::START);
    
    oldFile->Read(oldArray, size);
    newFile->Read(newArray, size);
    
    int retValue = memcmp(oldArray, newArray, size);
    
    numDiff = 0;
    firstDiff = -1;
    if (retValue != 0)
    {
        int i;
        for (i = 0; i < size; i++)
        {
            if ( ((char)(oldArray[i])) != ((char)(newArray[i])))
            {
                numDiff++;
                
                if (firstDiff == -1)
                {
                    firstDiff = i;
                }
            }
        }
    }

    delete oldArray;
    delete newArray;

    if (retValue != 0)
    {
        reason = FileEntry::DIFFERENT;
    }
    else
    {
        reason = FileEntry::IDENTICAL;
    }
    
    return (0 == retValue) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
void
BuildDifferenceList(nList& oldList, nList& newList, nList& diffList)
{
    // not very beautiful....
    FileEntry* oldEntry = 0, *newEntry = 0, *diffEntry = 0;

    // clear all test flags for faster search
    for (oldEntry = (FileEntry*) oldList.GetHead();
         oldEntry != 0;
         oldEntry = (FileEntry*) oldEntry->GetSucc())
    {
        oldEntry->testFlag = false;
    }

    for (newEntry = (FileEntry*) newList.GetHead();
         newEntry != 0;
         newEntry = (FileEntry*) newEntry->GetSucc())
    {
        // look for newEntry inside oldList
        bool found    = false;
        int reason    = FileEntry::NOT_FOUND;
        int numDiff   = 0;
        int firstDiff = -1;
        for (oldEntry = (FileEntry*) oldList.GetHead();
             oldEntry != 0;
             oldEntry = (FileEntry*) oldEntry->GetSucc())
        {
            // test entries they were not found yet
            if (!oldEntry->testFlag)
            {
                if (oldEntry->name == newEntry->name)
                {
                    // the old one was found in the newlist, but
                    // is it different?
                    if (AreFilesIdentical(oldEntry, newEntry, 
                                          reason, numDiff, firstDiff))
                    {
                        found = true;
                    }
                    
                    // testet. ignore it for next steps
                    oldEntry->testFlag = true;
                    break;
                }
            }
        }
        
        if (!found)
        {
            // not in old list??? put a new entry into diff list
            diffEntry = new FileEntry;
            n_assert(0 != diffEntry);

            diffEntry->name      = newEntry->name;
            diffEntry->entry     = newEntry->entry;
            diffEntry->reason    = reason; 
            diffEntry->numDiff   = numDiff;
            diffEntry->firstDiff = firstDiff;
            diffEntry->fileSize  = newEntry->entry->GetFileLength();
            diffList.AddTail((nNode*) diffEntry);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
FillList(nNpkTocEntry* entry, nList& list)
{
    n_assert(0 != entry);

    char name[N_MAXPATH];
    BuildInternalPath(entry, name, N_MAXPATH);
    nNpkTocEntry::Type type = entry->GetType();
    
    // store data in list
    FileEntry* fileEntry = new FileEntry;
    fileEntry->name  = name;
    fileEntry->entry = entry;
    list.AddTail((nNode *) fileEntry);

    if (nNpkTocEntry::DIR == type)
    {
        nNpkTocEntry* childEntry = 0;
        for (childEntry = entry->GetFirstEntry();
             childEntry != 0;
             childEntry = entry->GetNextEntry(childEntry))
        {
            FillList(childEntry, list);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
listIt(nFileServer2* fs, const char* listName)
{
    char absFileName[N_MAXPATH];
    char rootPath[N_MAXPATH];

    // absolutize filenames
    fs->MakeAbsolute(listName, absFileName, sizeof(absFileName));

    // get the directory name from the absolute filename
    getDirectoryName(absFileName, rootPath, sizeof(rootPath));

    // create a file wrapper object and parse npk file
    nNpkFileWrapper wrapper;
    if (wrapper.Open(fs, rootPath, absFileName))
    {
        // get table of contents object
        nNpkToc& toc = wrapper.GetTocObject();

        // print out file contents
        int recursionDepth = 0;
        n_printf("TYPE\tNAME\tLENGTH\n");
        printTocEntry(toc.GetRootEntry(), recursionDepth);
        
        wrapper.Close();
        return true;
    }
    else
    {
        n_printf("Could not open '%s' as npk file!\n", absFileName);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
GenerateDifferenceList(nFileServer2* fs, 
                       const char* oldName,
                       const char* newName,
                       nList& diffList)
{
    char absOldName[N_MAXPATH];
    char absNewName[N_MAXPATH];
    char rootNewPath[N_MAXPATH];
    char rootOldPath[N_MAXPATH];

    // absolutize filenames
    fs->MakeAbsolute(newName, absNewName, sizeof(absNewName));
    fs->MakeAbsolute(oldName, absOldName, sizeof(absOldName));

    // get the directory name from the absolute filename
    getDirectoryName(absNewName, rootNewPath, sizeof(rootNewPath));
    getDirectoryName(absOldName, rootOldPath, sizeof(rootOldPath));

    // create a file wrapper object and parse npk file
    nNpkFileWrapper oldWrapper, newWrapper;
    if (oldWrapper.Open(fs, rootOldPath, absOldName))
    {
        if (newWrapper.Open(fs, rootNewPath, absNewName))
        {
            // get table of contents object
            nNpkToc& oldToc = oldWrapper.GetTocObject();
            nNpkToc& newToc = newWrapper.GetTocObject();

            // build two tables
            nList oldList, newList;
            FillList(oldToc.GetRootEntry(), oldList);
            FillList(newToc.GetRootEntry(), newList);
            
            // print out file contents
            BuildDifferenceList(oldList, newList, diffList);
        
            oldWrapper.Close();
            newWrapper.Close();
            
            FileEntry* fEntry = 0;
            while (0 != (fEntry = (FileEntry*) oldList.RemHead()))
            {
                delete fEntry;
            }
            while (0 != (fEntry = (FileEntry*) newList.RemHead()))
            {
                delete fEntry;
            }
            return true;
        }
        else
        {
            n_printf("Could not open '%s' as npk file!\n", absNewName);
            return false;
        }
    }
    else
    {
        n_printf("Could not open '%s' as npk file!\n", absOldName);
        return false;
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
unPackFile(nFileServer2* fs, 
           nFile* npkFile, 
           nNpkTocEntry* entry, 
           const char* outName,
           nList* dList)
{
    nNpkTocEntry::Type type = entry->GetType();
    if (nNpkTocEntry::FILE == type)
    {
        // we can unpack whole npk files or parts of it. In this case
        // the files to extract are stored in a (FileEntry-) list.
        // Otherwise the list pointer is 0 and we extract every file.
        bool reallyExtract = true;
        if (0 != dList)
        {
            char buffer[N_MAXPATH];
            BuildInternalPath(entry, buffer, N_MAXPATH);
            if (!IsFileInList(dList, buffer))
            {
                reallyExtract = false;
            }
        }
        
        if (reallyExtract)
        {
            nFile* file = fs->NewFileObject();
            n_assert(0 != file);

            char absName[N_MAXPATH];
            char intern[N_MAXPATH];
            BuildInternalPath(entry, intern, N_MAXPATH);
            fs->MakeAbsolute(intern, absName, sizeof(absName));
            if (file->Open(entry->GetName(), "w"))
            {
                // copy bytes from npk into this file
                int start  = entry->GetFileOffset();
                int length = entry->GetFileLength();
                npkFile->Seek(start, nFile::START);
                char* buf = (char *) malloc(length);
                n_assert(0 != buf);
                npkFile->Read(buf, length);
                file->Write(buf, length);
            
                file->Close();
            }
            else
            {
                n_printf("Cannot open %s\n", absName);
            }
        
            delete file;
        }
    }

    if (nNpkTocEntry::DIR == type)
    {
        // create directory
        nDirectory* dir = fs->NewDirectoryObject();
        n_assert(0 != dir);

        char currentDir[N_MAXPATH];
        n_assert(N_MAXPATH >= strlen(fs->GetCwd()));
        sprintf(currentDir, fs->GetCwd());
        
        // use correct name or the one was given from outside?
        const char* dName = 0;
        if (0 != outName)
        {
            dName = outName;
        }
        else
        {   
            dName = entry->GetName();
        }
        n_assert(0 != dName);
        char absDir[N_MAXPATH];
        fs->MakeAbsolute(dName, absDir, sizeof(absDir));
        n_mkdir(dName);
        if (dir->Open(absDir))
        {
            fs->ChangeDir(dName);
        
            nNpkTocEntry* childEntry = 0;
            for (childEntry = entry->GetFirstEntry();
                 childEntry != 0;
                 childEntry = entry->GetNextEntry(childEntry))
            {
                // outName is for the highest level only, below
                // that we use correct name from npk file
                unPackFile(fs, npkFile, childEntry, 0, dList);
            }
        
            dir->Close();
            fs->ChangeDir(currentDir);
        }
        else 
        {
            n_printf("Cannot open dir %s\n", entry->GetName());
        }
        delete dir;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
unPack(nFileServer2* fs, const char* fileName, const char* outName, nList* dList)
{
    // create directory of the same name. check for existence and
    // ".npk" first
    if (0 == strstr(fileName, ".npk"))
    {
        n_printf("Error: %s is not an nebula pack file!\n", fileName);
        return;
    }
    
    char dirName[N_MAXPATH];
    strcpy(dirName, fileName);
    char* end = strstr(dirName, ".npk");
    *end = 0;
    strcat(dirName, ".n");
    
    char absFileName[N_MAXPATH];
    char rootPath[N_MAXPATH];
    nNpkFileWrapper fileWrapper;
    
    fs->MakeAbsolute(fileName, absFileName, sizeof(absFileName));
    getDirectoryName(absFileName, rootPath, sizeof(rootPath));
    
    if (fileWrapper.Open(fs, rootPath, absFileName))
    {
        // get table of contents object
        nNpkTocEntry* tocEntry = fileWrapper.GetTocObject().GetRootEntry();
        
        nFile* npkFile = fileWrapper.GetBinaryFile();
        unPackFile(fs, npkFile, tocEntry, outName, dList);       
        
        fileWrapper.Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
listDiff(nFileServer2* fs, 
         const char* oldName,
         const char* newName)
{
    nList diffList;
    GenerateDifferenceList(fs, oldName, newName, diffList);

    FileEntry* fEntry = 0;
    for (fEntry = (FileEntry *) diffList.GetHead();
         fEntry != 0;
         fEntry = (FileEntry *) fEntry->GetSucc())
    {
        char* wieso = "???";
        switch (fEntry->reason)
        {
            case FileEntry::DIFFERENT:
                wieso = "different content";
                break;
            case FileEntry::SIZE_CHANGED:
                wieso = "different size";
                break;
            case FileEntry::NOT_FOUND:
                wieso = "is new";
                break;
            case FileEntry::IDENTICAL:
                wieso = "identisch";
                break;
        }
        n_printf("- %s - %s\n", fEntry->name.Get(), wieso);            
    }

    fEntry = 0;
    while (0 != (fEntry = (FileEntry*) diffList.RemHead()))
    {
        delete fEntry;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
makeDiff(nFileServer2* fs, 
         const char* oldName,
         const char* newName,
         const char* outName)
{
    nList diffList;
    GenerateDifferenceList(fs, oldName, newName, diffList);
    
    // simple way: extract all files from diffList into a directory
    // and let pack the whole new directory with flohs routines
    unPack(fs, newName, "temp.n", &diffList);
    
    // kill all empty directories
    KillEmptyDirectories(fs, "temp.n");

    // pack "difference directory"
    if (0 == outName)
    {
        outName = "diff.npk";
    }
    packIt(fs, "temp.n", outName);

    // delete old temp-dir
    RemoveDir(fs, "temp.n");
    
    FileEntry* fEntry = 0;
    while (0 != (fEntry = (FileEntry*) diffList.RemHead()))
    {
        delete fEntry;
    }
}

//------------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    bool help, showDiff, diff;
    const char* packName;
    const char* listName;
    const char* outName;
    const char* oldName;
    const char* newName;
    const char* unPackName;

    // get args
    help       = wf_getboolarg(argc, argv, "-help");
    packName   = wf_getstrarg(argc, argv, "-pack", 0);
    listName   = wf_getstrarg(argc, argv, "-list", 0);
    outName    = wf_getstrarg(argc, argv, "-out", "pack.npk");
    diff       = wf_getboolarg(argc, argv, "-diff");
    showDiff   = wf_getboolarg(argc, argv, "-listdiff");
    oldName    = wf_getstrarg(argc, argv, "-old", 0);
    newName    = wf_getstrarg(argc, argv, "-new", 0);
    unPackName = wf_getstrarg(argc, argv, "-unpack", 0);

    // show help
    if (help)
    {
        printf("npacknpk [-help]\n"
               "(C) 2002 RadonLabs GmbH\n"
               "-help       show help\n"
               "-pack       pack this input directory\n"
               "-list       list contents of npk files\n"
               "-out        name of output file (def pack.npk)\n"
               "-diff       build upgrade file of two npk files\n"
               "-listdiff   list difference of two npk files\n"
               "-old        the 'older' npk file to compare\n"
               "-new        the 'newer' npk file to compare\n"
               "-unpack     unpack given npk file\n");
        return 0;
    }

    // create Nebula environment
    nKernelServer* ks = new nKernelServer;
    nFileServer2* fs = (nFileServer2*) ks->New("nfileserver2", "/sys/servers/file2");

    // what operation to do?
    if (packName)
    {
        // call pack function
        if (!packIt(fs, packName, outName))
        {
            n_printf("ERROR IN FILE GENERATION, DELETING NPK FILE\n");
            char absOutName[N_MAXPATH];
            fs->MakeAbsolute(outName, absOutName, sizeof(absOutName));
            remove(absOutName);
        }
    }
    else 
    {
        // do not pack. hm. any other action?
        if (diff)
        {
            n_assert((0 != oldName) && (0 != newName));
            
            // compare two files and build a new file with the difference
            makeDiff(fs, oldName, newName, outName);
        }
        else
        {
            // Show content only
            if (listName)
            {
                // call list function
                listIt(fs, listName);
            }
            else
            {
                // list differnce of two npk files
                if (showDiff)
                {
                    n_assert((0 != oldName) && (0 != newName));
                    
                    listDiff(fs, oldName, newName);
                }
                else
                {
                    if (0 != unPackName)
                    {
                        unPack(fs, unPackName, outName, 0);
                    }
                }
            }
        }
    }

    delete ks;

    // success
    return 0;
}







