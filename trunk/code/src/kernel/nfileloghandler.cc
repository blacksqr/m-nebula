#define N_IMPLEMENTS nFileLogHandler
#define N_KERNEL
//------------------------------------------------------------------------------
//  nfileloghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfileloghandler.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "util/npathstring.h"

//------------------------------------------------------------------------------
/**
    The constructor takes an application name which is used to name the
    log file.  It is expected that the kernel has been initialized at
    this point.

    @param  app     an application name
*/
nFileLogHandler::nFileLogHandler(const char* logname) :
    logFile(0)
{
    this->logName = logname;
	stl_string mangled;
    // obtain app directory
    nPathString logFileName;
    //We assume we have a kernel server up at this point
    if (nKernelServer::ks)
    {
		nFileServer2* fs = nKernelServer::ks->GetFileServer2();
		if (!fs->IsAbsolute(logname))
		{
			fs->ManglePath("home:", mangled);
			logFileName += mangled;
			logFileName += "/";
		}
    }
    logFileName += logName.c_str();
    logFileName += ".log";

    // low level file access functions since we cannot guarantee that
    // a file server exists!
    this->logFile = fopen(logFileName.c_str(), "w");
    
    // to prevent a infinite loop on error, we cannot simply call n_assert(),
    // since this would in turn ourselves, so handle the error message
    // with our own weapons
    if (!this->logFile)
    {
        n_error("nFileLogHandler: could not open log file!\n");		
        exit(10);
    }
}

//------------------------------------------------------------------------------
/**
*/
nFileLogHandler::~nFileLogHandler()
{
    fclose(this->logFile);
    this->logFile = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nFileLogHandler::Print(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nFileLogHandler::Message(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nFileLogHandler::Error(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
    fflush(this->logFile);
}

