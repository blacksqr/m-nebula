#define N_IMPLEMENTS nGlSlPair

//--------------------------------------------------------------------
/**
	GLSL support for nebula
	2006 by  Insightec, Inc, Ilya Kliot
	based on samples of Alex V. Boreskoff 
*/
#include "gfx/nglslpair.h"
#include "gfx/nglslshader.h"

nNebulaClass(nGlSlPair, "nshaderpair");

nGlSlPair::nGlSlPair () : nShaderPair(), pair(0)
{    
}

nGlSlPair::~nGlSlPair ()
{
	this->clear();
}

void nGlSlPair::clear () {
    glDeleteObjectARB(this->pair);                   // it will also detach shaders    
    this->pair = 0;    
}

bool nGlSlPair::Load(const char* vertexFilename, const char* fragmentFilename) 
{
	if (nShaderPair::Load(vertexFilename, fragmentFilename))
	{
		if (this->pair)
			this->clear();

		if (!this->pair)
			this->pair = glCreateProgramObjectARB();

		if (nCheckGlError())
		{
			this->pair = 0;
			this->linked = false;			
		}
		else
		{	
			nGlSlShader* vertex = (nGlSlShader*)(this->vertexRef.isvalid() ? this->vertexRef.get() : 0);
			nGlSlShader* fragment = (nGlSlShader*)(this->fragmentRef.isvalid() ? this->fragmentRef.get() : 0);

			// attach shaders to program object
			if (vertex)
				glAttachObjectARB(this->pair, vertex->GetShader());
			if (fragment)
				glAttachObjectARB(this->pair, fragment->GetShader());

			GLint islinked;
			this->log += "Linking programs\n";

			// link the program object and print out the info log
			glLinkProgramARB(this->pair);		

			glGetObjectParameterivARB(this->pair, GL_OBJECT_LINK_STATUS_ARB, &islinked);	

			this->linked = (islinked != 0);
			
			if (this->linked)
				this->log += ">ok\n";    

		}

		loadGLSLLog(this->pair, this->log);
		n_printf("%s\n",log.c_str());
	}

	return this->Linked();
}
