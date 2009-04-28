#ifndef N_PIXELSHADERDESC_H
#define N_PIXELSHADERDESC_H
//-------------------------------------------------------------------
//  CLASS
//  nPSI
//  nTextureUnit
//  nPixelShaderDesc
//
//  OVERVIEW
//  Holds generic pixel shader description.
//-------------------------------------------------------------------
#include <stdio.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif
#ifndef N_COLOR_H
#include "mathlib/color.h"
#endif
#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

//-------------------------------------------------------------------
/**
    @class nPSI

    @brief A single pixel shader instruction.
*/
//-------------------------------------------------------------------
class nPSI 
{
public:
    //---------------------------------------------------------------
    //  nArg
    //  Pixel shader argument types, with optional modifier bits.
    //---------------------------------------------------------------
    enum nArg {
        NOARG       = 0,
        TEX         = 1,        // result of texture unit
        PREV        = 2,        // result of previous stage
        CONSTANT    = 3,        // constant defined for this stage
        PRIMARY     = 4,        // the fragments primary color

        ARGMASK     = (1<<4)-1,

        ONE_MINUS   = (1<<4),   // invert argument
        COLOR       = (1<<5),   // only use the color part
        ALPHA       = (1<<6),   // only use the alpha part
    };

    //---------------------------------------------------------------
    //  nOp
    //  Pixel shader operation types.
    //---------------------------------------------------------------
    enum nOp {
        NOP,
        REPLACE,            // arg0
        MUL,                // arg0 * arg1
        ADD,                // arg0 + arg1
        ADDS,               // (arg0 + arg1)-0.5
        IPOL,               // arg0*arg2 + arg1*(1-arg2)
        DOT,                // dot product of color components 
    };

    //---------------------------------------------------------------
    //  nScale
    //  Optional value by which the result will be scaled.
    //---------------------------------------------------------------
    enum nScale {
        ONE,
        TWO,
        FOUR,
    };


    enum {
        N_MAXARGS = 3,
    };

    nOp     op;                     // operation
    nArg    args[N_MAXARGS];        // arguments
    nScale  scale;                  // optional scale value;

    //---------------------------------------------------------------
    nPSI() 
        : op(NOP),
          scale(ONE)
    {
        args[0] = NOARG;
        args[1] = NOARG;
        args[2] = NOARG;
    }
    
    void SetOp(nOp o) {
        op = o;
    }
    nOp GetOp(void) {
        return op;
    }

    void SetArg(int aindex, nArg a) {
        n_assert((aindex>=0) && (aindex<N_MAXARGS));
        args[aindex] = a;
    }
    nArg GetArg(int aindex) {
        n_assert((aindex>=0) && (aindex<N_MAXARGS));
        return args[aindex];
    }

    void SetScale(nScale s) {
        scale = s;
    }
    nScale GetScale(void) {
        return scale;
    }
};

//-------------------------------------------------------------------
/**
    @class nTextureUnit

    Each pixel shader instruction has a virtual texture unit attached,
    which completely describes a texture read operation (the result
    of the texture read operation is the N_TEX argument in a
    pixel shader instruction.

    03-Oct-00   floh    + texture matrix scale was initialized to (0,0,0)
    16-Nov-00   floh    + removed texture object, now defined at
                          render time
	25-Mar-2003 Ilya	+ anisotropic textures filtering support added	
	14-Jun-2004 Ilya    + 3d textures support, by Ilya Kliot, Insightec Ltd			
*/
//-------------------------------------------------------------------
class nTextureUnit {
public:

    //---------------------------------------------------------------
    nRStateParam addr_u;        // texture address mode for u dimension
    nRStateParam addr_v;        // texture address mode for v dimension
	// + 14-Jun-2004 3d textures support, by Ilya Kliot, Insightec Ltd
	nRStateParam addr_w;        // texture address mode for v dimension

    nRStateParam filter_min;    // texture minification filter
    nRStateParam filter_mag;    // texture magnification filter
    nRStateParam coord_src;     // texture coordinate source
    matrix44 matrix;            // texture coordinate transformation matrix
    bool matrix_dirty;          // is matrix current?
    bool enable_transform;      // explicitely turn on/off coord transform
	bool enable_3d;				// explicitely turn on/off 3d texture mode


	// + 25-Mar-2003 anisotropic textures filtering support, by Ilya Kliot, Clockwise  Ltd
	bool anisotropic_filter;	// anisotropic textures filtering
    vector3 t;                  // texture matrix translation
    vector3 r;                  // texture matrix rotation (euler)
    vector3 s;                  // texture matrix scale

    nTextureUnit() 
        :   addr_u(N_TADDR_CLAMP),
            addr_v(N_TADDR_CLAMP),
			addr_w(N_TADDR_CLAMP),
            filter_min(N_TFILTER_LINEAR),
            filter_mag(N_TFILTER_LINEAR),
            coord_src(N_TCOORDSRC_UV0),
            enable_transform(false),
			enable_3d(false),
            matrix_dirty(false),
			anisotropic_filter(false),
            s(1.0f,1.0f,1.0f)
    { }

	// + 25-Mar-2003 anisotropic textures filtering support, by Ilya Kliot, Clockwise  Ltd
	void SetAnisotropicFilter(bool b) {
		anisotropic_filter = b;
	}

	bool GetAnisotropicFilter() {
		return anisotropic_filter;
	}
        
    void SetAddrU(nRStateParam p) {
        addr_u = p;
    }
    nRStateParam GetAddrU(void) {
        return addr_u;
    }

    void SetAddrV(nRStateParam p) {
        addr_v = p;
    }
    nRStateParam GetAddrV(void) {
        return addr_v;
    }
	// + 14-Jun-2004 3d textures support, by Ilya Kliot, Insightec Ltd
	void SetAddrW(nRStateParam p) {
        addr_w = p;
    }
    nRStateParam GetAddrW(void) {
        return addr_w;
    }

    void SetMinFilter(nRStateParam p) {
        filter_min = p;
    }
    nRStateParam GetMinFilter(void) {
        return filter_min;
    }
    void SetMagFilter(nRStateParam p) {
        filter_mag = p;
    }
    nRStateParam GetMagFilter(void) {
        return filter_mag;
    }

    void SetCoordSrc(nRStateParam p) {
        coord_src = p;
    }
    nRStateParam GetCoordSrc(void) {
        return coord_src;
    }

    //--- texture coordinate transformation ---
    void SetEnableTransform(bool b) {
        enable_transform = b;
    }
    bool GetEnableTransform(void) {
        return enable_transform;
    }
    // + 14-Jun-2004 3d textures support, by Ilya Kliot, Insightec Ltd	
	//--- texture coordinate transformation ---
    void SetEnable3D(bool b) {
        enable_3d = b;
    }
    bool GetEnable3D(void) {
        return enable_3d;
    }    
    void SetMatrix(const matrix44& m) {
        matrix = m;
        matrix_dirty = false;
    }
    const matrix44& GetMatrix(void) {
        if (matrix_dirty) {
            // need to recompute internal matrix
            matrix.ident();
            matrix.scale(s);
            matrix.rotate_x(r.x);
            matrix.rotate_y(r.y);
            matrix.rotate_z(r.z);
            matrix.translate(t);
            matrix_dirty = false;
        }
        return matrix;
    }
    void Txyz(const vector3& v) {
        t = v;
        matrix_dirty = true;
    }
    void Rxyz(const vector3& v) {
        r = v;
        matrix_dirty = true;
    }
    void Sxyz(const vector3& v) {
        s = v;
        matrix_dirty = true;
    }
    const vector3& GetT(void) {
        return t;
    }
    const vector3& GetR(void) {
        return r;
    }
    const vector3& GetS(void) {
        return s;
    }
};

//-------------------------------------------------------------------
/**
    @class nPixelShaderDesc

    @brief Holds generic pixel shader description.

    19-Oct-00   floh    oops, SetColorOp() and SetAlphaOp() didn't
                        set scale argument
	08-Jan-03   Ilya			+ Specular support added
	25-Mar-2003 Ilya			+ anisotropic textures filtering support added				 
	27-Oct-2006 Ilya			+ line and dot parameters added
	19-Dec-2007 Ilya & Rachel	+ stencil support added
	20-May-2008 Ilya			- line and dot parameters removed
*/
//-------------------------------------------------------------------
class nPixelShaderDesc {
public:
    enum {
        MAXSTAGES = N_MAXNUM_TEXSTAGES,
    };
    
    int num_stages;                     // the number of valid stages
    nPSI color_op[MAXSTAGES];           // the shader instructions for color ops
    nPSI alpha_op[MAXSTAGES];           // the shader instructions for alpha ops
    nTextureUnit tunit[MAXSTAGES];      // up to 8 virtual texture units

    // misc other render states
    bool light_enable;                  // true/false
    bool alpha_enable;                  // true/false
    bool zwrite_enable;                 // true/false	
    bool fog_enable;                    // true/false
    bool alphatest_enable;              // true/false
	bool wireframe_enable;				// true/false
	bool normalizenormals_enable;		// true/false
	// 19-Dec-2007 Ilya & Rachel stencil support added
	bool stenciltest_enable;	

    nRStateParam alpha_src_blend;       // N_ABLEND_*
    nRStateParam alpha_dest_blend;      // N_ABLEND_*
    nRStateParam zfunc;                 // N_CMP_*
    nRStateParam cull_mode;             // N_CULL_*
    nRStateParam color_material;        // N_CMAT_*
    nRStateParam alphafunc;             // N_CMP_*
	float		 alpharef;	

	// 20-Dec-2007 Ilya & Rachel stencil support added
    nRStateParam	stencil_func;		// N_CMP_*
	nRStateParam	stencil_fail;		// N_STENCILOP_
    nRStateParam	stencil_zfail;		// N_STENCILOP_
    nRStateParam	stencil_pass;		// N_STENCILOP_
    int				stencil_ref;
	uint			stencil_check_mask;
    uint			stencil_write_mask;    

    // predefined constants
    color4 consts[MAXSTAGES];          

    // lighting equation parameters
    color4 diffuse;
    color4 emissive;
    color4 ambient;

    // + 08-Jan-03   Ilya    Specular support added
    color4 specular;
    float   shininess; 
    bool    specular_enable;

    // redundant state management
    bool state_dirty;       // dirty flag if to-be-compiled states have changed
    bool state_changed;     // state needs to be applied
    bool const_changed;     // one of the const's have changed
    bool transform_changed; // one of the texture matrices has changed
    bool lighting_changed;  // one of the lighting parameters have changed

    nPixelShaderDesc()
        : state_dirty(false),
          state_changed(false),
          const_changed(false),
          transform_changed(false),
          lighting_changed(false),
          num_stages(0),
          light_enable(true),
          alpha_enable(false),
          zwrite_enable(true),		  
          fog_enable(false),
          alpha_src_blend(N_ABLEND_SRCALPHA),
          alpha_dest_blend(N_ABLEND_INVSRCALPHA),
          zfunc(N_CMP_LESSEQUAL),
          cull_mode(N_CULL_CCW),
          color_material(N_CMAT_MATERIAL),
          alpharef(0.0f),
          alphafunc(N_CMP_GREATER),
          alphatest_enable(false),
          stenciltest_enable(false),
		  stencil_func(N_CMP_ALWAYS),
		  stencil_fail(N_STENCILOP_KEEP),
		  stencil_zfail(N_STENCILOP_KEEP),
		  stencil_pass(N_STENCILOP_KEEP),
		  stencil_ref(0),
		  stencil_check_mask(0xffff),
		  stencil_write_mask(0xffff),		  
		  wireframe_enable(false),
          specular_enable(false),
          shininess(0.0),		  		  
		  normalizenormals_enable(false)		  
    { }

    //--- dirty and changed handling ---
    void SetStateDirty(bool b)          { state_dirty = b; }
    bool IsStateDirty(void)             { return state_dirty; }
    void SetStateChanged(bool b)        { state_changed = b;  }
    bool HasStateChanged(void)          { return state_changed; }
    void SetConstChanged(bool b)        { const_changed = b; }
    bool HasConstChanged(void)          { return const_changed; }
    void SetTransformChanged(bool b)    { transform_changed = b; }
    bool HasTransformChanged(void)      { return transform_changed; }
    void SetLightingChanged(bool b)     { lighting_changed = b; }
    bool HasLightingChanged(void)       { return lighting_changed; }
    void SetAllDirty(void) {
        SetStateDirty(true);
        SetStateChanged(true);
        SetConstChanged(true);
        SetTransformChanged(true);
        SetLightingChanged(true);
    }

    //--- copy contents from another pixel shader ---
    void CopyFrom(nPixelShaderDesc& other) {
        *this = other;
        this->SetAllDirty();
    }

    //--- number of valid stages ---
    void SetNumStages(int n) {
        n_assert((n>=0) && (n<=MAXSTAGES));
        num_stages = n;
    }
    int GetNumStages(void) {
        return num_stages;
    }

    //--- setting shader stage related render states ---
    void SetColorOp(int stage, nPSI::nOp op, nPSI::nArg arg0, nPSI::nArg arg1, nPSI::nArg arg2, nPSI::nScale s) {
        n_assert((stage>=0) && (stage<num_stages));
        color_op[stage].SetOp(op);
        color_op[stage].SetArg(0,arg0);
        color_op[stage].SetArg(1,arg1);
        color_op[stage].SetArg(2,arg2);
        color_op[stage].SetScale(s);
        state_dirty = true;
    }

    void SetAlphaOp(int stage, nPSI::nOp op, nPSI::nArg arg0, nPSI::nArg arg1, nPSI::nArg arg2, nPSI::nScale s) {
        n_assert((stage>=0) && (stage<num_stages));
        alpha_op[stage].SetOp(op);
        alpha_op[stage].SetArg(0,arg0);
        alpha_op[stage].SetArg(1,arg1);
        alpha_op[stage].SetArg(2,arg2);
        alpha_op[stage].SetScale(s);
        state_dirty = true;
    }

    void SetAddressU(int stage, nRStateParam ta) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetAddrU(ta);
        state_dirty = true;
    }
    void SetAddressV(int stage, nRStateParam ta) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetAddrV(ta);
        state_dirty = true;
    }
	// + 14-Jun-2004 3d textures support, by Ilya Kliot, Insightec Ltd
	void SetAddressW(int stage, nRStateParam ta) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetAddrW(ta);
        state_dirty = true;
    }
    void SetMinFilter(int stage, nRStateParam tf) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetMinFilter(tf);
        state_dirty = true;
    }
    void SetMagFilter(int stage, nRStateParam tf) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetMagFilter(tf);
        state_dirty = true;
    }
    void SetTexCoordSrc(int stage, nRStateParam tcs) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetCoordSrc(tcs);
        state_dirty = true;
    }
    void SetConst(int stage, const color4& v) {
        n_assert((stage>=0) && (stage<num_stages));
        consts[stage] = v;
        const_changed = true;
    }

    //--- getting shader stage related render states ---
    int GetColorOp(int stage, nPSI::nOp& op, nPSI::nArg& arg0, nPSI::nArg& arg1, nPSI::nArg& arg2, nPSI::nScale& s)
    {
        n_assert((stage>=0) && (stage<num_stages));
        op   = color_op[stage].GetOp();
        arg0 = color_op[stage].GetArg(0);
        arg1 = color_op[stage].GetArg(1);
        arg2 = color_op[stage].GetArg(2);
        s    = color_op[stage].GetScale();
        int num_args = 0;
        switch (op) {
            case nPSI::NOP:     num_args=0; break;
            case nPSI::REPLACE: num_args=1; break;
            case nPSI::MUL:     num_args=2; break;
            case nPSI::ADD:     num_args=2; break;
            case nPSI::ADDS:    num_args=2; break;
            case nPSI::IPOL:    num_args=3; break;
            case nPSI::DOT:     num_args=2; break;
        }
        return num_args;
    }
    int GetAlphaOp(int stage, nPSI::nOp& op, nPSI::nArg& arg0, nPSI::nArg& arg1, nPSI::nArg& arg2, nPSI::nScale& s)
    {
        n_assert((stage>=0) && (stage<num_stages));
        op   = alpha_op[stage].GetOp();
        arg0 = alpha_op[stage].GetArg(0);
        arg1 = alpha_op[stage].GetArg(1);
        arg2 = alpha_op[stage].GetArg(2);
        s    = alpha_op[stage].GetScale();
        int num_args = 0;
        switch (op) {
            case nPSI::NOP:     num_args=0; break;
            case nPSI::REPLACE: num_args=1; break;
            case nPSI::MUL:     num_args=2; break;
            case nPSI::ADD:     num_args=2; break;
            case nPSI::ADDS:    num_args=2; break;
            case nPSI::IPOL:    num_args=3; break;
            case nPSI::DOT:     num_args=2; break;
        }
        return num_args;
    }
    nRStateParam GetAddressU(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetAddrU();
    }
    nRStateParam GetAddressV(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetAddrV();
    }
	// + 14-Jun-2004 3d textures support, by Ilya Kliot, Insightec Ltd
	nRStateParam GetAddressW(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetAddrW();
    }
    nRStateParam GetMinFilter(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetMinFilter();
    }
    nRStateParam GetMagFilter(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetMagFilter();
    }
    nRStateParam GetTexCoordSrc(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetCoordSrc();
    }
    const color4& GetConst(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return consts[stage];
    }

    //--- texture matrix manipulation ---
    void SetEnableTransform(int stage, bool b) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetEnableTransform(b);
        transform_changed = true;
    }
	// + 25-Mar-2003 anisotropic textures filtering support, by Ilya Kliot, Clockwise  Ltd
	void SetAnisotropicFilter(int stage, bool b) {
		n_assert((stage>=0) && (stage<num_stages));
		tunit[stage].SetAnisotropicFilter(b);
		state_dirty = true;
	}
	// + 14-Jun-2004 3d textures support, by Ilya Kliot, Insightec Ltd
	void SetEnable3D(int stage, bool b) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetEnable3D(b);
        transform_changed = true;
    }
    void SetMatrix(int stage, const matrix44& m) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].SetMatrix(m);
        transform_changed = true;
    }
    void Txyz(int stage, const vector3& v) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].Txyz(v);
        transform_changed = true;
    }
    void Rxyz(int stage, const vector3& v) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].Rxyz(v);
        transform_changed = true;
    }
    void Sxyz(int stage, const vector3& v) {
        n_assert((stage>=0) && (stage<num_stages));
        tunit[stage].Sxyz(v);
        transform_changed = true;
    }
	// + 25-Mar-2003 anisotropic textures filtering support, by Ilya Kliot, Clockwise  Ltd
	bool GetAnisotropicFilter(int stage) {
		n_assert((stage>=0) && (stage<num_stages));
		return tunit[stage].GetAnisotropicFilter();
	}
    bool GetEnableTransform(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetEnableTransform();
    }
	// + 14-Jun-2004 3d textures support, by Ilya Kliot, Insightec Ltd
	bool GetEnable3D(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetEnable3D();
    }
    const matrix44& GetMatrix(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetMatrix();
    }
    const vector3& GetT(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetT();
    }
    const vector3& GetR(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetR();
    }
    const vector3& GetS(int stage) {
        n_assert((stage>=0) && (stage<num_stages));
        return tunit[stage].GetS();
    }

    //--- set stage independent render states ---
    void SetDiffuse(const color4& v) {
        diffuse = v;
        lighting_changed = true;
    }
    //+ 08-Jan-03   Ilya    Specular support added
    void SetSpecular(color4& v) {
        specular = v;
        if (v.x != 0.0f || v.y != 0.0f || v.z != 0.0f)
            specular_enable = true;
        lighting_changed = true;
    }

    void SetShininess(float se) 
	{
		n_assert(se < 128.0f + N_TINY);
        shininess = se;
    }
    
    void SetEmissive(const color4& v) {
        emissive = v;
        lighting_changed = true;
    }
    void SetAmbient(const color4& v) {
        ambient = v;
        lighting_changed = true;
    }
    void SetLightEnable(bool b) {
        light_enable = b;
        state_dirty = true;
    }
    void SetAlphaEnable(bool b) {
        alpha_enable = b;
        state_dirty = true;
    }
    void SetZWriteEnable(bool b) {
        zwrite_enable = b;
        state_dirty = true;
    }
    void SetFogEnable(bool b) {
        fog_enable = b;
        state_dirty = true;
    }
    void SetAlphaSrcBlend(nRStateParam p) {
        alpha_src_blend = p;
        state_dirty = true;
    }
    void SetAlphaDestBlend(nRStateParam p) {
        alpha_dest_blend = p;
        state_dirty = true;
    }
    void SetZFunc(nRStateParam p) {
        zfunc = p;
        state_dirty = true;
    }
    void SetCullMode(nRStateParam p) {
        cull_mode = p;
        state_dirty = true;
    }
    void SetColorMaterial(nRStateParam p) {
        color_material = p;
        state_dirty = true;
    }
    void SetAlphaTestEnable(bool b)
    {
        alphatest_enable = b;
        state_dirty = true;
    }
    void SetAlphaFunc(nRStateParam p)
    {
        alphafunc = p;
        state_dirty = true;
    }
    void SetAlphaRef(float r)
    {
        this->alpharef = r;
        state_dirty = true;
    }
	// 20-Dec-2007 Ilya & Rachel stencil support added
	void SetStencilTestEnable(bool b)
	{state_dirty = true;
		this->stenciltest_enable = b;
	}
	void SetStencilFunc(nRStateParam p)
	{state_dirty = true;
		this->stencil_func = p;
	}
	void SetStencilRef(uint r)
	{state_dirty = true;
		this->stencil_ref = r;
	}
	void SetStencilMask(uint m)
	{state_dirty = true;
		this->stencil_check_mask = m;
	}
	void SetStencilOp(nRStateParam sfail, nRStateParam zfail, nRStateParam pass)
	{
		this->stencil_fail = sfail;
		this->stencil_zfail = zfail;
		this->stencil_pass = pass;
	}
	void SetStencilWriteMask(uint r)
	{state_dirty = true;
		this->stencil_write_mask = r;
	}

	void SetWireframe(bool b)
	{
		wireframe_enable = b;
		state_dirty = true;
	}
	
	void SetNormalizeNormals(bool b)
	{
		normalizenormals_enable = b;
		state_dirty = true;
	}
	
    //--- get stage independent render states ---
    const color4& GetDiffuse(void) {
        return diffuse;
    }
    //+ 08-Jan-03   Ilya    Specular support added
    const color4& GetSpecular(void) {
        return specular;
    }
    float GetShininess(void) const {
        return shininess;
    }
    const color4& GetEmissive(void) const {
        return emissive;
    }
    const color4& GetAmbient(void) const {
        return ambient;
    }
    bool GetLightEnable(void) const {
        return light_enable;
    }
    bool GetAlphaEnable(void) const {
        return alpha_enable;
    }
    bool GetZWriteEnable(void) const {
        return zwrite_enable;
    }
    bool GetFogEnable(void) const {
        return fog_enable;
    }
    nRStateParam GetAlphaSrcBlend(void) const {
        return alpha_src_blend;
    }
    nRStateParam GetAlphaDestBlend(void) const {
        return alpha_dest_blend;
    }
    nRStateParam GetZFunc(void) const {
        return zfunc;
    }
    nRStateParam GetCullMode(void) const {
        return cull_mode;
    }
    nRStateParam GetColorMaterial(void) const {
        return color_material;
    }
    bool GetAlphaTestEnable() const
    {
        return this->alphatest_enable;
    }
    float GetAlphaRef() const
    {
        return this->alpharef;
    }
    nRStateParam GetAlphaFunc() const
    {
        return this->alphafunc;
    }
	// 20-Dec-2007 Ilya & Rachel stencil support added
	bool GetStencilTestEnable() const
	{
		return this->stenciltest_enable;
	}
	nRStateParam GetStencilFunc() const
	{
		return this->stencil_func;
	}
	uint GetStencilRef() const
	{
		return this->stencil_ref;
	}
	uint GetStencilMask() const
	{
		return this->stencil_check_mask;
	}
	void GetStencilOp(nRStateParam& sfail, nRStateParam& zfail, nRStateParam& pass) const
	{
		sfail = this->stencil_fail;
		zfail = this->stencil_zfail;
		pass = this->stencil_pass;
	}
	uint GetStencilWriteMask() const
	{
		return this->stencil_write_mask;
	}
	bool GetWireframe() const
	{
		return this->wireframe_enable;
	}

	bool GetNormalizeNormals() const
	{
		return normalizenormals_enable;	
	}		

    //---------------------------------------------------------------
    //  Str2Op()
    //  Translate an nPixelShader opcode string of the form
    //  xxx[.1,.2,.4] into enums.
    //  21-Aug-00   floh    created
    //  22-Jul-01   floh    + dot product
    //---------------------------------------------------------------
    int Str2Op(const char *str, nPSI::nOp& op, nPSI::nScale& s)
    {
        n_assert(str);

        op = nPSI::NOP;
        s  = nPSI::ONE;
        int num_args = 0;

        // check if the opcode contains a known instruction
        if      (strstr(str, "nop"))        { op = nPSI::NOP;       num_args = 0; }
        else if (strstr(str, "mul"))        { op = nPSI::MUL;       num_args = 2; }
        else if (strstr(str, "replace"))    { op = nPSI::REPLACE;   num_args = 1; }
        else if (strstr(str, "adds"))       { op = nPSI::ADDS;      num_args = 2; }
        else if (strstr(str, "add"))        { op = nPSI::ADD;       num_args = 2; }
        else if (strstr(str, "ipol"))       { op = nPSI::IPOL;      num_args = 3; }
        else if (strstr(str, "dot"))        { op = nPSI::DOT;       num_args = 2; }

        // check if the opcode contains a string '.1, .2, .4', if yes,
        // this will be used as the scaler
        if (op != nPSI::NOP) {
            if (strstr(str,".1"))        s=nPSI::ONE;
            else if (strstr(str,".2"))   s=nPSI::TWO;
            else if (strstr(str,".4"))   s=nPSI::FOUR;
        }
        return num_args;
    }

    //---------------------------------------------------------------
    //  Str2Arg()
    //  Translate a pixel shader argument string into an enum;
    //  21-Aug-00   floh    created
    //  25-Aug-00   floh    + support for arg modifiers
    //  04-Oct-00   floh    + added 'prim'
    //---------------------------------------------------------------
    bool Str2Arg(const char *str, nPSI::nArg& a)
    {
        n_assert(str);
        
        a = nPSI::NOARG;

        // check for the arg type strings
        if      (strstr(str,"tex"))   a=nPSI::TEX;
        else if (strstr(str,"prev"))  a=nPSI::PREV;
        else if (strstr(str,"const")) a=nPSI::CONSTANT;
        else if (strstr(str,"prim"))  a=nPSI::PRIMARY;
        else return false;

        // check for optional leading minus sign
        if ('-' == str[0]) a = (nPSI::nArg) (a | nPSI::ONE_MINUS);

        // check for postfix (.a, .c)
        if (strstr(str,".a"))       a = (nPSI::nArg) (a | nPSI::ALPHA);
        else if (strstr(str,".c"))  a = (nPSI::nArg) (a | nPSI::COLOR);

        return true;
    }

    //---------------------------------------------------------------
    //  Op2Str()
    //  Translate opcode enum to string.
    //  22-Aug-00   floh    created
    //  22-Jul-01   floh    + dot product
    //---------------------------------------------------------------
    bool Op2Str(nPSI::nOp op, nPSI::nScale scale, char *buf, int bufsize)
    {
        n_assert(buf);
        char local_buf[32];
        switch (op) {
            case nPSI::NOP:         sprintf(local_buf, "nop");      break;
            case nPSI::REPLACE:     sprintf(local_buf, "replace");  break;
            case nPSI::MUL:         sprintf(local_buf, "mul");      break;
            case nPSI::ADDS:        sprintf(local_buf, "adds");     break;
            case nPSI::ADD:         sprintf(local_buf, "add");      break;
            case nPSI::IPOL:        sprintf(local_buf, "ipol");     break;
            case nPSI::DOT:         sprintf(local_buf, "dot");      break;
        }
        switch (scale) {
            case nPSI::ONE:     break;
            case nPSI::TWO:     strcat(local_buf,".2"); break;
            case nPSI::FOUR:    strcat(local_buf,".4"); break;
        }
        n_strncpy2(buf,local_buf,bufsize);
        return true;
    }

    //---------------------------------------------------------------
    //  Arg2Str()
    //  Translate arg enum to string.
    //  22-Aug-00   floh    created
    //  25-Aug-00   floh    + argument modifiers
    //  04-Oct-00   floh    + 'prim'
    //---------------------------------------------------------------
    bool Arg2Str(nPSI::nArg arg, char *buf, int bufsize)
    {
        n_assert(buf);
        char local_buf[32];
        local_buf[0] = 0;

        if (arg & nPSI::ONE_MINUS) strcat(local_buf,"-");

        switch (arg & nPSI::ARGMASK) {
            case nPSI::NOARG:     strcat(local_buf,"noarg");  break;
            case nPSI::TEX:       strcat(local_buf,"tex");    break;
            case nPSI::PREV:      strcat(local_buf,"prev");   break;
            case nPSI::CONSTANT:  strcat(local_buf,"const");  break;
            case nPSI::PRIMARY:   strcat(local_buf,"prim");   break;
        }
        
        if (arg & nPSI::COLOR)        strcat(local_buf,".c");
        else if (arg & nPSI::ALPHA)   strcat(local_buf,".a");

        n_strncpy2(buf,local_buf,bufsize);
        return true;
    }
};

//-------------------------------------------------------------------
#endif
