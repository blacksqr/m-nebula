#define N_IMPLEMENTS nApplication
//------------------------------------------------------------------------------
//  (C) 2009	plushy
//------------------------------------------------------------------------------
#include "kernel/ntimeserver.h"
#include "kernel/nremoteserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/nwin32loghandler.h"
#include "kernel/nfileloghandler.h"
#include "kernel/nscriptserver.h"
#include "kernel/nfileserver2.h"
#include "util/npathstring.h"

#include "app/napplication.h"

nNebulaScriptClass(nApplication, "nroot");

//------------------------------------------------------------------------------
/**
*/
nApplication::nApplication() : 
#ifdef LOG
#ifdef __WIN32__
	winlog(0), 
#endif
	filelog(0),
#endif		
	in_begin(false),
	script_class("ntclserver"),
	noinit(false),
	noremote(false),
	refScript(kernelServer, this)
{	
	nPathString app_path(this->kernelServer->GetArgv(0));
	nPathString app_file(app_path.ExtractFileName());
	app_file.StripExtension();
	this->port_name = this->app_name = app_file.c_str();	

	this->ext_script_map["tcl"] = "ntclserver";
	this->ext_script_map["ntcl"] = "ntclserver";
	this->ext_script_map["npy"] = "npythonserver";
	this->ext_script_map["py"] = "npythonserver";
	this->ext_script_map["lisp"] = "neclserver";
	this->ext_script_map["lsp"] = "neclserver";
	this->ext_script_map["nlsp"] = "neclserver";
}

//------------------------------------------------------------------------------
/**
*/
void nApplication::Initialize(void)
{
	if (this->process_command_line())
	{			
#ifdef LOG
#ifdef __WIN32__
		this->winlog = new nWin32LogHandler(this->app_name.c_str());
		if (this->winlog)
			this->kernelServer->AddLogHandler(this->winlog);
#endif
		this->filelog = new nFileLogHandler(this->app_name.c_str());

		if (this->filelog)
			this->kernelServer->AddLogHandler(this->filelog);
#endif	
		if (this->initialize())
		{
			this->run_init_script();
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
bool nApplication::initialize()
{n_assert("nApplication is in begin state" && !this->in_begin);
	return (this->AddServer(this->kernelServer, this->script_class.c_str(),	"/sys/servers/script", &this->refScript) != 0);
}

//------------------------------------------------------------------------------
/**
*/
bool nApplication::run_init_script()
{
	if (this->refScript.isvalid())
	{		
		nFileServer2* fs = this->kernelServer->GetFileServer2();	

		stl_string init_dir;		
		fs->MakeAbsoluteMangled("home:script", init_dir);
		init_dir += '/';
		init_dir += this->app_name;			
		init_dir += '/';		
		fs->SetAssign("appscript", init_dir.c_str());
		fs->SetAssign("lib", "home:script/lib/");

		if (!this->noinit && fs)			
		{	
			bool found = false;
			stl_string init_file("appscript:init.");
			for (strings_v::const_iterator i = this->refScript->GetExtensions().begin(); i != this->refScript->GetExtensions().end(); i++)
			{
				stl_string check_file(init_file + *i);
				if (fs->FileExists(check_file.c_str()))
				{
					init_file = check_file;
					found = true;
				}
			}
			if (found)
			{
				fs->PushCwd();
				fs->ChangeDir(init_dir.c_str());
				n_printf("running init script '%s'\n", init_file.c_str());
				const char* res;
				this->refScript->RunScriptFS(init_file.c_str(), res);
				fs->PopCwd();
			}			
		} 
		else 
		{
			this->noinit = true;
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
bool nApplication::run_startup_script()
{
	if (this->refScript.isvalid())
	{			
		if (!this->startup_script.empty()) 
		{				
			const char* res = "ok";
			n_printf("running startup script '%s'\n", this->startup_script.c_str());

			nPathString startup_path(this->startup_script.c_str());

			if (this->refScript->IsExtSupported(startup_path.GetExtension()))
				this->refScript->RunScriptFS(this->startup_script.c_str(), res);
			else
				this->Parse(this->startup_script.c_str());
			if (res) 
				n_printf("%s\n", res);
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
	sample for main loop
*/
static int def_mainloop(nKernelServer* ks, nApplication* app)
{	
	nScriptServer* script = (nScriptServer*)ks->Lookup("/sys/servers/script");		
	while (script->Trigger()) 
	{
		ks->Trigger();
		ks->ts->Trigger();
		//double t = ks->ts->GetFrameTime();
		if (false == app->noremote)
			ks->GetRemoteServer()->Trigger();
	}
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool nApplication::begin()
{n_assert("nApplication is in begin state" && !this->in_begin);
	this->in_begin = true;
	this->kernelServer->ts->EnableFrameTime();
	if (false == this->noremote)
		return this->kernelServer->GetRemoteServer()->Open(this->port_name.c_str());
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void nApplication::end()
{n_assert("nApplication is not in begin state" && this->in_begin);
	if (false == this->noremote)
		this->kernelServer->GetRemoteServer()->Close();
	this->kernelServer->ts->DisableFrameTime();
	this->in_begin = false;
}

//------------------------------------------------------------------------------
/**
*/
int nApplication::Run(int (*mainloop)(nKernelServer* ks, nApplication* app) = def_mainloop)
{	
	int res = 0;
	
	if (this->begin()) 
	{
		this->run_startup_script();
		res =  mainloop(this->kernelServer, this);		
		this->end();
	}
	
	return res;
}

//------------------------------------------------------------------------------
/**
*/
bool nApplication::process_command_line()
{
	nKernelServer* _ks = this->kernelServer;

	stl_string arg_cmd;	
	
	bool argerr = false;	

	int argc = _ks->GetArgc();

	bool script_specified = false;

	for (long i = 1; i < argc && !argerr; i++) 
	{
		const char* arg = _ks->GetArgv(i);
		if (strcmp(arg, "-port") == 0) 
		{
			if (++i < argc) this->port_name = _ks->GetArgv(i);
			else argerr = true;
		} 
		else if (strcmp(arg, "-startup") == 0) 
		{
			if (++i < argc) this->startup_script = _ks->GetArgv(i);
			else argerr = true;
		}
		else if (strcmp(arg,"-noinit") == 0) this->noinit = true;
		else if (strcmp(arg,"-noremote") == 0) this->noremote = true;
		else if (strcmp(arg,"-script") == 0) 
		{
			if (++i < argc) 
			{
				this->script_class = _ks->GetArgv(i);
				script_specified = true;
			}
			else 
				argerr = true;
		}
		else if (strcmp(arg, "-args")==0) 
		{
			// args are stored for use by scripts etc.
			for (++i; i < argc; i++) 
			{
				arg_cmd += _ks->GetArgv(i);
				arg_cmd += " ";				
			}
		} 
		else 
		{			
			if (this->kernelServer->GetFileServer2()->FileExists(_ks->GetArgv(i))) 
			{
				this->startup_script = _ks->GetArgv(i);
				if (!script_specified)
				{
					nPathString startup_path(this->startup_script.c_str());
					str2str_map::iterator i = this->ext_script_map.find(stl_string(startup_path.GetExtension()));
					if (i != this->ext_script_map.end())
						this->script_class = i->second;
				}
			} 
			else
			{
				n_printf("nApp: unknown arg: %s\n", _ks->GetArgv(i));
			}
		}			
	}	

	if (!arg_cmd.empty()) 
	{
		nKernelServer* _ks = this->kernelServer;
		nEnv* args = (nEnv*) _ks->New("nenv","/sys/share/args");
		args->SetS(arg_cmd.c_str());
	}

	if (argerr) 
	{
		n_printf("nApplication::process_command_line->Command line arguments error, exiting.\n"); 
		return false;	
	}

	return true;
}

//------------------------------------------------------------------------------
/**
*/
nApplication::~nApplication()
{n_assert("nApplication is in begin state" && !this->in_begin);
	while (!this->servers.empty())
	{
		nRoot* server = this->servers.back();
		this->servers.pop_back();
		server->Release();
	}

#ifdef LOG
	nKernelServer* _ks = this->kernelServer;
#ifdef __WIN32__
	if (this->winlog)
	{	
		_ks->RemoveLogHandler(this->winlog);
		delete this->winlog;
	}
#endif
	if (this->filelog)
	{	
		_ks->RemoveLogHandler(filelog);
		delete filelog;
	}
#endif
}