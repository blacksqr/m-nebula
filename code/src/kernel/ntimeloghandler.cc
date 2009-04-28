#define N_IMPLEMENTS nTimeLogHandler
#define N_KERNEL
//------------------------------------------------------------------------------
//  ntimeloghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <sys\timeb.h>
#include <time.h>
#include "kernel/ntimeloghandler.h"

stl_string nTimeLogHandler::buffer;

//------------------------------------------------------------------------------
/**
    The constructor takes an application name which is used to name the
    log file.  It is expected that the kernel has been initialized at
    this point.

    @param  app     an application name
*/
nTimeLogHandler::nTimeLogHandler(const char* logname) :
    nFileLogHandler(nTimeLogHandler::conv_file_name(logname))
{ 
}

//------------------------------------------------------------------------------
/**
*/
nTimeLogHandler::~nTimeLogHandler()
{ 
}

const char*	 nTimeLogHandler::conv_file_name(const char* fname)
{
	struct	_timeb time_buffer;	

	// Get Current time
	_ftime( &time_buffer );
	buffer = fname;
#ifndef _DEBUG
	stl_string date_suffix(ctime( & ( time_buffer.time ) ));
	std::replace(date_suffix.begin(), date_suffix.end(),' ', '_');
	std::replace(date_suffix.begin(), date_suffix.end(),':', '_');
	std::replace(date_suffix.begin(), date_suffix.end(),'\n', '\0');
	buffer += '_';
	buffer += date_suffix;	
#endif	
	return buffer.c_str();
}

//------------------------------------------------------------------------------
/**
*/
const char*  
nTimeLogHandler::add_time(const char* msg)
{
	static int cr = 0;
	const char* res = 0;

	if (!cr)
	{
		cr++;
		char	time_str[9] = "No Time!";	
		struct	_timeb time_buffer;
		char	*time_line;	

		// Get Current time
		_ftime( &time_buffer );
		time_line = ctime( & ( time_buffer.time ) );
		if (time_line)
		{
			strncpy(time_str, time_line + 11, 8 );		
		}
		time_str[8] = '\0';
		buffer = time_str;
		buffer += "> ";
		buffer += msg;
		res =  buffer.c_str();
	} 
	else
		res = msg;

	if (strchr(msg, '\n'))
		cr = 0;

	return res;	
}
//------------------------------------------------------------------------------
/**
*/
void
nTimeLogHandler::Print(const char* msg, va_list argList)
{	
	nFileLogHandler::Print(this->add_time(msg), argList);
	fflush(this->logFile);
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeLogHandler::Message(const char* msg, va_list argList)
{
    nFileLogHandler::Message(this->add_time(msg), argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeLogHandler::Error(const char* msg, va_list argList)
{
    nFileLogHandler::Error(this->add_time(msg), argList);
}

