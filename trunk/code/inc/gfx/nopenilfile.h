#ifndef N_OPENILFILE_H
#define N_OPENILFILE_H

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#include "il/il.h"
#include "il/ilu.h"

inline ILenum GetILType(const char* fname) {
    const char* ext = strrchr(fname, '.');
    if (ext) {            
        if (!_stricmp(ext, ".dds"))
            return IL_DDS;             
        else if (!_stricmp(ext, ".jpg"))   
            return IL_JPG;             
        else if (!_stricmp(ext, ".tga"))
            return IL_TGA;
        else if (!_stricmp(ext, ".bmp"))
            return IL_BMP;            
        else if (!_stricmp(ext, ".ico"))
            return IL_ICO;            
        else if (!_stricmp(ext, ".lbm"))
            return IL_LBM;             
        else if (!_stricmp(ext, ".pcx"))
            return IL_PCX; 
        else if (!_stricmp(ext, ".pic"))
            return IL_PIC; 
        else if (!_stricmp(ext, ".png"))
            return IL_PNG; 
        else if (!_stricmp(ext, ".pnm"))
            return IL_PNM;                         
        else if (!_stricmp(ext, ".tif"))
            return IL_TIF;
        else if (!_stricmp(ext, ".tiff"))
            return IL_TIF;
        else if (!_stricmp(ext, ".raw"))
            return IL_RAW;             
        else if (!_stricmp(ext, ".mng"))
            return IL_MNG; 
        else if (!_stricmp(ext, ".jng"))
            return IL_JNG;                         
        else if (!_stricmp(ext, ".psd"))
            return IL_PSD;             
        else if (!_stricmp(ext, ".xpm"))
            return IL_XPM; 
        else   
            return IL_TYPE_UNKNOWN;
    } else {
        return IL_TYPE_UNKNOWN;
    }
}

#include "util/nstl.h"

//------------------------------------------------------------------------------
/**
    nOpenILFile class
    wraps around nFile and devil
*/
// return type of the file

class nOpenILFile {
public:
    nOpenILFile(nKernelServer* ks) : 
        kernelServer(ks), 
        refFileServer(ks, 0) 
    {
          this->refFileServer = "/sys/servers/file2";
          this->data.clear();
          this->file = 0;
    }

    virtual ~nOpenILFile() {
        if (this->file) { 
            if (this->file->IsOpen())
                this->file->Close();
            delete this->file;
            this->file = 0;
        }
        this->data.clear();
    }    

    /// open texture file;
    bool Open(const char* fname) {
        if (!file)
            file = this->refFileServer->NewFileObject();
        
        if (file->Open(fname, "rb")) {
            return true;
        }
        return false;
    }

    /// get pointer to file
    nFile* GetFile() {
        return this->file;
    }

    /// read the whole file
    long ReadData() {
        if (this->file) {
            if (this->file->IsOpen()) {
                this->file->Seek(0, nFile::START);
                this->file->Seek(0, nFile::END);

                long size = file->Tell();

                this->file->Seek(0, nFile::START);

                this->data.resize(size);
                long redsize = this->file->Read(&this->data[0], size);
                return redsize;
            }
            return -1;
        }
        return -1;
    }

    /// close and delete the data
    void Close() {
        if (this->file) { 
            if (this->file->IsOpen())
                this->file->Close();            
        }
    }

    /// get the data
    void* GetData() {
        return &data[0];
    }

    /// get data size
    long GetSize() {
        return data.size();
    }

private:
    nFile*                  file;
    byte_buffer             data;

    nKernelServer*          kernelServer;
    nAutoRef<nFileServer2>  refFileServer;		
}; // nOpenILFile
#endif
