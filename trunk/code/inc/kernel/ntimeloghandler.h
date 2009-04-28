#ifndef N_TIMELOGHANDLER_H
#define N_TIMELOGHANDLER_H

//------------------------------------------------------------------------------
/**
    A fus timestamped log handler class:
    
    - maintains a log file in the home: directory where ALL
      output is recorded

    (C) 2007 Insightec, Inc
*/

#include "kernel/nfileloghandler.h"
#include "util/nstl.h"

#undef N_DEFINES
#define N_DEFINES nTimeLogHandler
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nTimeLogHandler : public nFileLogHandler
{
public:
    /// constructor
    nTimeLogHandler(const char* logName);
    /// destructor
    virtual ~nTimeLogHandler();
    /// print a message to the log dump
    virtual void Print(const char* msg, va_list argList);
    /// show an important message (may block the program until the user acks)
    virtual void Message(const char* msg, va_list argList);
    /// show an error message (may block the program until the user acks)
    virtual void Error(const char* msg, va_list argList);
	
protected:
	static const char*	 conv_file_name(const char* fname);
	static const char*  add_time(const char* msg);	
	static stl_string   buffer;
};

//------------------------------------------------------------------------------
#endif

