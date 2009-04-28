#ifndef N_SHADER_H
#define N_SHADER_H

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#include "gfx/ngfxtypes.h"
#include "util/nstl.h"

#undef N_DEFINES
#define N_DEFINES nShader
#include "kernel/ndefdllclass.h"

#pragma warning( push )
#pragma warning( disable : 4100 )	

//-------------------------------------------------------------------
/**
	@class nShader

	@brief provides wrapping service to vertex/fragment shader of 
	HLSL or GLSL    
*/
class N_PUBLIC nShader : public nRoot {
public:
	nShader() : compiled(false) {}
	virtual ~nShader() {}

	virtual bool Load(const char* fileName, nShaderType type);
	virtual bool Compiled() const { return this->compiled; }

public:
	static nKernelServer *kernelServer;

protected:
	virtual bool	file2Buffer(const char* fname);	

	bool			compiled;
	nShaderType		type;
	byte_buffer		data;	
	stl_string		log;
};



#endif