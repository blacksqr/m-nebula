#define N_IMPLEMENTS nShaderProgramNode

//------------------------------------------------------------------------------

//  (C) 2006	Ilya Kliot

//------------------------------------------------------------------------------

#include "gfx/npixelshader.h"
#include "node/nshaderprogramnode.h"
#include "kernel/npersistserver.h"

static void n_loadshaders(void *o, nCmd *cmd);
static void n_setuniformint(void *o, nCmd *cmd);	
static void n_setuniformfloat (void *o, nCmd *cmd);	
static void n_setuniformvector4(void *o, nCmd *cmd);
static void n_setuniformvector3(void *o, nCmd *cmd);
static void n_setuniformvector2(void *o, nCmd *cmd);
static void n_setuniformmatrix44(void *o, nCmd *cmd);
static void n_setuniformmatrix33(void *o, nCmd *cmd);
static void n_getuniformint(void *o, nCmd *cmd);
static void n_getuniformfloat(void *o, nCmd *cmd);
static void n_getuniformvector4(void *o, nCmd *cmd);
static void n_getuniformvector3(void *o, nCmd *cmd);
static void n_getuniformvector2(void *o, nCmd *cmd);
static void n_getuniformmatrix44(void *o, nCmd *cmd);
static void n_getuniformmatrix33(void *o, nCmd *cmd);
static void n_settexture(void *o, nCmd *cmd);
static void n_gettexture(void *o, nCmd *cmd);
static void n_setattribute(void *o, nCmd *cmd);
static void n_getattribute(void *o, nCmd *cmd);

static void n_setactiveprogram(void *o, nCmd *cmd);
static void n_getactiveprogram(void *o, nCmd *cmd);


//------------------------------------------------------------------------------
/**
    @scriptclass
    nshaderprogramnode    

    @superclass
    nshadernode    

    @classinfo
    Expands nshadernode by vertex and pixel shader programs
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
	cl->AddCmd("v_loadshaders_ss",			'LDSH', n_loadshaders);	
	cl->AddCmd("v_setuniformint_si",		'STUI', n_setuniformint);	
	cl->AddCmd("v_setuniformfloat_sf",		'STUF', n_setuniformfloat);	
	cl->AddCmd("v_setuniformvector4_sffff",	'SUV4', n_setuniformvector4);	
	cl->AddCmd("v_setuniformvector3_sfff",	'SUV3', n_setuniformvector3);	
	cl->AddCmd("v_setuniformvector2_sff",	'SUV2', n_setuniformvector2);	
	cl->AddCmd("v_setuniformmatrix44_sffffffffffffffff",'SUM4', n_setuniformmatrix44);	
	cl->AddCmd("v_setuniformmatrix33_sfffffffff",		'SUM3', n_setuniformmatrix33);	
	cl->AddCmd("i_getuniformint_s",			'GTUI', n_getuniformint);
	cl->AddCmd("f_getuniformfloat_s",		'GTUF', n_getuniformfloat);
	cl->AddCmd("ffff_getuniformvector4_s",  'GUV4', n_getuniformvector4);
	cl->AddCmd("fff_getuniformvector3_s",	'GUV3', n_getuniformvector3);
	cl->AddCmd("ff_getuniformvector2_s",	'GUV2', n_getuniformvector2);
	cl->AddCmd("ffffffffffffffff_getuniformmatrix44_s", 'GUM4', n_getuniformmatrix44);
	cl->AddCmd("fffffffff_getuniformmatrix33_s",		'GUM3', n_getuniformmatrix33);
	cl->AddCmd("v_settexture_si",			'STTX', n_settexture);
	cl->AddCmd("i_gettexture_s",			'GTTX', n_gettexture);
	cl->AddCmd("v_setattribute_sffff",		'STAT', n_setattribute);
	cl->AddCmd("ffff_getattribute_s",		'GTAT', n_getattribute);

	cl->AddCmd("v_setactiveprogram_b",		'STAP', n_setactiveprogram);
	cl->AddCmd("b_getactiveprogram_v",		'GTAP', n_getactiveprogram);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    loadshaders

    @input
    s, s - names of vertex and fragment shaders

    @output
    v

    @info
    loads shader program script files and compiles them 
*/
static void n_loadshaders(void *o, nCmd *cmd) 
{
	nShaderProgramNode* self = (nShaderProgramNode*) o;	
	const char* vertex = cmd->In()->GetS();
	const char* fragment = cmd->In()->GetS();	
	if (vertex && !_stricmp(vertex, "none")) vertex = 0;
	if (fragment && !_stricmp(fragment, "none")) fragment = 0;

	self->LoadShaders(vertex, fragment);
}


//------------------------------------------------------------------------------
/**
    @cmd
    setuniformint

    @input
    s, i - name of variable and it's int value passed to SL program

    @output
    v

    @info
    set int value of shader program named variable
*/
static void n_setuniformint(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	int val = cmd->In()->GetI();	
	self->SetUniform(name, val);
}


//------------------------------------------------------------------------------
/**
    @cmd
    setuniformfloat

    @input
    s, f - name of variable and it's float value passed to SL program

    @output
    v

    @info
    set float value of shader program named variable
*/
static void n_setuniformfloat (void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	float val = cmd->In()->GetF();
	self->SetUniform(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuniformvector4

    @input
    s, f f f f - name of variable and it's vector4 value passed to SL program

    @output
    v

    @info
    set vector4 value of shader program named variable
*/
static void n_setuniformvector4(void *o, nCmd *cmd) { 
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	float z = cmd->In()->GetF();
	float w = cmd->In()->GetF();
	vector4 val(x, y, z, w);
	self->SetUniform(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuniformvector3

    @input
    s, f f f - name of variable and it's vector3 value passed to SL program

    @output
    v

    @info
    set vector3 value of shader program named variable
*/
static void n_setuniformvector3(void *o, nCmd *cmd) { 
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	float z = cmd->In()->GetF();	
	vector3 val(x, y, z);
	self->SetUniform(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuniformvector2

    @input
    s, f f - name of variable and it's vector2 value passed to SL program

    @output
    v

    @info
    set vector2 value of shader program named variable
*/
static void n_setuniformvector2(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();	
	vector2 val(x, y);
	self->SetUniform(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuniformmatrix44

    @input
    s, 
	f f f f
	f f f f
	f f f f
	f f f f - name of variable and it's matrix44 value passed to SL program

    @output
    v

    @info
    set matrix44 value of shader program named variable
*/
static void n_setuniformmatrix44(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	float m11 = cmd->In()->GetF();
	float m12 = cmd->In()->GetF();
	float m13 = cmd->In()->GetF();
	float m14 = cmd->In()->GetF();
	float m21 = cmd->In()->GetF();
	float m22 = cmd->In()->GetF();
	float m23 = cmd->In()->GetF();
	float m24 = cmd->In()->GetF();
	float m31 = cmd->In()->GetF();
	float m32 = cmd->In()->GetF();
	float m33 = cmd->In()->GetF();
	float m34 = cmd->In()->GetF();
	float m41 = cmd->In()->GetF();
	float m42 = cmd->In()->GetF();
	float m43 = cmd->In()->GetF();
	float m44 = cmd->In()->GetF();
	matrix44 val(
		m11, m12, m13, m14,
		m21, m22, m23, m24,
		m31, m32, m33, m34,
		m41, m42, m43, m44);
	self->SetUniform(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuniformmatrix33

    @input
    s, 	
	f f f
	f f f
	f f f - name of variable and it's matrix33 value passed to SL program

    @output
    v

    @info
    set matrix33 value of shader program named variable
*/
static void n_setuniformmatrix33(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	float m11 = cmd->In()->GetF();
	float m12 = cmd->In()->GetF();
	float m13 = cmd->In()->GetF();
	
	float m21 = cmd->In()->GetF();
	float m22 = cmd->In()->GetF();
	float m23 = cmd->In()->GetF();
	
	float m31 = cmd->In()->GetF();
	float m32 = cmd->In()->GetF();
	float m33 = cmd->In()->GetF();
	
	matrix33 val(
		m11, m12, m13,
		m21, m22, m23,
		m31, m32, m33);
	self->SetUniform(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuniformint

    @input
    s - name of variable to query

    @output
    i - int value of given variable

    @info
    get int value of shader program named variable
*/
static void n_getuniformint(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	int val = 0;
	self->GetUniform(name, val, err);
	n_assert(!err);
	cmd->Out()->SetI(val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuniformfloat

    @input
    s - name of variable to query

    @output
    f - float value of given variable

    @info
    get float value of shader program named variable
*/
static void n_getuniformfloat(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	float val = 0.0f;
	self->GetUniform(name, val, err);
	n_assert(!err);
	cmd->Out()->SetF(val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuniformvector4

    @input
    s - name of variable to query

    @output
    f f f f - vector4 value of given variable

    @info
    get vector4 value of shader program named variable
*/
static void n_getuniformvector4(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	vector4 v;
	self->GetUniform(name, v, err);
	n_assert(!err);
	cmd->Out()->SetF(v.x);
	cmd->Out()->SetF(v.y);
	cmd->Out()->SetF(v.z);
	cmd->Out()->SetF(v.w);
}
//------------------------------------------------------------------------------
/**
    @cmd
    getuniformvector3

    @input
    s - name of variable to query

    @output
    f f f - vector3 value of given variable

    @info
    get vector3 value of shader program named variable
*/
static void n_getuniformvector3(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	vector3 v;
	self->GetUniform(name, v, err);
	n_assert(!err);
	cmd->Out()->SetF(v.x);
	cmd->Out()->SetF(v.y);
	cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuniformvector2

    @input
    s - name of variable to query

    @output
    f f - vector2 value of given variable

    @info
    get vector2 value of shader program named variable
*/
static void n_getuniformvector2(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	vector2 v;
	self->GetUniform(name, v, err);
	n_assert(!err);
	cmd->Out()->SetF(v.x);
	cmd->Out()->SetF(v.y);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuniformmatrix44

    @input
    s - name of variable to query

    @output
    f f f f- matrix44 value of given variable
	f f f f
	f f f f
	f f f f

    @info
    get matrix44 value of shader program named variable
*/
static void n_getuniformmatrix44(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	matrix44 m;
	self->GetUniform(name, m, err);
	n_assert(!err);
	cmd->Out()->SetF(m.M11);
	cmd->Out()->SetF(m.M12);
	cmd->Out()->SetF(m.M13);
	cmd->Out()->SetF(m.M14);
	cmd->Out()->SetF(m.M21);
	cmd->Out()->SetF(m.M22);
	cmd->Out()->SetF(m.M23);
	cmd->Out()->SetF(m.M24);
	cmd->Out()->SetF(m.M31);
	cmd->Out()->SetF(m.M32);
	cmd->Out()->SetF(m.M33);
	cmd->Out()->SetF(m.M34);
	cmd->Out()->SetF(m.M41);
	cmd->Out()->SetF(m.M42);
	cmd->Out()->SetF(m.M43);
	cmd->Out()->SetF(m.M44);	
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuniformmatrix33

    @input
    s - name of variable to query

    @output
    f f f - matrix33 value of given variable
	f f f
	f f f

    @info
    get matrix33 value of shader program named variable
*/
static void n_getuniformmatrix33(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	matrix33 m;
	self->GetUniform(name, m, err);
	n_assert(!err);
	cmd->Out()->SetF(m.M11);
	cmd->Out()->SetF(m.M12);
	cmd->Out()->SetF(m.M13);
	
	cmd->Out()->SetF(m.M21);
	cmd->Out()->SetF(m.M22);
	cmd->Out()->SetF(m.M23);	

	cmd->Out()->SetF(m.M31);
	cmd->Out()->SetF(m.M32);
	cmd->Out()->SetF(m.M33);		
}

//------------------------------------------------------------------------------
/**
    @cmd
    settexture

    @input
    s, i - name of texUnit variable 

    @output
    v

    @info
    set int value of texUnit variable 
*/
static void n_settexture(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	int val = cmd->In()->GetI();
	self->SetTexture(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettexture

    @input
    s - name of texUnit variable 

    @output
    i - texUnit

    @info
    set int value of texUnit variable 
*/
static void n_gettexture(void *o, nCmd *cmd){
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	int val = self->GetTexture(name, err);
	n_assert(!err);
	cmd->Out()->SetI(val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setattribute

    @input
    s, f f f f - name of variable and it's vector4 value passed as attribute of SL program

    @output
    v

    @info
    set vector4 value of shader program named variable
*/
static void n_setattribute(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	float z = cmd->In()->GetF();
	float w = cmd->In()->GetF();
	vector4 val(x, y, z, w);
	self->SetAttribute(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuniformvector4

    @input
    s - name of variable to query

    @output
    f f f f - vector4 value of given variable

    @info
    get vector4 value of shader program named variable
*/
static void n_getattribute(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;	
	const char* name = cmd->In()->GetS();
	long err = 0;	
	vector4 v = self->GetAttribute(name, err);
	n_assert(!err);
	cmd->Out()->SetF(v.x);
	cmd->Out()->SetF(v.y);
	cmd->Out()->SetF(v.z);
	cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
	@cmd
	setactiveprogram

	@input
	b - active value
	@output
	v

	@info
	set shader program active or not 
*/
void n_setactiveprogram(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;
	nPixelShader* shader = self->GetPixelShader();
	if (shader)
		shader->SetActiveProgram(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
	@cmd
	getactiveprogram

	@input
	v - none
	@output
	b - active value

	@info
	get shader program active or not 
*/
void n_getactiveprogram(void *o, nCmd *cmd) {
	nShaderProgramNode* self = (nShaderProgramNode*)o;
	nPixelShader* shader = self->GetPixelShader();
	if (shader)
		cmd->Out()->SetB(shader->GetActiveProgram());
}


//------------------------------------------------------------------------------

/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure

*/
bool
nShaderProgramNode::SaveCmds(nPersistServer* ps)
{
    if (nShaderNode::SaveCmds(ps))
    {		
		nCmd *cmd;
		if (!(this->vertexFilename.empty() && this->fragmentFilename.empty()))
		{			
		cmd = ps->GetCmd(this,'LDSH');			
		cmd->In()->SetS(this->vertexFilename.c_str());
		cmd->In()->SetS(this->fragmentFilename.c_str());
		ps->PutCmd(cmd);           
		}
		{
			tNamedParamList<int>* il = 0;
			this->GetUniformList(il);
			n_assert(il);
			for (tNamedParamList<int>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'STUI');			
				cmd->In()->SetS(i->first.c_str());
				cmd->In()->SetI(i->second.param);
				ps->PutCmd(cmd);           
			}
		}
		{
			tNamedParamList<float>* il = 0;
			this->GetUniformList(il);	
			n_assert(il);
			for (tNamedParamList<float>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'STUF');			
				cmd->In()->SetS(i->first.c_str());
				cmd->In()->SetF(i->second.param);				
				ps->PutCmd(cmd);           
			}
		}		
		{
			tNamedParamList<vector4>* il = 0;
			this->GetUniformList(il);
			n_assert(il);
			for (tNamedParamList<vector4>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'SUV4');			
				cmd->In()->SetS(i->first.c_str());
				cmd->In()->SetF(i->second.param.x);
				cmd->In()->SetF(i->second.param.y);
				cmd->In()->SetF(i->second.param.z);
				cmd->In()->SetF(i->second.param.w);
				ps->PutCmd(cmd);
			}
		}
		{
			tNamedParamList<vector3>* il = 0;
			this->GetUniformList(il);
			n_assert(il);
			for (tNamedParamList<vector3>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'SUV3');			
				cmd->In()->SetS(i->first.c_str());
				cmd->In()->SetF(i->second.param.x);
				cmd->In()->SetF(i->second.param.y);
				cmd->In()->SetF(i->second.param.z);				
				ps->PutCmd(cmd);
			}
		}
		{
			tNamedParamList<vector2>* il = 0;
			this->GetUniformList(il);
			n_assert(il);
			for (tNamedParamList<vector2>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'SUV3');			
				cmd->In()->SetS(i->first.c_str());
				cmd->In()->SetF(i->second.param.x);
				cmd->In()->SetF(i->second.param.y);
				ps->PutCmd(cmd);
			}
		}
		{
			tNamedParamList<matrix44>* il = 0;
			this->GetUniformList(il);
			n_assert(il);
			for (tNamedParamList<matrix44>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'SUM4');			
				cmd->In()->SetS(i->first.c_str());
				const matrix44& m = i->second.param;
				cmd->In()->SetF(m.M11);
				cmd->In()->SetF(m.M12);
				cmd->In()->SetF(m.M13);
				cmd->In()->SetF(m.M14);
				cmd->In()->SetF(m.M21);
				cmd->In()->SetF(m.M22);
				cmd->In()->SetF(m.M23);
				cmd->In()->SetF(m.M24);
				cmd->In()->SetF(m.M31);
				cmd->In()->SetF(m.M32);
				cmd->In()->SetF(m.M33);
				cmd->In()->SetF(m.M34);
				cmd->In()->SetF(m.M41);
				cmd->In()->SetF(m.M42);
				cmd->In()->SetF(m.M43);
				cmd->In()->SetF(m.M44);	
				ps->PutCmd(cmd);
			}
		}
		{
			tNamedParamList<matrix33>* il = 0;
			this->GetUniformList(il);
			n_assert(il);
			for (tNamedParamList<matrix33>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'SUM3');			
				cmd->In()->SetS(i->first.c_str());
				const matrix33& m = i->second.param;
				cmd->In()->SetF(m.M11);
				cmd->In()->SetF(m.M12);
				cmd->In()->SetF(m.M13);
				cmd->In()->SetF(m.M21);
				cmd->In()->SetF(m.M22);
				cmd->In()->SetF(m.M23);
				cmd->In()->SetF(m.M31);
				cmd->In()->SetF(m.M32);
				cmd->In()->SetF(m.M33);	
				ps->PutCmd(cmd);
			}
		}
		{
			const tNamedParamList<int>* il = this->GetTextureList();
			for (tNamedParamList<int>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'STTX');			
				cmd->In()->SetS(i->first.c_str());
				cmd->In()->SetI(i->second.param);
				ps->PutCmd(cmd);           
			}
		}
		{
			const tNamedParamList<vector4>* il = this->GetAttribVector4List();
			for (tNamedParamList<vector4>::const_iterator i = il->begin(); i != il->end(); i++) {
				cmd = ps->GetCmd(this,'STAT');			
				cmd->In()->SetS(i->first.c_str());
				cmd->In()->SetF(i->second.param.x);
				cmd->In()->SetF(i->second.param.y);
				cmd->In()->SetF(i->second.param.z);
				cmd->In()->SetF(i->second.param.w);
				ps->PutCmd(cmd);
			}
		}

		nPixelShader* shader = this->GetPixelShader();
		if (shader) {
			cmd = ps->GetCmd(this,'STAP');			
			cmd->In()->SetB(shader->GetActiveProgram());
			ps->PutCmd(cmd);
		}

        return true;
    }
    return false;
}
