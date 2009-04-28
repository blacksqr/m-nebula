#define N_IMPLEMENTS nShaderProgram
//------------------------------------------------------------------------------
// nshaderprogram.cc
//  (C) 2006 Insightec Ltd, -- I.Kliot
//------------------------------------------------------------------------------
#include "gfx/nshaderprogram.h"
#include "gfx/ngfxserver.h"
#include "kernel/nfileserver2.h"


nNebulaClass(nShaderProgram, "nroot");


nShaderProgram::nShaderProgram() : 
	initialized(false), 
		activeProgram(false), 
		refGfx(kernelServer, this),
		pairRef(this) 
{        
	this->refGfx = "/sys/servers/gfx";
}

nShaderProgram::~nShaderProgram() {
	// invalidate current font
	release_ref(this->pairRef);
}


/// load shaders from files
bool nShaderProgram::LoadShaders(const char* vertex, const char* fragment) 
{
	release_ref(this->pairRef);
	nFileServer2* fs = this->kernelServer->GetFileServer2();	

	stl_string path_vsh("no_vertex");	
	
	if (vertex)
	{
		fs->MakeAbsoluteMangled(vertex, path_vsh);		
	}	

	stl_string id(path_vsh);

	stl_string path_fsh("no_fragment");
	const char* fsh_fname = path_fsh.c_str();
	
	if (fragment) 
	{
		fs->MakeAbsoluteMangled(fragment, path_fsh);		
		fsh_fname = strrchr(path_fsh.c_str(), '/') + 1;		
	}	

	id += '_';
	id += fsh_fname;

	// see if texture already exists and can be shared
	nShaderPair* pair = this->refGfx->FindShaderPair(id.c_str());
	if (!pair)
	{
		pair = this->refGfx->NewShaderPair(id.c_str());
		n_assert(pair);
		if (!(vertex && fragment))
			n_printf("Loading program %s without %s shader\n", id.c_str(), vertex ? "fragment" : "vertex");

		this->initialized = pair->Load(vertex, fragment);
	}
	else
		this->initialized = pair->Linked();

	if (pair && this->initialized)
		this->pairRef = pair;

	return this->initialized;
}	