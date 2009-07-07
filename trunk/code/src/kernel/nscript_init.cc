#define N_IMPLEMENTS nScriptServer
#define N_KERNEL
//--------------------------------------------------------------------
//  IMPLEMENTATION
//      nScriptServer -- Superklasse fuer konkrete ScriptServer
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"

nClass *nScriptServer::local_cl  = NULL;
nKernelServer *nScriptServer::kernelServer = NULL;

extern char *nkernel_version;
extern "C" bool N_EXPORT nscriptserver_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT nscriptserver_fini(void);
extern "C" N_EXPORT void *nscriptserver_new(void);
extern "C" N_EXPORT char *nscriptserver_version(void);

//--------------------------------------------------------------------
//  n_init()
//  09-Oct-98   floh    created
//--------------------------------------------------------------------
bool N_EXPORT nscriptserver_init(nClass *cl, nKernelServer *ks)
{
    nScriptServer::local_cl = cl;
    nScriptServer::kernelServer = ks;
    ks->AddClass("nroot",cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//  09-Oct-98   floh    created
//--------------------------------------------------------------------
void N_EXPORT nscriptserver_fini(void)
{
    nScriptServer::kernelServer->RemClass(nScriptServer::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//  09-Oct-98   floh    created
//--------------------------------------------------------------------
N_EXPORT void *nscriptserver_new(void)
{
    return n_new nScriptServer;
}

//--------------------------------------------------------------------
//  n_version()
//  09-Oct-98   floh    created
//--------------------------------------------------------------------
N_EXPORT char *nscriptserver_version(void)
{
    return nkernel_version;
}

//--------------------------------------------------------------------
//  ~nScriptServer()
//  31-Oct-98   floh    created
//  21-Jul-99   floh    korrektes Aufraeumen, wenn im Server oder
//                      Client-Modus
//--------------------------------------------------------------------
nScriptServer::~nScriptServer()
{
	this->extensions.clear();
}


nScriptServer::nScriptServer() :
	indent_level(0),
    quitRequested(false),
    failOnError(false)
{
	this->extensions.clear();
    SetSelectMethod(SELCOMMAND);
    this->indent_buf.clear();
}
//--------------------------------------------------------------------
//  Run()
//  10-Oct-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::Run(const char *, const char*&)
{
    n_error("nScriptServer::Run pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  RunScript()
//  10-Oct-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::RunScript(const char *, const char*&)
{
    n_error("nScriptServer::RunScript pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  RunScript()
//  10-Oct-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::RunScriptFS(const char *, const char*&)
{
	n_error("nScriptServer::RunScriptFS pure virtual function called!");
	return false;
}

//--------------------------------------------------------------------
//  BeginWrite()
//--------------------------------------------------------------------
nFile*
nScriptServer::BeginWrite(const char * /*filename */, nRoot * /* obj */)
{
	n_error("nScriptServer::BeginWrite pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteBeginNewObject()
//  04-Nov-98   floh    created
//  06-Mar-00   floh    + new name
//--------------------------------------------------------------------
bool nScriptServer::WriteBeginNewObject(nFile *, nRoot *, nRoot *)
{
    n_error("nScriptServer::WriteBeginNewObject pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteBeginNewObjectCmd()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::WriteBeginNewObjectCmd(nFile *, nRoot *, nRoot *, nCmd *)
{
    n_error("nScriptServer::WriteBeginNewObjectCmd pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteBeginSelObject()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::WriteBeginSelObject(nFile *, nRoot *, nRoot *)
{
    n_error("nScriptServer::WriteBeginSelObject pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteCmd()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::WriteCmd(nFile *, nCmd *)
{
    n_error("nScriptServer::WriteCmd pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteEndObject()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::WriteEndObject(nFile *, nRoot *, nRoot *)
{
    n_error("nScriptServer::WriteEndObject pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteEnd()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool
nScriptServer::EndWrite(nFile *)
{
    n_error("nScriptServer::EndWrite pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  GetCmdBegin()
//  27-May-09   Ilya    created
//--------------------------------------------------------------------
const char* 
nScriptServer::GetCmdBegin() const
{
	n_error("nScriptServer::GetCmdParamBegin() pure virtual function called!");
	return 0;
}

//--------------------------------------------------------------------
//  GetCmdEnd()
//  27-May-09   Ilya    created
//--------------------------------------------------------------------
const char* 
nScriptServer::GetCmdEnd() const
{
	n_error("nScriptServer::GetCmdEnd() pure virtual function called!");
	return 0;
}

//--------------------------------------------------------------------
//  GetCmdParamBegin()
//  17-Dec-07   Ilya    created
//--------------------------------------------------------------------
const char* 
nScriptServer::GetCmdParamBegin() const
{
	n_error("nScriptServer::GetCmdParamBegin() pure virtual function called!");
	return 0;
}

//--------------------------------------------------------------------
//  GetCmdParamDelim()
//  17-Dec-07   Ilya    created
//--------------------------------------------------------------------	
const char* 
nScriptServer::GetCmdParamDelim() const
{
	n_error("nScriptServer::GetCmdParamDelim() pure virtual function called!");
	return 0;
}

//--------------------------------------------------------------------
//  GetCmdParamEnd()
//  17-Dec-07   Ilya    created
//--------------------------------------------------------------------
const char* 
nScriptServer::GetCmdParamEnd() const
{
	n_error("nScriptServer::GetCmdParamEnd() pure virtual function called!");
	return 0;
}


/// check if extension supported by script server
bool nScriptServer::IsExtSupported(const char* ext) const 
{ 
	for (strings_v::const_iterator i = this->GetExtensions().begin(); i != this->GetExtensions().end(); i++)
		if (!_stricmp(ext, i->c_str()))
			return true;
	return false;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

