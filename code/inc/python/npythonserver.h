#ifndef N_PYTHONSERVER_H
#define N_PYTHONSERVER_H
//--------------------------------------------------------------------
/**
    @class nPythonServer
    @ingroup PythonScriptServices
    @brief wraps around Python interpreter

    Implements an nScriptServer that understands Python, extended
    by a few Nebula specific Python commands and the ability
    to route script cmds to Nebula C++ objects.
*/
//--------------------------------------------------------------------
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG 1
#else
#include <Python.h>
#endif
#include <errcode.h>

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_SCRIPTSERVER_H
#include "kernel/nscriptserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nPythonServer
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nFileServer2;
class nPythonServer : public nScriptServer 
{
public:
    
    static nKernelServer* kernelServer;
    nAutoRef<nFileServer2> refFileServer;
    
    static nPythonServer* Instance;
public:
    nPythonServer();
    virtual ~nPythonServer();
 
    virtual bool Run(const char *, const char*&);
    virtual bool Run(const char *, int mode, const char*&);
    virtual bool RunScript(const char *, const char*&);
    virtual bool RunCommand(nCmd *);
    virtual char* Prompt(char *, int);
 
    virtual nFile* BeginWrite(const char* filename, nRoot* obj);  
    virtual bool WriteComment(nFile *, const char *);
    virtual bool WriteBeginNewObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteBeginNewObjectCmd(nFile *, nRoot *, nRoot *, nCmd *);
    virtual bool WriteBeginSelObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteCmd(nFile *, nCmd *);
    virtual bool WriteEndObject(nFile *, nRoot *, nRoot *);
    virtual bool EndWrite(nFile *);

	/// get script command parameters begin symbols
	virtual const char* GetCmdParamBegin() const { return "("; }
	/// get script command parameters delimiter
	virtual const char* GetCmdParamDelim() const { return ","; }
	/// get script command parameters end symbols
	virtual const char* GetCmdParamEnd() const { return ")"; }
 
    virtual bool Trigger(void);

    bool is_standalone_python;
    PyObject *callback;   // Callback for Trigger() function

protected:
    virtual void write_select_statement(nFile *, nRoot *, nRoot *);

    bool print_error;
    PyObject *nmodule;
    PyObject *main_module;
};
//--------------------------------------------------------------------
#endif
