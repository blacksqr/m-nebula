#define N_IMPLEMENTS nEclServer
//------------------------------------------------------------------------------
/**
	neclserver.cc
    -Jun-1-2009        plushy      created 
*/
#include <stdlib.h>
#include <stdio.h>

#include "ecl/neclserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/nfile.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nEclServer, "nscriptserver");

#define N_ECLSERVER_SCRIPTS  "/sys/share/ecl_scriptlets"


#pragma warning( push )
#pragma warning( disable : 4100 )

void nEclServer::reg_func(char* name, void(__stdcall *func)() , int narg)
{
	if (narg >= 0)
		cl_def_c_function(to_cl_symbol(name), (void*)func, narg);
	else
		cl_def_c_function_va(to_cl_symbol(name), (void*)func);
}

//------------------------------------------------------------------------------
/**  
	nEclServer     
    (C) May-1-2009 + by plushy
*/
nEclServer::nEclServer() :
    refFileServer(kernelServer, this)
{
    this->refFileServer     = "/sys/servers/file2";
    this->print_error       = false;    	

	// ECL Initialization
	
	ecl_set_option(ECL_OPT_TRAP_SIGFPE, 0);
	char* argv[1];
	argv[0] = (char*)kernelServer->GetArgv(0);
	cl_boot(1, argv);	

	this->read_error_symbol = cl_gensym(0);
    this->eval_error_symbol = cl_gensym(0);

	cl_safe_eval(c_string_to_object("(setq *terminal-io* "
		"  (make-two-way-stream "
		"    (make-string-input-stream \"\") "
		"    (make-string-output-stream)))"), Cnil, Cnil);
	cl_safe_eval(c_string_to_object("(setq *error-output* "
		"  (two-way-stream-output-stream *terminal-io*))"), Cnil, Cnil);

	reg_func("nd",		(c_func4cl)eclcmd_ne,	   -1);	// - 
	reg_func("nd_print",(c_func4cl)eclcmd_nprint,	1); // +/-
	reg_func("new",		(c_func4cl)eclcmd_new,      2); // +
	reg_func("del",		(c_func4cl)eclcmd_del,		1); // +
	reg_func("sel",		(c_func4cl)eclcmd_sel,      1); // +
 	reg_func("psel",	(c_func4cl)eclcmd_psel,     0); // +	
	reg_func("exit",	(c_func4cl)eclcmd_exit,     0); // +
	reg_func("dir",		(c_func4cl)eclcmd_dir,      0); // +
	reg_func("exists",	(c_func4cl)eclcmd_exists,   1); // +

//	si_select_package(make_simple_base_string("CL-USER"));
	
	kernelServer->New("nroot",N_ECLSERVER_SCRIPTS); 

	this->extensions.push_back(stl_string("lsp"));
	this->extensions.push_back(stl_string("lisp"));
}

//------------------------------------------------------------------------------
/**  
	~nEclServer     
    (C) 2009/05/01 + by plushy
*/
nEclServer::~nEclServer()
{
	cl_shutdown();
}


//------------------------------------------------------------------------------
/**
    Begin writing a persistent object.
*/
nFile* 
nEclServer::BeginWrite(const char* filename, nRoot* obj)
{
    n_assert(filename);
    n_assert(obj);

    this->indent_level = 0;

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "w"))
    {
        char buf[N_MAXPATH];
        sprintf(buf, "; $parser:%s$ $class:%s$\n", this->GetClass()->GetName(), obj->GetClass()->GetName());

        file->PutS("; ---\n");
        file->PutS(buf);
        file->PutS("; ---\n");
        return file;
    }
    else
    {
        n_printf("nEclServer::WriteBegin(): failed to open file '%s' for writing!\n", filename);
        delete file;
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Finish writing a persistent object.
*/
bool 
nEclServer::EndWrite(nFile* file)
{
    n_assert(file);
 
    file->PutS("; ---\n");
    file->PutS("; Eof\n");
    
    file->Close();
    delete file;
    return (this->indent_level == 0);
}

//--------------------------------------------------------------------
//  _indent()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
static void _indent(long i, stl_string& buf)
{    
    for (long j = 0; j < i; j++) buf += "\t";
}

//------------------------------------------------------------------------------
/**
	WriteComment()
    04-Nov-98   floh    created
*/
bool nEclServer::WriteComment(nFile* file, const char *str)
{
    n_assert(file);
    n_assert(str);
    file->PutS("; ");
    file->PutS(str);
    file->PutS("\n");
    return true;
}

//------------------------------------------------------------------------------
/**
	write_select_statement()
	Write the statement to select an object after its creation
	statement.
	05-Oct-00   floh    created
*/
void nEclServer::write_select_statement(nFile* file, nRoot* o, nRoot* owner)
{
    switch (this->GetSelectMethod()) 
	{
        case SELCOMMAND:
            // get relative path from owner to o and write select statement
			{
				stl_string relpath;
				_indent(++this->indent_level, this->indent_buf);            
				file->PutS(this->indent_buf.c_str());
				file->PutS("(sel ");
				file->PutS(owner->GetRelPath(o, relpath));
				file->PutS(")\n");
			}
            break;
        case NOSELCOMMAND:
            break;
    }
}

//------------------------------------------------------------------------------
/**
	WriteBeginNewObject()
	Write start of persistent object with default constructor.
	06-Mar-00   floh    created
	05-Sep-00   floh    + support for optional enter/leave syntax
*/
bool nEclServer::WriteBeginNewObject(nFile* file, nRoot* o, nRoot* owner)
{
    n_assert(file);
    n_assert(o);
    const char *o_name = o->GetName();

    // write generic 'new' statement
    const char *o_class = o->GetClass()->GetName();
    _indent(this->indent_level, this->indent_buf);

    file->PutS(this->indent_buf.c_str());
    file->PutS("(new ");
    file->PutS(o_class);
    file->PutS(" ");
    file->PutS(o_name);
    file->PutS(" )\n");

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteBeginNewObjectCmd()
//  Write start of persistent object with custom constructor
//  defined by command.
//  06-Mar-00   floh    created
//--------------------------------------------------------------------
bool nEclServer::WriteBeginNewObjectCmd(nFile* file, nRoot *o, nRoot *owner, nCmd *cmd)
{
    n_assert(file);
    n_assert(o);
    n_assert(cmd);

    // write constructor statement
    this->WriteCmd(file, cmd);

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteBeginSelObject()
//  Write start of persisting object without constructor, only
//  write the select statement.
//  06-Mar-00   floh    created
//--------------------------------------------------------------------
bool nEclServer::WriteBeginSelObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteEndObject()
//  04-Nov-98   floh    created
//  06-Mar-00   floh    + write relative path from o to owner
//                        into select statement, instead of
//                        hardwiring to 'sel ..'
//  05-Oct-00   floh    + write unselect statement
//--------------------------------------------------------------------
bool nEclServer::WriteEndObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // get relative path from owner to o and write select statement
    stl_string relpath;
    _indent(--this->indent_level, this->indent_buf);
    file->PutS(this->indent_buf.c_str());
    file->PutS("(sel ");
    file->PutS(owner->GetRelPath(o, relpath));
    file->PutS(")\n");

    return true;
}

//--------------------------------------------------------------------
//  WriteCmd()
//  04-Nov-98   floh    created
//  12-Jan-99   floh    verwendet jetzt den "neuen" Syntax, nicht
//                      mehr "do".
//  08-Aug-99   floh    nCmd-Objekte kennen nicht mehr unmittelbar
//                      ihren Namen, haben aber einen Pointer auf
//                      ihren nCmdProto eingebettet
//  12-Oct-99   floh    - String-Args werden mit einem umschliessenden
//                        " " geschrieben
//                      - Support fuer ARGTYPE_OBJECT
//  09-Nov-99   floh    + Support fuer "null" Objects
//--------------------------------------------------------------------
bool nEclServer::WriteCmd(nFile* file, nCmd* cmd)
{
    n_assert(file);
    n_assert(cmd);
    const char *name = cmd->GetProto()->GetName();
    n_assert(name);
    nArg *arg;

    // write the command name
    file->PutS(this->indent_buf.c_str());
	file->PutS("(");
    file->PutS(name);

    // write command arguments
    cmd->Rewind();
    int num_args = cmd->GetNumInArgs();

    const char* strPtr = 0;
	const wchar* wstrPtr = 0;
    ushort strLen;
    ushort bufLen;
    
    for (int i = 0; i < num_args; i++) 
    {
        char buf[N_MAXPATH*2];
		
        arg = cmd->In();

        switch(arg->GetType()) 
		{
            case nArg::ARGTYPE_INT:
                sprintf(buf, " %d", arg->GetI());
                break;

            case nArg::ARGTYPE_FLOAT:
                sprintf(buf, " %.6f", arg->GetF());
                break;

            case nArg::ARGTYPE_STRING:
                strPtr = arg->GetS();
                strLen = (ushort)strlen(strPtr);
                bufLen = sizeof(buf)-1;
            
                file->PutS(" \"");
                if (strLen > bufLen-1) 
                {
                    buf[bufLen] = 0; // Null terminator
                    for (int j = 0; j < strLen - 2; j+=bufLen)
                    {
                        memcpy((void*)&buf[0], strPtr, bufLen);
                        file->PutS(buf);
                        strPtr += bufLen;     
                    }
					strPtr += bufLen;
                }
                sprintf(buf, "%s\"", strPtr);
                break;

			case nArg::ARGTYPE_USTRING:
				{				
					wstrPtr = arg->GetU();
					strLen = (ushort)wcslen(wstrPtr);
					bufLen = sizeof(buf)/sizeof(wchar) - 1;
					wchar* ubuf = (wchar*)buf;

					file->PutU(L" \"");
					if (strLen > bufLen - 1) 
					{
						ubuf[bufLen] = 0; // Null terminator
						for (int j = 0; j < strLen-2; j += bufLen)
						{
							memcpy((void*)&buf[0], wstrPtr, bufLen*sizeof(wchar));
							file->PutU(ubuf);
							wstrPtr += bufLen;     
						}
						wstrPtr += bufLen;
					}
					swprintf_s(ubuf, N_MAXPATH, L"%s\"", wstrPtr);
				}
				break;

            case nArg::ARGTYPE_CODE:
                sprintf(buf, " { %s }", arg->GetC());
                break;

            case nArg::ARGTYPE_BOOL:
                sprintf(buf," %s",(arg->GetB() ? "T" : "Nil"));
                break;

            case nArg::ARGTYPE_OBJECT:
                {
                    nRoot *o = (nRoot *) arg->GetO();
                    if (o) 
					{
                        char buf[N_MAXPATH];
                        sprintf(buf, " %s", o->GetFullName().c_str());
                    } 
                    else 
                    {
                        sprintf(buf, "Nil");
                    }
                }
                break;

            default:
                sprintf(buf, ";???");
                break;
        }
        file->PutS(buf);
    }
    return file->PutS(")\n");
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
