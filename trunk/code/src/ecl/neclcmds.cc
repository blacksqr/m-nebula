#define N_IMPLEMENTS nEclServer

//------------------------------------------------------------------------------
/**  
	neclcmds.cc - implements nebula ecl commands	
	based on original ntclcmds.cc 
	(C) May-8-2009 + by plushy
*/
#include <stdlib.h>
#include <stdio.h>
#include "ecl/neclserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"
#include "mathlib/nmath.h"
//#define _RUNANYWAY

//------------------------------------------------------------------------------
/**  
	nEclServer::is_error
	basic error handling
	(C) May-8-2009 + by plushy
*/
bool nEclServer::is_error(cl_object obj, stl_string* error_msg)
{    
	if (error_msg)
	{
		if (obj == this->read_error_symbol)
		{
			*error_msg = "Error reading form";
		}
		else if (obj == this->eval_error_symbol)
		{
			*error_msg  = "Error evaluating form";
		}
	}	
	return obj == this->read_error_symbol || obj == this->eval_error_symbol;
}

//------------------------------------------------------------------------------
/**  
	to_stl_string
	Convert a lisp object into its string representation.
	(C) May-8-2009 + by plushy
	//@# check the consistence and robustness 
*/
stl_string to_stl_string(cl_object obj)
{
	if (obj == Cnil)
	{
		return stl_string("Nil");
	}
	else if (ecl_stringp(obj))
	{
		return stl_string(ecl_base_string_pointer_safe(obj)); 
	} 
	else
	{
		cl_object princed = cl_princ_to_string(1, obj);
		return stl_string((char*)princed->base_string.self);
	}	
}

//------------------------------------------------------------------------------
/**  
	to_c_string
	Convert a lisp object into its string representation.
	(C) May-8-2009 + by plushy
	//@# check the consistence and robustness 
*/
char* to_c_string(cl_object obj)
{	
	return ecl_base_string_pointer_safe(obj); 	
}

//------------------------------------------------------------------------------
/**  
	to_cl_symbol
	convert c-string to CL symbol
	(C) May-3-2009 + by plushy
*/
cl_object __stdcall to_cl_symbol(const char* s) 
{ 
	// for some reason we need to provide upper case symbols
	stl_string str(s); // so we copying string to allow strupr to proceed
	// and the using copy into lisp environment
	return cl_intern(1, make_base_string_copy(_strupr(&str[0])));
}

//------------------------------------------------------------------------------
/**  
	to_cl_symbol
	convert c-string to CL string
	(C) May-3-2009 + by plushy
*/
cl_object	__stdcall to_cl_string(const char* s)
{
	return ecl_cstring_to_base_string_or_nil(s);
}

//------------------------------------------------------------------------------
/**  
	eclcmd_new
    (C) May-3-2009 + by plushy

	@# may be better to return Cnil in any case of failure and just to type error 
	by n_printf
*/
cl_object __stdcall eclcmd_new(cl_object class_name, cl_object name)
{      
	if (ecl_stringp(class_name) && ecl_stringp(name))
	{
        nRoot* o = nEclServer::kernelServer->NewNoFail(to_c_string(class_name), to_c_string(name));

        if (o)
		{               
            return to_cl_string(o->GetFullName().c_str());
        } 
		else
		{
			//n_printf("Could not create object %s of type %s. \n", to_c_string(name), to_c_string(class_name));
			return cl_error(2, to_cl_string("Could not create object ~A of type ~A. ~%"), name, class_name);
		}
    }
	else
	{
		//n_printf("Invalid parameters to new.\n");
		return cl_error(2, to_cl_string("Invalid parameters to new ~A ~A. ~%"), name, class_name);		
	}
    //return Cnil; // return false in case of failure
}

//------------------------------------------------------------------------------
/**  
	eclcmd_del
    (C) May-3-2009 + by plushy
*/
cl_object __stdcall eclcmd_del(cl_object path)
{
	if (ecl_stringp(path))
	{
        nRoot* o = nEclServer::kernelServer->Lookup(ecl_base_string_pointer_safe(path));
        if (o) 
		{
            o->Release();            
        } 
		else
		{
			FEerror("The object ~A does not exists.~%",  1, to_stl_string(path).c_str());
		}
    }
	else
	{
		FEerror("Invalid parameter to delete ~A. ~%", 1, to_stl_string(path).c_str());
	}
    return Cnil; //@#
}

//------------------------------------------------------------------------------
/**  
	eclcmd_sel
	(C) May-4-2009 + by plushy
*/
cl_object __stdcall eclcmd_sel(cl_object cl_path)
{	
	if (ecl_stringp(cl_path))
	{
		stl_string path(to_stl_string(cl_path));
	
		nRoot* o = nEclServer::kernelServer->Lookup(path.c_str());
		if (o) 
		{
			nEclServer::kernelServer->SetCwd(o);
			return to_cl_string(o->GetFullName().c_str());			
		}
		else
		{
			n_printf("The object %s does not exists.\n", path.c_str());
		}
	} 
	else
	{
		n_printf("The sel argument has invalid type.\n");
	}	
	return Cnil; //@#
}

//------------------------------------------------------------------------------
/**  
	eclcmd_psel
	(C) May-4-2009 + by plushy
*/
cl_object __stdcall eclcmd_psel()
{
	nRoot* o = nEclServer::kernelServer->GetCwd();
	if (o) 
	{            
		return to_cl_string(o->GetFullName().c_str());        
	}    
	return Cnil; //@#
}


static cl_object __stdcall _dir_recursive(nRoot* o)
{	
	return (o ? ecl_cons(to_cl_string(o->GetName()), _dir_recursive(o->GetSucc())) : Cnil);	
}
//------------------------------------------------------------------------------
/**  
	eclcmd_dir
    (C) May-26-2009 + by plushy
*/
cl_object __stdcall eclcmd_dir()
{
    nRoot* cwd = nEclServer::kernelServer->GetCwd();
	return _dir_recursive(cwd ? cwd->GetHead() : 0);
}

#pragma warning( push )
#pragma warning( disable : 4100 )
//------------------------------------------------------------------------------
/**  
	eclcmd_exit
	(C) May-26-2009 + by plushy
*/
cl_object __stdcall eclcmd_exit()
{
	nEclServer::kernelServer->SetQuitRequest();
	return to_cl_symbol("T");
}
#pragma warning( pop )
//------------------------------------------------------------------------------
/**  
	eclcmd_nprint
	(C) May-26-2009 + by plushy
*/
cl_object __stdcall eclcmd_nprint(cl_object cl_str)
{
	if (ecl_stringp(cl_str))
	{
		nEclServer::kernelServer->Print(ecl_base_string_pointer_safe(cl_str));
		return to_cl_string(ecl_base_string_pointer_safe(cl_str));
	} 
	else
	{
		FEerror("Invalid parameter to nd_print ~A. ~%", 1, cl_str);
		return Cnil;
	}		
}

//------------------------------------------------------------------------------
/**  
	eclcmd_exists
	(C) May-26-2009 + by plushy
*/
cl_object __stdcall eclcmd_exists(cl_object path)
{
	if (ecl_stringp(path))
	{
		nRoot* o = nEclServer::kernelServer->Lookup(ecl_base_string_pointer_safe(path));
		if (o) 
		{
			return to_cl_symbol("T");
		} 
		else
		{
			n_printf("The object %s does not exists.\n",  to_stl_string(path).c_str());
		}
	}
	else
	{
		n_printf("Invalid parameter to exists %s .\n", to_stl_string(path).c_str());
	}
	return Cnil; //@#
}

bool str2bool(const char* str)
{
	return str && strlen(str) && 
		(
			!n_stricmp(str, "true") || 
			!n_stricmp(str, "t") ||
			!n_stricmp(str, "on") ||
			!n_stricmp(str, "1")
		) 
		&& 
		(
			n_stricmp(str, "false") && 
			n_stricmp(str, "f") &&
			n_stricmp(str, "off") &&
			n_stricmp(str, "null") &&
			n_stricmp(str, "nil") &&
			n_stricmp(str, "0")
		);
}
//------------------------------------------------------------------------------
/**  
	_getInArgs
    (C) May-26-2009 + by plushy
*/
static bool getInArgs(nCmd* cmd, int argc, cl_object* argv)
{
    long num_args = cmd->GetNumInArgs();

    if (num_args == argc) 
	{ 
        // read out arguments        
        nArg* arg;
        cmd->Rewind();
        for (int i = 0; i < num_args; i++) 
		{
            bool arg_ok = false;
            arg = cmd->In();
			cl_object obj = argv[i];
			//@# if (type_of(obj) == t_list)

            switch(arg->GetType()) 
			{
                case nArg::ARGTYPE_INT:
					if (type_of(obj) == t_fixnum)
					{
                        arg->SetI(fix(obj));                                
                        arg_ok = true;
                    }
                break;
                case nArg::ARGTYPE_FLOAT:                
					switch(type_of(obj)) 
					{
					case t_singlefloat:
						arg->SetF(ecl_to_float(obj));
                        arg_ok = true;
						break;
					case t_doublefloat:
						arg->SetF((float)ecl_to_double(obj));
                        arg_ok = true;
						break;
					case t_fixnum:
						arg->SetF((float)fix(obj));                                
                        arg_ok = true;
						break;
					}
                break;				
                case nArg::ARGTYPE_STRING:
                {
					if (ecl_stringp(obj))
					{
						stl_string s(to_stl_string(obj));
						const char* str = s.c_str();
						if (_stricmp("Nil",str)==0) 
						{
							str = 0;
						}
						arg->SetS(str);
						arg_ok = true;                        
                    }                    
                }
                break;    

				case nArg::ARGTYPE_USTRING:
				{
					n_error("Implement me.");
				}
				break;    

                case nArg::ARGTYPE_CODE:
                {
					if (ecl_stringp(obj))
					{
						stl_string s(to_stl_string(obj));
						const char* str = s.c_str();
						if (!strcmp("", str)) 
						{
							str = 0;
						}
						arg->SetC(str);
						arg_ok = true;                        
                    }                    
                }
                break;    

                case nArg::ARGTYPE_BOOL:
                {
					switch (type_of(obj))
					{
					case t_fixnum:
						arg->SetB(fix(obj) != 0);
                        arg_ok = true;
						break;
					case t_singlefloat:
						arg->SetB(n_neqz(ecl_to_float(obj)));
                        arg_ok = true;
						break;
					case t_doublefloat:
						arg->SetB(n_neqz(ecl_to_float(obj)));
                        arg_ok = true;
						break;
					case t_base_string:
						{
							stl_string s(to_stl_string(obj));							
							arg->SetB(str2bool(s.c_str()));							
							arg_ok = true;
						}						
						break;
					default:
						n_printf("Invalid argument type instead of boolean.");
						break;
					}                    
                }
                break;

                case nArg::ARGTYPE_OBJECT:
                    {
						stl_string o_name(to_stl_string(obj));
                        nRoot* o = 0;                        
                        if (!n_stricmp("null", o_name.c_str()) && n_stricmp("nil", o_name.c_str()))
						{
                            o = 0;
                            arg_ok = true;
                        } 
						else 
						{
                            o = nEclServer::kernelServer->Lookup(o_name.c_str());
                            if (o) arg_ok = true;
                            else   n_printf("could not lookup '%s' as object!\n" ,o_name.c_str());
                        }
                        arg->SetO(o);
                    }
                    break;
 
                case nArg::ARGTYPE_VOID:
                    break;

                case nArg::ARGTYPE_LIST:
                    n_warn("List values aren't acceptable in arguments.");
                    arg_ok = false;
                    break;
            }
            if (!arg_ok) return false;
        }
        return true;
    }
    return false;
}

static cl_object __stdcall putOutListArg(nArg* listArg);

//------------------------------------------------------------------------------
/**
	NArg2EclObj  - convert nArg to cl_object
	-May-15-2009        plushy      created 
*/
static cl_object __stdcall NArg2EclObj(nArg* arg) 
{
	cl_object res = Cnil;
	switch (arg->GetType()) 
	{
	case nArg::ARGTYPE_INT:				
		res = ecl_make_integer(arg->GetI());
		break;
	case nArg::ARGTYPE_FLOAT:
		res = ecl_make_singlefloat(arg->GetF());
		break;
	case nArg::ARGTYPE_STRING:
		res = to_cl_string(arg->GetS());
		break;
	case nArg::ARGTYPE_USTRING:
		n_error("UNICODE: implement me.");
		break;
	case nArg::ARGTYPE_CODE:
		res = to_cl_string(arg->GetC());
		break;
	case nArg::ARGTYPE_BOOL:
		res = arg->GetB() ? to_cl_symbol("T") : Cnil;
		break;
	case nArg::ARGTYPE_OBJECT: // represent engine objects by full path
		{   
			nRoot* o = (nRoot*) arg->GetO();
			if (o)
			{
				res = to_cl_string(o->GetFullName().c_str());
			}					
		}
		break;
	case nArg::ARGTYPE_LIST:
		res =  putOutListArg(arg);
		break;
	case nArg::ARGTYPE_VOID:
		break;
	}
	return res;
}


//--------------------------------------------------------------------
/**
	recursive answer list builder using cons - @# improve it by iteration
*/
static cl_object __stdcall _out_list_args_recursive(nArg* arg, int i, int num)
{
	return (i < num ? ecl_cons(NArg2EclObj(arg), _out_list_args_recursive(arg+1, i+1, num)) : Cnil);
}

//--------------------------------------------------------------------
/**
    utility function for nArg of type ARGTYPE_LIST
*/
static cl_object __stdcall putOutListArg(nArg* listArg)
{
    n_assert(nArg::ARGTYPE_LIST == listArg->GetType());
    nArg* args = 0;
    int num_args = listArg->GetL(args);
	return _out_list_args_recursive(args, 0, num_args);
}


//--------------------------------------------------------------------
/**
	another recursive answer list builder @# improve it by iteration
*/
static cl_object __stdcall _out_args_recursive(nCmd* cmd, int i, int num)
{
	return (i < num ? ecl_cons(NArg2EclObj(cmd->Out()), _out_args_recursive(cmd, i+1, num)) : Cnil);
}
//------------------------------------------------------------------------------
/**  
	putOutArgs     
    (C) May-28-2009 + by plushy
*/
static cl_object __stdcall putOutArgs(nCmd* cmd)
{    
    int num_args = cmd->GetNumOutArgs();
    cmd->Rewind();    
	return _out_args_recursive(cmd, 0, num_args);
}    

//------------------------------------------------------------------------------
/**  
	eclcmd_ne
    (C) May-26-2009 + by plushy
*/
cl_object __stdcall eclcmd_ne(cl_narg narg, ...)
{	
	cl_object result = Cnil;
	cl_va_list params;
	cl_va_start(params, narg, narg, 0);
	std::vector<cl_object> args;	
	
	for (int i = 0; i < narg; i++)
	{
		cl_object arg = cl_va_arg(params);
		if (type_of(arg) != t_list)
		{
			args.push_back(arg);
		}
		else
		{
			cl_object tail = arg;
			while ((arg = cl_car(tail)) != Cnil)
			{
				args.push_back(arg);
				tail = cl_cdr(tail);
			}
		}
	}

	narg = args.size();

	stl_string cmd_str = to_stl_string(args[0]);
    
	char* obj_name = 0;
	char* cmd_name = 0;
	char* dot = 0;
	char cmd[N_MAXPATH];
	nRoot* o = 0;
	bool has_dot = false;

	// extract object name and cmd name
	n_strncpy2(cmd, cmd_str.c_str(), sizeof(cmd));
	dot = strchr(cmd,'.');

	// special case handle path components
	while (dot && ((dot[1] == '.')||(dot[1] == '/'))) dot=strchr(++dot,'.');

	if (dot) 
	{
		has_dot = true;
		*dot = 0;
		obj_name = cmd;
		if (obj_name == dot) 
			obj_name = 0;
		cmd_name = dot+1;
	} 
	else 
	{
		obj_name = 0;
		cmd_name = cmd;
	}

	if (*cmd_name == 0) 
		cmd_name = 0;

	// find object to invoke command on
	if (obj_name) 
		o = nEclServer::kernelServer->Lookup(obj_name);
	else          
		o = nEclServer::kernelServer->GetCwd();

	if (o) 
	{
		if (cmd_name) 
		{
			// invoke command
			nClass* cl = o->GetClass();
			nCmdProto* cmd_proto = (nCmdProto*)cl->FindCmdByName(cmd_name);
			if (cmd_proto) 
			{
				nCmd* cmd = cmd_proto->NewCmd();
				n_assert(cmd);

				// retrieve input args (skip the 'unknown' and cmd statement)            
				if (getInArgs(cmd, narg - 1, narg > 1 ? &args[1] : 0))
				{
					// let object handle the command
					if (o->Dispatch(cmd)) 
					{						
						result = putOutArgs(cmd);
					} 
					else 
					{
						//@# tcl_objcmderror(interp, tcl, "Dispatch error, object '%s', command '%s'",o,cmd_name);
						n_printf("Dispatch error, object '%s', command '%s'", o->GetName(), cmd_name);
					}
					cmd_proto->RelCmd(cmd);

				}
				else //!(_getInArgs(cmd, narg-1, narg > 1 ? &args[1] : 0))
				{
					n_printf("Broken input args, object '%s', command '%s'", o->GetName(), cmd_name);
					cmd_proto->RelCmd(cmd);					
				}				
			} 
			else 
			{
				// exception, the object doesn't know about the command!
				// if there was a dot in the command name, we know that
				// it was supposed to be a Nebula command statement,
				// so we will just barf.
				if (has_dot) 
				{
					// this is not very critical...
					//@# tcl_objcmderror(interp, tcl, "Object '%s' doesn't accept command '%s'",o,cmd_name);
					n_printf("Object '%s' doesn't accept command '%s'", o->GetName(), cmd_name);
					result = to_cl_symbol("T");
				} 
				else 
				{
					// otherwise, let's see if the original unknown command
					// knows what to do...
					//@# retval = tcl_pipe_command(interp,"tcl_unknown", objc, objv);
				}
			}
		}
		else // !cmd_name
		{
			n_printf("Invalid command.\n");			
		}
	}
	else // !o
	{
		//@# tcl_unknownObject(interp, tcl->print_error, obj_name);
#if defined(_RUNANYWAY) || defined(_DEBUG)
		result = to_cl_symbol("T");
#endif		
	}	
	return result;		
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
