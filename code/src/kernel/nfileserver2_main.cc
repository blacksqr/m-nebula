#define N_IMPLEMENTS nFileServer2
#define N_KERNEL
//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"

#if defined(__MACOSX__)
#include <Carbon/carbon.h>
#endif

#undef n_init
#undef n_fini
#undef n_create
#define n_init   nfileserver2_init
#define n_fini   nfileserver2_fini
#define n_create nfileserver2_create

nNebulaScriptClass(nFileServer2, "nroot");

//------------------------------------------------------------------------------
/**

	history:
    - 30-Jan-2002   peter    created
*/
nFileServer2::nFileServer2() :
    assignDir(this),
    bytesRead(0),
    bytesWritten(0),
    numSeeks(0),
#ifdef __NEBULA_NO_ASSERT__
    dev_mode(false)
#else
    dev_mode(true)
#endif
{
    this->assignDir = kernelServer->Lookup("/sys/share/assigns");
    if (!this->assignDir.isvalid())
    {
        this->assignDir = kernelServer->New("nroot","/sys/share/assigns");
    }
	this->files.clear();	
	this->freecells.clear();
    this->initHomeAssign();
    this->initBinAssign();
	this->files.push_back(0); // to avoid 0 index of file
}

//------------------------------------------------------------------------------
/**

	history:
    - 30-Jan-2002   peter    created
*/
nFileServer2::~nFileServer2()
{
    // note: keep assigns valid

	for (std::vector<nFile*>::iterator i = this->files.begin(); i != this->files.end(); i++) {
		nFile* f = *i;
		if (f) {			
			n_printf("undeleted file found at %d\n" ,f->GetIndex());
			delete f;			
		}
	}
}

//------------------------------------------------------------------------------
/**
    Reset statistics.
*/
void
nFileServer2::ResetStatistics()
{
    this->bytesRead = 0;
    this->bytesWritten = 0;
    this->numSeeks = 0;
}

//------------------------------------------------------------------------------
/**
    Add read bytes to statistics.
*/
void
nFileServer2::AddBytesRead(int b)
{
    this->bytesRead += b;
}

//------------------------------------------------------------------------------
/**
    Add read bytes written to statistics.
*/
void
nFileServer2::AddBytesWritten(int b)
{
    this->bytesWritten += b;
}

//------------------------------------------------------------------------------
/**
    Add a seek op to stats.
*/
void
nFileServer2::AddSeek()
{
    this->numSeeks++;
}

//------------------------------------------------------------------------------
/**
    Get bytes read since last ResetStatistics()
*/
int
nFileServer2::GetBytesRead() const
{
    return this->bytesRead;
}

//------------------------------------------------------------------------------
/**
    Get bytes written since last ResetStatistics()
*/
int
nFileServer2::GetBytesWritten() const
{
    return this->bytesWritten;
}

//------------------------------------------------------------------------------
/**
    Get number of seeks since last ResetStatistics().
*/
int
nFileServer2::GetNumSeeks() const
{
    return this->numSeeks;
}

//------------------------------------------------------------------------------
/**
	creates new or modifies existing assign under /sys/share/assigns

    @param assignName   the name of the assign
    @param pathName		the path to which the assign links

	history:
    - 30-Jan-2002   peter    created
*/
bool
nFileServer2::SetAssign(const char* assignName, const char* pathName)
{
	if (pathName[strlen(pathName)-1] != '/') {
        n_printf("path must end with a '/'\n");
        return false;
    }
        
    // ex. das Assign schon?
    kernelServer->PushCwd(this->assignDir.get());
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (!env) {
        env = (nEnv *) kernelServer->New("nenv", assignName);
        if (!env) n_error("Could not create assign object!");
    }
    env->SetS(pathName);
    kernelServer->PopCwd();
    return true;

}

//------------------------------------------------------------------------------
/**
    queries existing assign under /sys/share/assigns

    @param assignName   the name of the assign
    @return				the path to which the assign links

	history:
    - 30-Jan-2002   peter    created
*/
const char*
nFileServer2::GetAssign(const char* assignName)
{
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (env) 
		return env->GetS();
    else 
	{
        n_printf("Assign '%s' not defined!\n",assignName);
        return NULL;
    }
}

//------------------------------------------------------------------------------
/**
    Cleanup the path name in place (replace any backslashes with slashes),
    and removes a trailing slash if exists.
*/
void
nFileServer2::CleanupPathName(stl_string& path)
{
	std::replace(path.begin(), path.end(), '\\', '/');

	size_t len = path.size();
    if (len && path[len-1] == '/')
		path.erase(len-1);
}

//------------------------------------------------------------------------------
/**
	makes an absolute path out of a relative one
	@param path		the relative path (may already be absolute)
	@param res		result string to store absolute path in
	
	history:
    - 06-Feb-2002   peter    created
*/
void
nFileServer2::MakeAbsolute(stl_string& path)
{
	if (!this->IsAbsolute(path.c_str()))
    {
		stl_string pref(this->GetCwd());
		pref += '/';
		path.insert(0, pref);
    } 
    this->CleanupPathName(path);
}

//------------------------------------------------------------------------------
/**
    Expands assign in path to full absolute path, replaces any backslashes
    by slashes, and returns any trailing slash, and makes the path absolute.
	  
	@param pathName		the path to expand
	@param res			string for result	
*/
const char* 
nFileServer2::ManglePath(const char* pathName, stl_string& res) //char* buf, int bufSize)
{
	const char* colon = strchr(pathName, ':');
	size_t pos = colon - pathName;
	if (colon && pos > 1)
	{
		res = pathName;		
		stl_string assign(res.substr(0, pos));
		stl_string tail(res.substr(pos + 1));
		bool mangled = false;
		const char* assign_ptr = this->GetAssign(assign.c_str());
		while (assign_ptr)
        {
			mangled = true;
			assign = assign_ptr;			
			size_t pos = assign.find_first_of(':');
			assign_ptr = (pos > 1 && pos != stl_string::npos ? this->GetAssign(assign.c_str()) : 0);
        }
		if (mangled)
			res = assign + tail;
    }
	else
	{
		res = pathName;
    }
    
    this->CleanupPathName(res);  
	return res.c_str();
}

//------------------------------------------------------------------------------
/**
	expand path alias to real path and make it absolute
	@param path		the relative path (may already be absolute)
	@param res		result string to store absolute path in
	
	history:
    - 02-Jan-2009   Ilya    created
*/
const char* nFileServer2::MakeAbsoluteMangled(const char* pathName, stl_string& res)
{	
	this->ManglePath(pathName, res);
	this->MakeAbsolute(res);
	return res.c_str();
}

//------------------------------------------------------------------------------
/**
	creates a new nDirectory object

    @return          the nDirectory object

	history:
    - 30-Jan-2002   peter    created
*/
nDirectory* 
nFileServer2::NewDirectoryObject()
{
	return new nDirectory(this);
}

//------------------------------------------------------------------------------
/**

	find new index for file from freecells or create a new one
	@return          the index found
	- 09-Dec-2007 by Ilya 
*/
int 
nFileServer2::GetNewFileIndex() const {
	int idx = 0;
	if (this->freecells.size()) {
		idx = this->freecells.back();		
	} else {
		idx = this->files.size();
	}
	return idx;
}


//------------------------------------------------------------------------------
/**

	adds file to vector
	@return          pointer to file
	- 09-Dec-2007 by Ilya 
*/ 
nFile* 
nFileServer2::AddFile(nFile* f) {
	if (f) {
		int idx = this->GetNewFileIndex();
		f->SetIndex(idx);
		if (idx < (int)this->files.size()) {
			this->files[idx] = f;
			this->freecells.pop_back();
		} else {
			this->files.push_back(f);
		}
	}
	return f;
}

//------------------------------------------------------------------------------
/**
	creates a new nFile object
  
	@return          the nFile object

	history:
    - 30-Jan-2002   peter    created
*/
nFile*
nFileServer2::NewFileObject() {
	return new nFile(this);
}

//------------------------------------------------------------------------------
/**
	removes pointer from files vector
  
	@return          none

	history:
    09-Dec-2007 by Ilya - to control nFile* list
*/
void 
nFileServer2::FreeFile(int idx) {
	if (idx < (int)this->files.size()) {
		this->freecells.push_back(idx);
		this->files[idx] = 0;
	} else {
		n_printf("Invalid file index %d\n", idx);
	}
}

//------------------------------------------------------------------------------
/**
    Change current working dir.

    @param  newDir      path to new working dir, may contain assigns
    @return             true or false
*/
bool
nFileServer2::ChangeDir(const char* newDir)
{
    // change to working dir
	stl_string new_dir;		
	this->MakeAbsoluteMangled(newDir, new_dir);
    int result = _chdir(new_dir.c_str());
    return (result == 0);
}

//------------------------------------------------------------------------------
/**
    Return path to current working dir.

    @return     path to current working dir
*/
const char*
nFileServer2::GetCwd()
{
    char buf[N_MAXPATH*2];
    getcwd(buf, sizeof(buf));
    this->cwd = buf;
	this->CleanupPathName(cwd);
    return this->cwd.c_str();
}

//-------------------------------------------------------------------
//  initHomeAssign()
//  Initialize Nebula's home directory assign ("home:").
//  10-Aug-99   floh    created
//-------------------------------------------------------------------
void nFileServer2::initHomeAssign(void)
{
    char buf[N_MAXPATH];
#ifdef __WIN32__
    
    // Win32: try to find the nkernel.dll module handle's filename
    // and cut off the last 2 directories
    HMODULE hmod = GetModuleHandle("nkernel.dll");
    DWORD res = GetModuleFileName(hmod,buf,sizeof(buf));
    if (res == 0) {
        n_printf("nFileServer2::initHomeAssign(): GetModuleFileName() failed!\n");
    }

    // "x\y\bin\win32\xxx.exe" -> "x\y\"
    int i;
    char c;
    char *p;
    for (i=0; i<3; i++) 
    {
        p = strrchr(buf,'\\');
        n_assert(p);
        p[0] = 0;
    }
    
    // convert all backslashes to slashes
    p = buf;
    while ((c = *p)) 
    {
        if (c == '\\') *p = '/';
        p++;
    }
    strcat(buf,"/");
    
#elif defined(__LINUX__)
    
    // under Linux, the NEBULADIR environment variable must be set,
    // otherwise the current working directory will be used
    char *s = getenv("NEBULADIR");
    if (s) 
    {
        n_strncpy2(buf,s,sizeof(buf));
    } else {
        n_printf("Env variable NEBULADIR not set, assuming cwd as nebula home directory!\n");
        if (!getcwd(buf,sizeof(buf))) {
            n_error("Arrgh, getcwd() failed... bailing out!\n");
        }
    }
    // if last char is not a /, append one
    if (buf[strlen(buf)] != '/') strcat(buf,"/");

#elif defined(__MACOSX__)
    // isn't there an easer way?
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef bundleURL = CFBundleCopyBundleURL( mainBundle );
    FSRef bundleFSRef;
    CFURLGetFSRef( bundleURL, &bundleFSRef );
    FSRefMakePath( &bundleFSRef, (unsigned char*)buf, N_MAXPATH );	
    strcat( buf, "/Contents/Resources/" );
#else
#error nFileServer2::initHomeAssign() not implemented!
    // n_error("nPersistServer::initHomeAssign not implemented!\n");
#endif
    
    // finally, set the assign
    this->SetAssign("home",buf);
}

//-------------------------------------------------------------------
//  initBinAssign()
//  10-Aug-99   floh    created
//-------------------------------------------------------------------
void nFileServer2::initBinAssign(void)
{    
    stl_string home_dir = this->GetAssign("home");    

#ifdef __WIN32__
    home_dir += "bin/win32";
#elif defined(__LINUX__)
    home_dir += "bin/linux";
#elif defined(__MACOSX__)
    home_dir += "bin/macosx";
#else
#error nFileServer2::initBinAssign() not implemented!
    n_error("nFileServer2::initBinAssign() not implemented!\n");
#endif

    // append 'd' to bin: in debug builds.
#ifdef _DEBUG
    home_dir += "d/";
#else
    home_dir += "/";
#endif

    this->SetAssign("bin", home_dir.c_str());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    check if file exists, + 27-01-2003, Ilya Clockwise;
    @return     true or false
*/
bool nFileServer2::FileExists(const char* path) {
    nFile* test = this->NewFileObject();
    if (test->Exists(path)) {        
        delete test;
        return true;
    }
    delete test;
    return false;
}

//------------------------------------------------------------------------------
/**
    check if file exists, + 01-01-2003, Ilya 
    @return     size of file or -1 if file does not exists
*/
int nFileServer2::FileSize(const char* path) {
    
	int size = -1;
	nFile* test = this->NewFileObject();
	if (test->Open(path, "rb"))
	{
		size = test->Size();
		test->Close();
	}
    delete test;
    return size;
}
//------------------------------------------------------------------------------
