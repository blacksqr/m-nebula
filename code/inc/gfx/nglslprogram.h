#ifndef N_GLSLPROGRAM_H
#define N_GLSLPROGRAM_H
//--------------------------------------------------------------------
/**
    @class nGlSlProgram

    @brief OpenGL implementation of shader program
	2006 by  Insightec, Inc, Ilya Kliot
*/
#ifndef N_GLSERVER_H
#include "gfx/nglserver.h"
#endif

#ifndef N_PIXELSHADER_H
#include "gfx/npixelshader.h"
#endif
#ifndef N_STL_H
#include "util/nstl.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nGlSlProgram
#include "kernel/ndefdllclass.h"

class   nGlSlProgram : public nPixelShader {
protected:

   
public:
    nGlSlProgram();
    virtual ~nGlSlProgram();

	static nKernelServer *kernelServer;        

	/// load shaders from files
	virtual bool LoadShaders(const char * vertex, const char * fragment);

	// uniform variables handling methods	
	
	/// set int named param
	virtual void SetUniform(int loc, int value) {
		glUniform1iARB(loc, value);
	}
	/// set float named param
	virtual void SetUniform(int loc, float value){
		glUniform1fARB(loc, value);
	}
	/// set vector4 named param
    virtual void SetUniform(int loc, const color4& value){
		glUniform4fvARB(loc, 1, value.v);
	}
	/// set vector4 named param
    virtual void SetUniform(int loc, const vector4& value){
		glUniform4fvARB(loc, 1, value.v);
	}
	/// set vector3 named param
    virtual void SetUniform(int loc, const vector3& value){
		glUniform3fvARB(loc, 1, value.v);
	}
	/// set vector2 named param
    virtual void SetUniform(int loc, const vector2& value){
		glUniform2fvARB(loc, 1, value.v);
	}
	/// set matrix44 named param
    virtual void SetUniform(int loc, const matrix44& value){
		glUniformMatrix4fvARB( loc, 1, GL_FALSE, &value.m[0][0]);
	}
	/// set matrix33 named param
    virtual void SetUniform(int loc, const matrix33& value){
		glUniformMatrix3fvARB( loc, 1, GL_FALSE, &value.m[0][0]);
	}		
	
	// attribute variables handling methods
	/// set vector4 attribute
    virtual void SetAttribute(int loc, const vector4& value) {
		glVertexAttrib4fvARB(loc, &value.x);
	}

	virtual int GetLocationForUniformName(const char* name) {
		return glGetUniformLocationARB(this->program, name);
	}

	virtual int GetIndexForAttrName(const char * name) {
		return glGetAttribLocationARB(this->program, name);
	}

	virtual void Bind() {
		//n_assert(glUseProgramObjectARB);
		if (this->program && this->GetActiveProgram())
			glUseProgramObjectARB(this->program);
	}

	virtual void Unbind() {
		if (this->program && this->GetActiveProgram())
			glUseProgramObjectARB(0);
	}
	// check whether there is a support for GLSL
   // static  bool    isSupported ();
	static stl_string version();
    
                                                        // some limitations on program
    static  int maxVertexUniformComponents   ();
    static  int maxVertexAttribs             ();
    static  int maxFragmentTextureUnits      ();
    static  int maxVertexTextureUnits        ();
    static  int maxCombinedTextureUnits      ();
    static  int maxVaryingFloats             ();
    static  int maxFragmentUniformComponents ();
    static  int maxTextureCoords             ();

protected:
	GLhandleARB program;
};

#endif
