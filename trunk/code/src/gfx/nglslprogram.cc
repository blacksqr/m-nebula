	#define N_IMPLEMENTS nGlSlProgram

//--------------------------------------------------------------------
/**
	GLSL support for nebula
	2006 by  Insightec, Inc, Ilya Kliot
	based on samples of Alex V. Boreskoff 
*/

#include "gfx/nglslprogram.h"
#include "gfx/nglslpair.h"

nNebulaClass(nGlSlProgram, "npixelshader");
nGlSlProgram::nGlSlProgram () : nPixelShader(), program(0) { }

nGlSlProgram::~nGlSlProgram (){}

bool nGlSlProgram::LoadShaders(const char* vertexFilename, const char* fragmentFilename) 
{
	if (nShaderProgram::LoadShaders(vertexFilename, fragmentFilename))
	{
		nGlSlPair* pair = (nGlSlPair*)(this->pairRef.isvalid() ? this->pairRef.get() : 0);
		if (pair && pair->Linked())
		{
			this->program = pair->GetProgram();
			n_printf("Linked program, id %d\n", (long)this->program);			
			return true;
		}
	}
	this->activeProgram = false;
	return false;                                
}

stl_string nGlSlProgram::version() 
{
    const char* slVer = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);

    if (glGetError() != GL_NO_ERROR )
        return "1.051";

    return stl_string(slVer);
}

int nGlSlProgram::maxVertexUniformComponents ()
{
    int maxVertexUniformComponents;

    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &maxVertexUniformComponents);

    return maxVertexUniformComponents;
}

int nGlSlProgram::maxVertexAttribs ()
{
    int maxVertexAttribs;

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &maxVertexAttribs);

    return maxVertexAttribs;
}

int nGlSlProgram::maxFragmentTextureUnits ()
{
    int maxFragmentTextureUnits;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &maxFragmentTextureUnits);

    return maxFragmentTextureUnits;
}

int nGlSlProgram::maxVertexTextureUnits ()
{
    int maxVertexTextureUnits;

    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, &maxVertexTextureUnits);

    return maxVertexTextureUnits;
}

int nGlSlProgram::maxCombinedTextureUnits ()
{
    int maxCombinedTextureUnits;

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &maxCombinedTextureUnits);

    return maxCombinedTextureUnits;
}

int nGlSlProgram::maxVaryingFloats ()
{
    int maxVaryingFloats;

    glGetIntegerv(GL_MAX_VARYING_FLOATS_ARB, &maxVaryingFloats);


    return maxVaryingFloats;
}

int nGlSlProgram::maxFragmentUniformComponents ()
{
    int maxFragmentUniformComponents;

    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB, &maxFragmentUniformComponents);

    return maxFragmentUniformComponents;
}

int nGlSlProgram::maxTextureCoords ()
{
    int maxTextureCoords;

    glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &maxTextureCoords);

    return maxTextureCoords;
}

