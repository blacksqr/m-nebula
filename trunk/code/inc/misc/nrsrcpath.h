#ifndef N_RSRCPATH_H
#define N_RSRCPATH_H
//-------------------------------------------------------------------
/**
    @class nRsrcPath

    @brief smart resource file locator

    Encapsulates a path name to a resource which may or may
    not be relative to the current working directory or may
    contain a fileserver assign. The pathname is absolutized
    and stored in the object.
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_PERSISTSERVER_H
#include "kernel/npersistserver.h"
#endif

#include "util/nstl.h"

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
class nRsrcPath 
{
public:
    /// clear paths
    void Clear(void); 
    /// set paths
    void Set(nFileServer2* fs, const char *_name);
    /// get original path
    const char *GetPath(void) const;
    /// get absolute path
    const char *GetAbsPath(void) const;

private:
    stl_string origName;   // original filename
    stl_string absName;    // absolutized filename
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRsrcPath::Clear()
{
    this->origName.clear();
    this->absName.clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRsrcPath::Set(nFileServer2* fs, const char* name) 
{
    if (name)
    {
        this->origName = name;
		fs->MakeAbsoluteMangled(name, this->absName);
    }
    else
    {
		this->Clear();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRsrcPath::GetPath() const
{
    return this->origName.empty() ? 0 : this->origName.c_str();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char* 
nRsrcPath::GetAbsPath() const
{
    return this->absName.empty() ? 0 : this->absName.c_str();
}

//-------------------------------------------------------------------
#endif

