#define N_IMPLEMENTS nShaderPair
//------------------------------------------------------------------------------
// nshaderpair.cc
//  (C) 2006 Insightec Ltd, -- I.Kliot
//------------------------------------------------------------------------------
#include "gfx/nshaderpair.h"
#include "gfx/nshader.h"
#include "gfx/ngfxserver.h"

nNebulaClass(nShaderPair, "nroot");

nShaderPair::nShaderPair() : 
	refGfx(kernelServer, this),
	linked(false), 
	vertexRef(this), 
	fragmentRef(this) 
{
	this->refGfx = "/sys/servers/gfx";
}

nShaderPair::~nShaderPair() 
{
	release_ref(this->vertexRef);
	release_ref(this->fragmentRef);
}

/// load shaders from files
bool nShaderPair::Load(const char* vertex, const char* fragment) 
{	
	bool res = false;
	if ((vertex && strlen(vertex)) || (fragment && strlen(fragment)))
	{
		if (vertex && strlen(vertex))
		{
			this->log += "Loading vertex shader "; this->log += vertex; this->log += "\n";
			res = this->Load(vertex, N_VERTEX_SHADER, this->vertexRef);
		} 		

		if (fragment && strlen(fragment))
		{
			this->log += "Loading fragment shader "; this->log += fragment; this->log += "\n";
			res = this->Load(fragment, N_FRAGMENT_SHADER, this->fragmentRef) && res;		
		}		
		
		if (res)
			res = this->vertexRef.isvalid() || this->fragmentRef.isvalid();
	}	
	return res;
}	

bool nShaderPair::Load(const char* fname, nShaderType type, nRef<nShader>& shaderRef)
{
	release_ref(shaderRef);
	if (!fname)
		return true;

	bool res = true;

	// see if texture already exists and can be shared
	nShader* shader = this->refGfx->FindShader(fname);
	if (!shader)
	{
		shader = this->refGfx->NewShader(fname);
		n_assert(shader);
		res = shader->Load(fname, type);
	}
	else
		res = shader->Compiled();

	if (shader && res)
		shaderRef = shader;

	return res;
}


