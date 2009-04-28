#define N_IMPLEMENTS nShaderProgramNode
//------------------------------------------------------------------------------
//  (C) 2006	Ilya Kliot
//------------------------------------------------------------------------------
#ifndef N_SL_PROGRAM
#include "node/nshaderprogramnode.h"
#endif
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif
#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif
#ifndef N_PIXELSHADER_H
#include "gfx/npixelshader.h"
#endif

nNebulaScriptClass(nShaderProgramNode, "nshadernode");

nShaderProgramNode::nShaderProgramNode() : nShaderNode(), initialized(false) 
{
	this->vertexFilename.clear();
	this->fragmentFilename.clear();
}

nShaderProgramNode::~nShaderProgramNode () 
{
}

void nShaderProgramNode::Update() 
{
	nPixelShader* ps = this->GetPixelShader();
	if (!ps->GetActiveProgram())
		return;
	if (!this->initialized && (this->fragmentFilename.size() || this->vertexFilename.size())) 
	{
		this->initialized = ps->LoadShaders(this->vertexFilename.c_str(), this->fragmentFilename.c_str());
	}
	if (this->initialized) 
	{
		ps->Bind();	
		int* i = 0;
		float* f = 0;
	//	color4* c4 = 0;
		vector4* v4 = 0;
		vector3* v3 = 0;
		vector2* v2 = 0;

		matrix44* m4 = 0;
		matrix33* m3 = 0;

		this->UpdateList(ps, i);
		this->UpdateList(ps, f);
	//	this->UpdateList(ps, c4);
		this->UpdateList(ps, v4);
		this->UpdateList(ps, v3);
		this->UpdateList(ps, v2);
		this->UpdateList(ps, m4);
		this->UpdateList(ps, m3);		
		
		if (!this->GetAttribVector4List()->empty()) 
		{
			tNamedParamList<vector4>* il = this->GetAttribVector4List();
			for (tNamedParamList<vector4>::iterator i = il->begin(); i != il->end(); i++) 
			{				
				if (i->second.desc == -1)
					i->second.desc = ps->GetIndexForAttrName(i->first.c_str());
				if (i->second.desc >= 0)
					ps->SetAttribute(i->second.desc, i->second.param);				
			}			
		}		
		ps->Unbind();
	}	
}

void nShaderProgramNode::Compute(nSceneGraph2* sceneGraph) 
{
	this->Update();	
	nShaderNode::Compute(sceneGraph);	
}

/// set int named param
void nShaderProgramNode::SetTexture( const char* name, int texUnit)
{			
	this->texUnits[name] = texUnit;	
}
/// get texture unit named param
int nShaderProgramNode::GetTexture( const char* name, long& err) 
{		
	const tSLParam<int>* p = this->texUnits.find_by_name(name);
	err = 0;
	if (p)
		return p->param;
	else {
		err = UNDEFINED_PARAM;
		return 0;
	}
}
// attribute variables handling methods
/// set vector4 attribute
void nShaderProgramNode::SetAttribute( const char* name, const vector4& value ) 
{			
	this->attribVector4[name] = value;
}
/// get vector4 attribute
vector4 nShaderProgramNode::GetAttribute( const char* name, long& err ) 
{	
	const tSLParam<vector4>* p = this->attribVector4.find_by_name(name);
	err = 0;
	if (p)
		return p->param;
	else {
		err = UNDEFINED_PARAM;
		return vector4();
	}
}


