#ifndef N_SHADERPROGRAM_H
#define N_SHADERPROGRAM_H

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif
#ifndef N_REF_H
#include "kernel/nref.h"
#endif
#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif
#ifndef N_SHADERPAIR_H
#include "gfx/nshaderpair.h"
#endif
#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif
#ifndef N_COLOR_H
#include "mathlib/color.h"
#endif


#define MAX_PARAM_NAME 64

#undef N_DEFINES
#define N_DEFINES nShaderProgram
#include "kernel/ndefdllclass.h"

#pragma warning( push )
#pragma warning( disable : 4100 )	


//-------------------------------------------------------------------
/**
    @class nShaderProgram

    @brief provides wrapping to shader languages like HLSL and GLSL    
*/
class N_PUBLIC nShaderProgram : public nRoot {
public:	
    nShaderProgram  ();
    virtual ~nShaderProgram ();

public:
	static nKernelServer *kernelServer;

public:	
	/// load shaders from files
    virtual bool LoadShaders(const char * vertex, const char * fragment);

	// uniform variables handling methods	
	/// set int named param
	virtual void SetUniform(int loc, int value) {
		n_error("Pure virtual function called.\n");
	}
	/// set float named param
	virtual void SetUniform(int loc, float value){
		n_error("Pure virtual function called.\n");
	}
	/// set vector4 named param
    virtual void SetUniform(int loc, const color4& value){
		n_error("Pure virtual function called.\n");
	}
	/// set vector4 named param
    virtual void SetUniform(int loc, const vector4& value){
		n_error("Pure virtual function called.\n");
	}
	/// set vector3 named param
    virtual void SetUniform(int loc, const vector3& value){
		n_error("Pure virtual function called.\n");
	}
	/// set vector2 named param
    virtual void SetUniform(int loc, const vector2& value){
		n_error("Pure virtual function called.\n");
	}
	/// set matrix44 named param
    virtual void SetUniform(int loc, const matrix44& value){
		n_error("Pure virtual function called.\n");
	}
	/// set matrix33 named param
    virtual void SetUniform(int loc, const matrix33& value){
		n_error("Pure virtual function called.\n");		
	}		
	
	// attribute variables handling methods
	/// set vector4 attribute
    virtual void SetAttribute(int loc, const vector4& value) {
		n_error("Pure virtual function called.\n");	
	}

	virtual int GetLocationForUniformName(const char* name) {
		n_error("Pure virtual function called.\n");
		return -1;
	}

	virtual int GetIndexForAttrName(const char * name) {
		n_error("Pure virtual function called.\n");
		return -1;		
	}

	virtual void Bind() {
		n_error("Pure virtual function called.\n");
	}

	virtual void Unbind() {
		n_error("Pure virtual function called.\n");
	}

	virtual void SetActiveProgram(bool b) {
		this->activeProgram = b;
	}

	virtual bool GetActiveProgram() const {
		return this->activeProgram;
	}

protected:	
	nAutoRef<nGfxServer> refGfx;
	nRef<nShaderPair>	pairRef;
	bool				activeProgram;	
	bool				initialized;  
};
#pragma warning( pop )
#endif
