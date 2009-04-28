#ifndef N_SHADERPAIR_H
#define N_SHADERPAIR_H

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif
#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif
#ifndef N_REF_H
#include "kernel/nref.h"
#endif
#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif
#ifndef N_STL_H
#include "util/nstl.h"
#endif


#undef N_DEFINES
#define N_DEFINES nShaderPair
#include "kernel/ndefdllclass.h"

class nShader;
class nGfxServer;

#pragma warning( push )
#pragma warning( disable : 4100 )
//-------------------------------------------------------------------
/**
    @class nShaderPair

    @brief provides wrapping to shader languages like HLSL and GLSL    
*/
class N_PUBLIC nShaderPair : public nRoot {

public:
	static nKernelServer *kernelServer;
	
    nShaderPair  ();
    virtual ~nShaderPair ();
	
	/// load shaders from files
    virtual bool Load(const char * vertex, const char * fragment);

	/// load shaders from files
	virtual bool Load(const char* fname, nShaderType type, nRef<nShader>& shaderRef);

	virtual bool Linked() const { return this->linked; }
protected:
	nAutoRef<nGfxServer>	refGfx;

	nRef<nShader>			vertexRef;
	nRef<nShader>			fragmentRef;

	stl_string				log;

	bool					linked;
};
#pragma warning( pop )
#endif
