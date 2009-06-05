#ifndef N_ECLSERVER_H
#define N_ECLSERVER_H
//--------------------------------------------------------------------
/** 
    @class nEclServer
    @ingroup ScriptServices

    @brief wraps around Ecl interpreter

    Implements an nScriptServer that understands Common Lisp ECL, 
	extended by a few Nebula specific Ecl commands and the ability
    to route script cmds to Nebula C++ objects.
*/
//--------------------------------------------------------------------
#include <ecl/ecl.h>

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
#define N_DEFINES nEclServer
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nFileServer2;
class nEclServer : public nScriptServer 
{
public:
    nEclServer();
    virtual ~nEclServer();
    
    virtual bool Run(const char*, const char*&);
    virtual bool RunScript(const char*, const char*&);
	virtual bool RunScriptFS(const char*, const char*&);    
    virtual char* Prompt(char*, int);
    
    virtual nFile* BeginWrite(const char* filename, nRoot* obj);  
    virtual bool WriteComment(nFile *, const char *);
    virtual bool WriteBeginNewObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteBeginNewObjectCmd(nFile *, nRoot *, nRoot *, nCmd *);
    virtual bool WriteBeginSelObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteCmd(nFile *, nCmd *);
    virtual bool WriteEndObject(nFile *, nRoot *, nRoot *);
    virtual bool EndWrite(nFile *);

	/// get script command begin symbols
	virtual const char* GetCmdBegin() const {return "("; }
	/// get script command end symbols
	virtual const char* GetCmdEnd() const {return ")"; }
	/// get script command parameters begin symbols
	virtual const char* GetCmdParamBegin() const { return " "; }
	/// get script command parameters delimiter
	virtual const char* GetCmdParamDelim() const { return " "; }
	/// get script command parameters end symbols
	virtual const char* GetCmdParamEnd() const { return " "; }
    
    virtual bool Trigger(void);

public:
	static nKernelServer *kernelServer;        
    nAutoRef<nFileServer2> refFileServer;

public:		

protected:    
	typedef void (__stdcall* c_func4cl) ();	
	static	void reg_func(char* name, c_func4cl func, int narg);
	virtual void write_select_statement(nFile *, nRoot *, nRoot *);
    bool		print_error;    

protected:
	
	bool		is_error(cl_object obj, stl_string* error_msg);

	stl_string	result_str;
	cl_object	eval_handler;
	cl_object	read_error_symbol;
	cl_object	eval_error_symbol;
};

stl_string to_stl_string(cl_object obj);

#ifdef __cplusplus
extern "C" {
#endif
	//@# todo: check comments
	cl_object	__stdcall eclcmd_new(cl_object class_name, cl_object name);
	cl_object	__stdcall eclcmd_del(cl_object path);
	cl_object	__stdcall eclcmd_sel(cl_object path);;
	cl_object	__stdcall eclcmd_psel();
	cl_object	__stdcall eclcmd_exit();
	cl_object	__stdcall eclcmd_nprint(cl_object cl_str);
	cl_object	__stdcall eclcmd_dir();
	cl_object	__stdcall eclcmd_exists(cl_object path);
	cl_object	__stdcall eclcmd_ne(cl_narg narg, ...);
	cl_object	__stdcall to_cl_symbol(const char* s); 
	cl_object	__stdcall to_cl_string(const char* s); 

#ifdef __cplusplus
}
#endif


//--------------------------------------------------------------------
#endif
