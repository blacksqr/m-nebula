#ifndef N_FILESERVER2_H
#define N_FILESERVER2_H
//------------------------------------------------------------------------------
/**
    @class nFileServer2
    @ingroup FileServices

    @brief new fileserver, provides functions for creating file and
    directory objects and assigns

    <h2>File Server Initialization</h2>

    There are currently two file servers, nFileServer and nFileServer2.
    nFileServer is created by the kernel server, but nFileServer2 must
    be manually initialized and shut down. In general you should use
    nFileServer2, as the file I/O portions of nFileServer will be removed
    when it is converted to being @c nPersistServer.
    
    It is also possible to subclass nFileServer2 to add additional or
    altered functionality, nNpkFileServer is an example of this.

    @note nFileServer is indeed going away and most things in Nebula
    should currently be using nFileServer2 already.

@code
    nFileServer2 * fileServer2;

    fileServer2 = (nFileServer2*)kernelServer->New("nfileserver2",
                                                   "/sys/servers/file2");
@endcode

    <h2>File Server Usage</h2>

    In order to read and write files a nFile object should be created.

@code
    nFile* file = fileServer2->NewFileObject();
@endcode

    The nFile object can then be opened and read from.  The nFile object
    that is returned may be a subclass of nFile, such as nNpkFile or nZipFile,
    if you are using one of the more specialized nFileServer2 implementations.

    @note File output is not supported with nZipFile.

@code
    // file input
    if (file->Open(fileName, "rb"))
    {
        void* buf = n_malloc(dataSize);
        int num = file->Read(buf, dataSize);
    }
    file->Close()

    // file output
    if (file->Open("todo.txt", "w"))
    {
        file->PutS("Must write scene graph demo\n");
    }
    file->Close()
@endcode

    The nDirectory class can be used to search directories.

@code
    nDirectory* folder = fileServer2->NewDirectoryObject();
    if(folder->Open(dir))
        if(!folder->IsEmpty()) {
            do {
                entryName = folder->GetEntryName();
                if(folder->GetEntryType() == nDirectory::FILE) {
                    ...
                }
            } while (folder->SetToNextEntry());
        }
    }
    folder->Close();
@endcode

    When using a more specialized nFileServer2 implementation, the nDirectory
    object returned may be a more specialized subclass.

    <h2>Assigns</h2>

    Assigns provide a way to isolate system-specific paths from a path or
    filename.  They are used in a path as an alias, which gets mapped
    to a new absolute path.  For example, with an assign @c "data" that maps to
    @c "/usr/local/share/nebula":

@verbatim
    "data:vehicle/vehicle.n3d"
@endverbatim

    would map to:

@verbatim
    "/usr/local/share/nebula/vehicle/vehicle.n3d"
@endverbatim

    Assigns can be read and written with both nFileServer and nFileServer2:

      - nFileServer::GetAssign()
      - nFileServer::SetAssign()
      - nFileServer2::GetAssign()
      - nFileServer2::SetAssign()

    Assigns are stored as nEnv variables within @c /sys/share/assigns/
    in the Nebula object hierarchy.  By default, two assigns exist:

@verbatim
    /sys/share/assigns/home  - Nebula's home directory
    /sys/share/assigns/bin   - Nebula's application directory
@endverbatim

    @todo Talk about how to mangle paths to process assigns and so on.

    <h2>Shutdown</h2>

    As with initialization, only the nFileServer2 must be explicitly shutdown
    as the nFileServer is managed by nKernelServer.

@code
    if (fileServer2 != NULL)
        fileServer2->Release();
    fileServer2 = NULL;
@endcode

    (C) 2002 RadonLabs GmbH
*/

#include "kernel/nroot.h"
#include "kernel/nfile.h"
#include "kernel/ndirectory.h"
#include "kernel/nref.h"


// 09-Dec-2007 by Ilya - to control nFiles
#include "util/nstl.h"

#undef N_DEFINES
#define N_DEFINES nFileServer2
#include "kernel/ndefdllclass.h"

#ifdef __WIN32__
#   include <direct.h>
#    ifndef getcwd
#        define getcwd      _getcwd
#    endif
#else
#   include <unistd.h>
#endif


class nDirectory;
//------------------------------------------------------------------------------
class N_PUBLIC nFileServer2 : public nRoot
{
public:
    /// constructor
    nFileServer2();
    /// destructor
    virtual ~nFileServer2();

    /** @name Assigns and Path management
 
        See @ref fs_assigns for more information on assigns and how
        they are used.
    */
    //@{
    /// sets a path alias
    virtual bool SetAssign(const char* assignName, const char* pathName);
    /// gets a path alias
    virtual const char* GetAssign(const char* assignName);
    /// expand path alias to real path
    virtual const char* ManglePath(const char* pathName, stl_string& res);
	/// makes a path absolute
    virtual void MakeAbsolute(stl_string& path);
	/// expand path alias to real path and make it absolute
    virtual const char* MakeAbsoluteMangled(const char* pathName, stl_string& res);
	/// cleanup a path name in place
	void CleanupPathName(stl_string& path);

	/// check if given path is absolute
	virtual bool IsAbsolute(const char* path) {
		return ((path && strlen(path)) ? (path[0] == this->GetPathDelim() || path[1] == ':') : 0);
	}    
    /// set current working dir
    virtual bool ChangeDir(const char* newDir);
    /// get current working dir
    virtual const char* GetCwd();
	/// push current location
	virtual void PushCwd() { this->cwd_stack.push(this->GetCwd()); }
	/// pop latest location
	virtual void PopCwd() 
	{ 
		if (!this->cwd_stack.empty())
		{
			this->ChangeDir(this->cwd_stack.top().c_str());
			this->cwd_stack.pop(); 
		}
	}
    //@}

	virtual char GetPathDelim() const 
	{
#if defined(__LINUX__)
		return '/';
#elif defined(__WIN32__)
		return '\\';
#elif defined(__MACOSX__)
		return '/';
#endif
	}

    /// check if file exists, + 27-01-2003, Ilya Clockwise;
    virtual bool FileExists(const char* path);
	/// return file size or -1 if file does not exists, + 01-01-2009, Ilya 
    virtual int FileSize(const char* path);
    /// set developer mode
    virtual void SetDevMode(bool mode) { this->dev_mode = mode; }
    /// get developer mode
    virtual bool GetDevMode() { return this->dev_mode; }

    /// creates a new nDirectory object
    virtual nDirectory* NewDirectoryObject();
    /// creates a new nFile object
    virtual nFile* NewFileObject();

    /** @name Statistics
        Statistics gathering for tracking how much I/O is going on.
    */
    //@{
    /// reset statistics
    void ResetStatistics();
    /// add read bytes 
    void AddBytesRead(int b);
    /// add written bytes
    void AddBytesWritten(int b);
    /// add seek operation to stats
    void AddSeek();
    /// get bytes read since last reset
    int GetBytesRead() const;
    /// get bytes written since last reset
    int GetBytesWritten() const;
    /// get number of seeks
    int GetNumSeeks() const;
    //@}

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;


// 09-Dec-2007 by Ilya - to control nFiles	
public:	
	/// returns nFile* by index;
	virtual nFile*	GetFileByIndex(int idx) {
		if (idx < (int)this->files.size()) {
			return this->files[idx];
		} else {
			return 0;
		}
	}
	/// adds file to vector
	virtual nFile*	AddFile(nFile* f);
	/// removes pointer from files vector and adds to free cells the index
	virtual void FreeFile(int idx);
protected:
	/// find new index for file from freecells or create a new one
	virtual int		GetNewFileIndex() const;

protected:
	/// moved here from nfileserver
    void initHomeAssign();
    void initBinAssign();

    /// pointer to nebula-directory with assigns
    nRef<nRoot> assignDir;
    /// current working directory
    stl_string cwd;
    /// statistics: bytes read
    int bytesRead;
    /// statistics: bytes written
    int bytesWritten;
    /// statistics: number of seeks
    int numSeeks;
    /// developer mode flag;
    bool dev_mode;

	// 09-Dec-2007 by Ilya - to control nFiles
	/// vector of nFile pointers to control correctness
	std::vector<nFile*> files;
	/// list of removed files indexse to keep vector above full
	std::list<int>		freecells;	
	std::stack<stl_string> cwd_stack;
};
//------------------------------------------------------------------------------
#endif
