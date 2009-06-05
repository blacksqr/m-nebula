#define N_IMPLEMENTS nEclServer
//------------------------------------------------------------------------------
/**
	neclrun.cc
	based on original tclrun.cc 
	created 21-May-2009 by plushy
*/
#include <stdlib.h>
#include <stdio.h>

#include "ecl/neclserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/narg.h"
#include "util/nstl.h"


char* to_c_string(cl_object obj);

//------------------------------------------------------------------------------
/**
    -May-15-2009        plushy      created 
*/
char* nEclServer::Prompt(char* buf, int size)
{
    char buffer[1024];
    const char* res;
    buffer[0] = 0;
    this->Run("(psel)", res);	
    strcat(buffer, res);
    strcat(buffer, ">");
    n_strncpy2(buf, buffer, size);
    return buf;
}

//------------------------------------------------------------------------------
/**
    -May-15-2009        plushy      created 
*/
bool nEclServer::Run(const char* cmd_str, const char*& result)
{
	//cl_object cl_result = cl_eval(c_string_to_object(cmd_str));
    //cl_object cl_result = si_safe_eval(3, c_string_to_object(cmd_str), Cnil, this->eval_error_symbol); 
	cl_object cl_result = cl_safe_eval(c_string_to_object(cmd_str), Cnil, this->eval_error_symbol); 	
	
	if (this->is_error(cl_result, &this->result_str))
	{
		n_printf("*** Ecl error '%s' \n", this->result_str.c_str());
		if (this->GetFailOnError()) 
		{
            n_error("Aborting.");
        }		
		result = this->result_str.c_str();
        return false;
	}
	else
	{			
		this->result_str = to_stl_string(cl_result);
		result = this->result_str.c_str();

		return true;
	}	
}

//------------------------------------------------------------------------------
/**
    -May-15-2009        plushy      created 
*/
bool nEclServer::RunScriptFS(const char* fname, const char*& result)
{
	stl_string path;	
	this->refFileServer->ManglePath(fname, path);

	char cpath[N_MAXPATH];
	strcpy_s(cpath, N_MAXPATH, path.c_str());

	cl_object cl_result = cl_load(1, make_simple_base_string(cpath));	

	stl_string error_msg;
	if (this->is_error(cl_result, &error_msg))
	{
		n_printf("*** Ecl error '%s'\n", error_msg.c_str());
		if (this->GetFailOnError()) 
		{
			n_error("Aborting.");
		}
		return false;
	}
	else
	{
		this->result_str = to_stl_string(cl_result);
		result = this->result_str.c_str();
		return true;
	}	
}

//------------------------------------------------------------------------------
/**
    -May-15-2009        plushy      created 
*/
bool nEclServer::RunScript(const char* fname, const char*& result)
{  
	bool fres = true;
    result = 0;
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

				char* str = &script_buffer.front();

				while (str < &script_buffer.front() + size && fres)
				{
					fres = this->Run(str, result);					
					str += strlen(str);
				}				
			} 
			else
			{
				fres = false;
				file->Close();
			}
        } 
		else 
		{
            n_printf("file %s opening error \n", fname);			
			fres = false;
        }    

		delete file;
    } 
	else 
	{
		return this->RunScriptFS(fname, result);
	}
	return fres;
}

//------------------------------------------------------------------------------
/**
    -May-21-2009        plushy      created 
*/
bool nEclServer::Trigger(void)
{
	// while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT));
	return nScriptServer::Trigger();
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
