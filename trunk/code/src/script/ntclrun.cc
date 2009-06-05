#define N_IMPLEMENTS nTclServer
//--------------------------------------------------------------------
//  ntclrun.cc -- Kommandos auswerten und weiterleiten
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include "kernel/nfileserver2.h"
#include "script/ntclserver.h"
#include "kernel/narg.h"
#include "util/nstl.h"
//--------------------------------------------------------------------
/**
    - 23-Feb-99   floh    created
*/
//--------------------------------------------------------------------
char *nTclServer::Prompt(char *buf, int size)
{
    char buffer[1024];
    const char* res;
    buffer[0] = 0;
    this->Run("psel", res);
    strcat(buffer, res);
    strcat(buffer, ">");
    n_strncpy2(buf, buffer, size);
    return buf;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 03-Feb-00   floh    + now uses Tcl_EvalEx()
    - 14-Oct-01   floh    + checks error code
*/
//--------------------------------------------------------------------
bool nTclServer::Run(const char *cmd_str, const char*& result)
{
    result = 0;
    int errCode = Tcl_EvalEx(this->interp, (char *) cmd_str, -1, TCL_EVAL_DIRECT);
    Tcl_Obj *res = Tcl_GetObjResult(interp);
    result = Tcl_GetString(res);
    if (errCode == TCL_ERROR) 
	{
        n_printf("*** Tcl error '%s'\n", Tcl_GetString(res));
        if (this->GetFailOnError()) {
            n_error("Aborting.");
        }
        return false;
    }
    return true;
}

Tcl_Obj * NArg2TclObj(nArg *a) 
{
    switch (a->GetType()) {
        case nArg::ARGTYPE_VOID:
            return Tcl_NewStringObj("(null)", 6);
        case nArg::ARGTYPE_INT:
            return Tcl_NewIntObj(a->GetI());
        case nArg::ARGTYPE_FLOAT:
            return Tcl_NewDoubleObj(a->GetF());
        case nArg::ARGTYPE_STRING:
            return Tcl_NewStringObj(a->GetS(), -1);
		case nArg::ARGTYPE_USTRING:
			return Tcl_NewUnicodeObj((const Tcl_UniChar*)a->GetU(), -1);
        case nArg::ARGTYPE_BOOL:
            return Tcl_NewBooleanObj(a->GetB());
        case nArg::ARGTYPE_OBJECT:
            return (Tcl_Obj *)a->GetO();
        case nArg::ARGTYPE_CODE:
            return Tcl_NewStringObj(a->GetC(), -1);
    }
    return NULL;
}

//--------------------------------------------------------------------
//  RunScript()
//  10-Oct-98   floh    created
//  25-Feb-99   floh    Error-Message
//  30-Mar-99   floh    + schickt den Filename zuerst durch
//                        nFileServer::ManglePath(), um evtl
//                        Assigns aufzuloesen
//--------------------------------------------------------------------
bool nTclServer::RunScriptFS(const char *fname, const char*& result)
{
	stl_string path;	
	result = 0;	
	this->print_error = true;
	int errCode = Tcl_EvalFile(this->interp, this->refFileServer->ManglePath(fname, path));
	this->print_error = false;
	Tcl_Obj *res = Tcl_GetObjResult(interp);
	result = Tcl_GetString(res);
	if (errCode == TCL_ERROR) 
	{
		n_printf("*** Tcl error '%s' in file %s line %d.\n", result, fname, this->interp->errorLine);
		if (this->GetFailOnError())
		{
			n_error("Aborting.");
		}
		return false;
	}             
	return true;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 25-Feb-99   floh    Error-Message
    - 30-Mar-99   floh    + schickt den Filename zuerst durch
                            nFileServer::ManglePath(), um evtl
                            Assigns aufzuloesen
*/
//--------------------------------------------------------------------
bool nTclServer::RunScript(const char *fname, const char*& result)
{  
    result = 0;
	bool fres = false;
    if (this->refFileServer.isvalid()) 
	{    
        nFile* file = this->refFileServer->NewFileObject();
        n_assert(file);
    
        if (file->Open(fname, "r")) 
		{
            char_buffer script_buffer;

            file->Seek(0, nFile::END);
            long size = file->Tell();
            file->Seek(0, nFile::START);
			if (size > 0)
			{
				script_buffer.resize(size);

				file->Read(script_buffer.get_buffer(), size);
				file->Close();            
				

				this->print_error = true;
				int errCode = Tcl_EvalEx(this->interp, script_buffer.get_buffer(), size, TCL_EVAL_DIRECT);
				this->print_error = false;
				Tcl_Obj *res = Tcl_GetObjResult(interp);
				result = Tcl_GetString(res);

				if (errCode == TCL_ERROR) 
				{               
					n_printf("*** Tcl error '%s' in file %s line %d.\n", result, fname, this->interp->errorLine);
					if (this->GetFailOnError())
					{
						n_error("Aborting.");
					}            					
				}           
				else
				{
					fres = true;
				}
			}
			else
			{
				file->Close();
			}			
        } 
		else 
		{
            n_printf("file %s opening error \n", fname);			
        }    
		delete file;
    } 
	else 
	{
		return this->RunScriptFS(fname, result);
	}
	return fres;
}

//--------------------------------------------------------------------
/**
    @brief Guarantee that the TCL events are processed.

    - 21-Dec-98   floh    created
    - 23-Feb-99   floh    jetzt mit Return-Val, wenn Quit requested.
*/
//--------------------------------------------------------------------
bool nTclServer::Trigger(void)
{
    while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT));
    return nScriptServer::Trigger();
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

