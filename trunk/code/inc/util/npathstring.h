#ifndef N_PATHSTRING_H
#define N_PATHSTRING_H
//------------------------------------------------------------------------------
/**
    A special stl_string subclass for file paths.

    (C) 2002 RadonLabs GmbH
*/

#include "util/nstl.h"


//------------------------------------------------------------------------------
class nPathString : public stl_string
{
public:
    /// default constructor
	nPathString() {}
    /// constructor 1
	nPathString(const char* str) : stl_string(str) {}
    /// copy constructor
	nPathString(const nPathString& rhs) : stl_string(rhs.c_str()) {}
    /// = operator
	nPathString& operator=(const nPathString& rhs) { this->assign(rhs.c_str()); }
    /// = operator with string
    nPathString& operator=(const char* rhs) { this->assign(rhs); return *this;}

    /// get pointer to extension (without the dot)
    const char* GetExtension() const;
    /// check if extension matches (no dot in the extension!)
    bool CheckExtension(const char* ext) const;
    /// convert backslashes to slashes
    void ConvertBackslashes();
    /// remove extension
    void StripExtension();
    /// extract the part after the last directory separator
    nPathString ExtractFileName() const;
    /// extract the part before the last directory separator
    nPathString ExtractDirName() const;

private:
    /// get pointer to last directory separator
    const char* GetLastSlash() const;
};


//------------------------------------------------------------------------------
/**
    06-Jan-03   floh    renamed to something more intuitive
*/
inline
void
nPathString::ConvertBackslashes()
{
	std::replace(this->begin(), this->end(), '\\', '/');    
}

//------------------------------------------------------------------------------
/**
    @return     pointer to extension (without the dot), or 0
*/
inline
const char*
nPathString::GetExtension() const
{
    const char* ext = strrchr(this->c_str(), '.');
    if (ext)
    {
        ext++;
        if (ext[0])
        {
            return ext;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Returns true if file extension matches.

    @param  ext     extension string (without the dot)
    @return         true if extension matches
*/
inline
bool
nPathString::CheckExtension(const char* ext) const
{
    n_assert(ext);
    const char* extStr = this->GetExtension();
    if (extStr)
    {
        return (0 == (strcmp(ext, extStr)));
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Remove the file extension.
*/
inline
void
nPathString::StripExtension()
{
	size_t pos = this->find_last_of('.');
	if (pos != stl_string::npos)
		this->erase(pos);
}

//------------------------------------------------------------------------------
/**
    Get a pointer to the last directory separator.
*/
inline
const char*
nPathString::GetLastSlash() const
{
    const char* s = this->c_str();
    const char* lastSlash = strrchr(s, '/');
    if (!lastSlash) lastSlash = strrchr(s, '\\');
    if (!lastSlash) lastSlash = strrchr(s, ':');
    return lastSlash;
}

//------------------------------------------------------------------------------
/**
    Return a nPathString object containing the part after the last
    path separator.
*/
inline
nPathString
nPathString::ExtractFileName() const
{
    nPathString pathString;
    const char* lastSlash = this->GetLastSlash(); 
    if (lastSlash)
    {
        pathString = &(lastSlash[1]);
    }
    else
    {
        pathString = this->c_str();
    }
    return pathString;
}

//------------------------------------------------------------------------------
/**
    Return a nPathString object containing the part before the last
    directory separator.
*/
inline
nPathString
nPathString::ExtractDirName() const
{
    nPathString pathString(*this);
    const char* lastSlash = pathString.GetLastSlash();
    if (lastSlash)
    {
        size_t pos = lastSlash - pathString.c_str();
		pathString.erase(pos);
    }
    return pathString;
}

//------------------------------------------------------------------------------
#endif

