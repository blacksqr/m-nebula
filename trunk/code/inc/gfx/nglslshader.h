#ifndef N_GLSLSHADER_H
#define N_GLSLSHADER_H
//--------------------------------------------------------------------
/**
    @class nGlSlShader

    @brief OpenGL implementation of nshader 
	2007 by  Insightec, Inc, Ilya Kliot
*/

#include "gfx/nshader.h"

#ifndef N_GLSERVER_H
#include "gfx/nglserver.h"
#endif


//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nGlSlShader
#include "kernel/ndefdllclass.h"

class   nGlSlShader : public nShader {
public:
    nGlSlShader();
    virtual ~nGlSlShader();

public:
	// load shader
	virtual bool Load(const char* fileName, nShaderType _type);  
	GLhandleARB GetShader() { return this->shader; }

public:
	static nKernelServer *kernelServer;

protected:	
	GLhandleARB shader;  	
};

#endif
