#define N_IMPLEMENTS nGlSlShader
//------------------------------------------------------------------------------
// nglslshader.cc
//  (C) 2007 Insightec Ltd, -- I.Kliot
//------------------------------------------------------------------------------
#include "gfx/nglslshader.h"


nNebulaClass(nGlSlShader, "nshader");

nGlSlShader::nGlSlShader() : nShader(), shader(0) {}

nGlSlShader::~nGlSlShader()
{
	if (this->shader)
	{
		glDeleteObjectARB(shader);
		this->shader = 0;				
	}
}

bool nGlSlShader::Load(const char* fileName, nShaderType _type) 
{	
	if (nShader::Load(fileName, _type))
	{		
		
		this->shader = glCreateShaderObjectARB( 
			(this->type == N_VERTEX_SHADER ? GL_VERTEX_SHADER_ARB : GL_FRAGMENT_SHADER_ARB)
			);	    

		const char*	 body = (const char*)this->data.get_buffer();
		int			 len  = this->data.size();
		GLint        compileStatus;

		glShaderSourceARB( this->shader, 1, &body,  &len);

		// compile the particle vertex shader, and print out
		glCompileShaderARB(this->shader);	

		this->data.clear();		

		glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);	

		this->compiled = (compileStatus != 0);	

		if (this->compiled)
			this->log += ">ok\n";

		loadGLSLLog(this->shader, this->log);
		
		n_printf("%s\n", this->log.c_str());  

		return this->compiled;
	}
	else
	{
		return false;
	}
}
