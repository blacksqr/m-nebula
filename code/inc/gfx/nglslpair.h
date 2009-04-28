#ifndef N_GLSLPAIR_H
#define N_GLSLPAIR_H
//--------------------------------------------------------------------
/**
    @class nGlSlProgram

    @brief OpenGL implementation of shader program
	2006 by  Insightec, Inc, Ilya Kliot
*/
#ifndef N_GLSERVER_H
#include "gfx/nglserver.h"
#endif

#ifndef N_SHADERPAIR_H
#include "gfx/nshaderpair.h"
#endif

#include "util/nstl.h"

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nGlSlPair
#include "kernel/ndefdllclass.h"

class nGlSlPair : public nShaderPair {
public:
    nGlSlPair();
    virtual ~nGlSlPair();

	static nKernelServer *kernelServer;

	/// load shaders from files
	virtual bool Load(const char * vertex, const char * fragment);

	GLhandleARB GetProgram() { return this->pair; }

protected:   	
	void clear();    
    
protected:

	GLhandleARB pair;
	stl_string	error;	
};

#endif
