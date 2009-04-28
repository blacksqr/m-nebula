#ifndef N_SL_PROGRAM
#define N_SL_PROGRAM

#include "util/nstl.h"
//-------------------------------------------------------------------
/** 
	template class for shader program parameter
	that should be keeped in nSghaderProgram for persistance
*/
template <class _T_>
class tSLParam
{
public:
	tSLParam() : desc(-1) {}
	tSLParam(const _T_& p) : desc(-1), param(p) {}
	_T_			param;
	int			desc;
};

//-------------------------------------------------------------------
/** 
	template class for shader program parameters list
	that should be keep in nSghaderProgram for persistence
*/
template <class _T_>
class tNamedParamList : public std::map<stl_string, tSLParam<_T_>> 
{
public:
	tSLParam<_T_>* find_by_name(const stl_string& name)
	{
		tNamedParamList::iterator i = this->find(name);
		if (i != this->end())
			return &(i->second);
		else
			return 0;
	}
};

#define  UNDEFINED_PARAM -1

#ifndef N_COLOR_H
#include "mathlib/color.h"
#endif
#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#include "node/nshadernode.h"

#undef N_DEFINES
#define N_DEFINES nShaderProgramNode
#include "kernel/ndefdllclass.h"
//-------------------------------------------------------------------
/**
    @class nShaderProgramNode

    @brief provides wrapping to shader languages like HLSL and GLSL    
	inserted before shadernode to avoid major changes	
*/
class N_PUBLIC nShaderProgramNode : public nShaderNode {
protected:    
    protected:    	
	tNamedParamList<int>		uniformInt;
	tNamedParamList<float>		uniformFloat;
	//tNamedParamList<color4>		uniformColor;
    tNamedParamList<vector4>	uniformVector4;
	tNamedParamList<vector3>	uniformVector3;
	tNamedParamList<vector2>	uniformVector2;
	tNamedParamList<matrix33>	uniformMatrix33;
	tNamedParamList<matrix44>	uniformMatrix44;	

	tNamedParamList<int>		texUnits;

	tNamedParamList<vector4>	attribVector4;

	stl_string	vertexFilename;
	stl_string	fragmentFilename;	


public:
	static nKernelServer *kernelServer;
	
    nShaderProgramNode();
    virtual ~nShaderProgramNode();

	virtual void Compute(nSceneGraph2*);
	virtual bool SaveCmds(nPersistServer*);
	
public:	
	void GetUniformList(tNamedParamList<int>*& lst)      {lst = &this->uniformInt;}
	void GetUniformList(tNamedParamList<float>*& lst)    {lst = &this->uniformFloat;}
//	void GetUniformList(tNamedParamList<color4>*& lst)	 {lst = &this->uniformColor;}
	void GetUniformList(tNamedParamList<vector4>*& lst)  {lst = &this->uniformVector4;}
	void GetUniformList(tNamedParamList<vector3>*& lst)  {lst = &this->uniformVector3;}
	void GetUniformList(tNamedParamList<vector2>*& lst)  {lst = &this->uniformVector2;}
	void GetUniformList(tNamedParamList<matrix44>*& lst) {lst = &this->uniformMatrix44;}
	void GetUniformList(tNamedParamList<matrix33>*& lst) {lst = &this->uniformMatrix33;}

	tNamedParamList<int>* GetTextureList() {return &texUnits;}

	tNamedParamList<vector4>* GetAttribVector4List() {return &attribVector4;}	

	/// load shaders from files :TODO - control set names in case it second or more time call
    virtual void LoadShaders ( const char* vertex, const char* fragment) 
	{
		this->initialized = false;
		if (vertex)
			this->vertexFilename = vertex;
		if (fragment)
			this->fragmentFilename = fragment;				
    }	

	// uniform variables handling methods
	/// set named param
	template <class Type>
	void SetUniform( const char* name, const Type& value) 
	{			
		tNamedParamList<Type>* lst = 0;
		this->GetUniformList(lst);
		if (lst)
			(*lst)[name] = value;		
	}

	/// get named param
	template <class Type>
	void GetUniform( const char* name, Type& i,  long& err)  {		
		tNamedParamList<Type>* lst = 0;
		this->GetUniformList(lst);
		if (lst)
		{
			const tSLParam<Type>* p = lst->find_by_name(name);
			err = 0;
			if (p)
				i = p->param;
			else 	
				err = UNDEFINED_PARAM;
		}		
	}	
	
	/// set texture named param
	virtual void SetTexture( const char * name, int texUnit);
	/// get texture unit named param
	virtual int GetTexture( const char * name, long& err);

	// attribute variables handling methods
	/// set vector4 attribute
    virtual void SetAttribute( const char * name, const vector4& value );
	/// get vector4 attribute
    virtual vector4 GetAttribute( const char * name, long& err );
    
protected:

	void Update();

	template <class Type>	
	void UpdateList(nPixelShader* ps, Type*& n)
	{		
		tNamedParamList<Type>* il = 0;
		this->GetUniformList(il);
		if (il && !il->empty()) 
		{			
			for (tNamedParamList<Type>::iterator i = il->begin(); i != il->end(); i++) 
			{				
				if (i->second.desc == -1)
					i->second.desc = ps->GetLocationForUniformName(i->first.c_str());
				if (i->second.desc >= 0)
					ps->SetUniform(i->second.desc, i->second.param);					
			}			
		}
		n = 0;
	}	
	bool		initialized;  

};

#endif
