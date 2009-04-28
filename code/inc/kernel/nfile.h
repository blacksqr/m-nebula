#ifndef N_FILE_H
#define N_FILE_H
//------------------------------------------------------------------------------
/**
    @class nFile
    @ingroup FileServices

    @brief wrapper for file routines

    nFile provides functions for reading and writing files.  For usage
    information, see nFileServer2.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifdef __WIN32__
#include <windows.h>
#include <winbase.h>
#else
#include <stdio.h>
#endif

#undef N_DEFINES
#define N_DEFINES nFile
#include "kernel/ndefdllclass.h"

class nFileServer2;
//------------------------------------------------------------------------------
class N_PUBLIC nFile
{
public:
    /// start point for seeking in file
    enum nSeekType
    {
        CURRENT,
        START,
        END,
    };

    /// constructor
    nFile(nFileServer2* server);
    /// destructor
    virtual ~nFile();

    /**
        @name Basic Operations
    */
    //@{
    /// opens a file
    virtual bool Open(const char* fileName, const char* accessMode);
    /// determines wether the file is opened
    virtual bool IsOpen() const;
    /// closes the file
    virtual void Close();
    /// writes some bytes to the file
    virtual int Write(const void* buffer, int numBytes);
    /// reads some bytes from the file
    virtual int Read(void* buffer, int numBytes) const;
    /// gets actual position in file
    virtual int Tell() const;
    /// sets new position in file
    virtual bool Seek(int byteOffset, nSeekType origin) const;
	/// returns size of the file in bytes
	virtual int Size() const;

    //@}

    /**
        @name Data Storage Utility Methods
    */
    //@{
    /// writes a string to the file
    virtual bool PutS(const char* buffer);
    /// reads a string from the file
    virtual bool GetS(char* buffer, int numChars);

	/// writes a unocode string to the file
	virtual bool PutU(const wchar* buffer);
	/// reads a unicode string from the file
	virtual bool GetU(wchar* buffer, int numChars);

    /// check if file exists, + 27-Jan-2003, Ilya Clockwise;
    virtual bool Exists(const char* path);

    /// write a 32bit int to the file
    int PutInt(int val);
    /// write a 16bit int to the file
    int PutShort(short val);
    /// write a 8bit int to the file
    int PutChar(char val);
    /// write a float to the file
    int PutFloat(float val);
    /// read a 32 bit int from the file
    int GetInt(int& val);
    /// read a 16 bit int from the file
    int GetShort(short& val);
    /// read a 8 bit int from the file
    int GetChar(char& val);
    /// read a float from the file
    int GetFloat(float& val);
    //@}

	// 09-Dec-2007 by Ilya - to control nFiles
	/// returns index in fs vector of nFile pointers
	int GetIndex() const {return this->index;}

	/// set index (only once)
	void SetIndex(int idx) {
		n_assert(this->index == -1);
		this->index = idx;
	}

protected:	
    /// pointer to fileserver
    nFileServer2* fs;
	
	// 09-Dec-2007 by Ilya - to control nFiles
	/// fs index
	int	index;

    #ifdef __WIN32__
        // win32 file handle
        HANDLE handle;
    #else
        // ansi c file pointer
        FILE* fp;
    #endif
};

//------------------------------------------------------------------------------
/**
   @return           true if the file is open, false if it isn't.
*/
inline
bool
nFile::IsOpen() const
{
#ifdef __WIN32__
    return (0 != this->handle);
#else
    return (0 != this->fp);
#endif
}

//------------------------------------------------------------------------------
/**
    @param   val    a 32 bit int value
    @return         number of bytes written
*/
inline
int
nFile::PutInt(int val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     a 16 bit int value
    @return         number of bytes written
*/
inline
int
nFile::PutShort(short val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     a 8 bit int value
    @return         number of bytes written
*/
inline
int
nFile::PutChar(char val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     a 32 bit float value
    @return         number of bytes written
*/
inline
int
nFile::PutFloat(float val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     [out] 32 bit int value
    @return         number of bytes read
*/
inline
int
nFile::GetInt(int& val)
{
    return this->Read(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     [out] 16 bit int value
    @return         number of bytes read
*/
inline
int
nFile::GetShort(short& val)
{
    return this->Read(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     [out] 8 bit int value
    @return         number of bytes read
*/
inline
int
nFile::GetChar(char& val)
{
    return this->Read(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     [out] 32 bit float value
    @return         number of bytes read
*/
inline
int
nFile::GetFloat(float& val)
{
    return this->Read(&val, sizeof(val));
}

//------------------------------------------------------------------------------
#endif
