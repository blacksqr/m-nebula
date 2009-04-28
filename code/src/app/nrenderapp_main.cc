#define N_IMPLEMENTS nRenderApp
//------------------------------------------------------------------------------
//  (C) 2009	plushy
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "gfx/ngfxserver.h"
#include "node/n3dnode.h"

#include "app/nrenderapp.h"

nNebulaScriptClass(nRenderApp, "napplication");

//------------------------------------------------------------------------------
/**
*/
nRenderApp::nRenderApp() : 
	refGfx(kernelServer, this),
		refChannel(kernelServer, this),
		refSG(kernelServer, this),
		refScene(kernelServer, this),
		refShadow(kernelServer, this),
		gfx_class("nglserver"),	
		//gfx_class("nd3d8server"),			
		mode("w(840)-h(525)"),
		grid(false)
{
}

//------------------------------------------------------------------------------
/**
*/
bool nRenderApp::initialize()
{	
	if (nApplication::initialize() && 
		this->AddServer(this->kernelServer, this->gfx_class.c_str(), "/sys/servers/gfx", &this->refGfx) &&
		this->AddServer(this->kernelServer, "nscenegraph2",		"/sys/servers/sgraph2", &this->refSG) &&		
		this->AddServer(this->kernelServer, "nchannelserver" ,	"/sys/servers/channel", &this->refChannel) &&
		this->AddServer(this->kernelServer, "nsbufshadowserver", "/sys/servers/shadow", &this->refShadow) &&
		this->kernelServer->New("n3dnode", "/usr/scene")
		) 
	{			
		this->refScene = "/usr/scene";
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
bool nRenderApp::begin()
{
	if (nApplication::begin())
	{
		this->refGfx->SetDisplayMode(this->mode.c_str());		

		if (this->refGfx->OpenDisplay()) 
		{	
			this->kernelServer->ts->ResetTime();			
			this->refScene->RenderContextCreated(0);
			this->grid |= (this->startup_script.empty() && this->noinit);
			if (this->refScene.isvalid())
				this->kernelServer->SetCwd(this->refScene.get());
			return true;
		}
		return false;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void nRenderApp::end()
{
	this->refScene->RenderContextDestroyed(0);	
	this->refGfx->CloseDisplay();
	nApplication::end();
}

//------------------------------------------------------------------------------
/**
*/
bool nRenderApp::process_command_line()
{
	if (nApplication::process_command_line())
	{
		nKernelServer* _ks = this->kernelServer;
		bool argerr = false;
		int argc = _ks->GetArgc();
		
		for (long i = 1; i < argc && !argerr; i++) 
		{
			const char* arg = _ks->GetArgv(i);
			if (strcmp(arg, "-grid") == 0) this->grid = true;
			else if (strcmp(arg, "-gfx") == 0) 
			{
				if (++i < argc) this->gfx_class = _ks->GetArgv(i);
				else argerr = true;
			} 
			else if (strcmp(arg, "-mode") == 0) 
			{
				if (++i < argc) this->mode = _ks->GetArgv(i);
				else argerr = true;
			} 			
		}

		if (argerr) 
		{
			n_printf("nApplication::process_command_line->Command line arguments error, exiting.\n"); 
			return false;
		}
		
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
/**
*/
nRenderApp::~nRenderApp()
{
	this->refScene->Release();
}

