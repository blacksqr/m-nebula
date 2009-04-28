#ifndef N_TEXARRAYNODE_H
#define N_TEXARRAYNODE_H
//--------------------------------------------------------------------
/**
    @class nTexArrayNode
    @ingroup NebulaVisnodeModule
    
    @brief Hold all textures for nShaderNodes.
*/
//--------------------------------------------------------------------

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_TEXTUREARRAY_H
#include "gfx/ntexturearray.h"
#endif

#ifndef N_RSRCPATH_H
#include "misc/nrsrcpath.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nTexArrayNode
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class N_PUBLIC nTexArrayNode : public nVisNode 
{
    nAutoRef<nFileServer2> refFileServer;
    nTextureArray tex_array;
    nRsrcPath tex_pixel_desc[N_MAXNUM_TEXSTAGES];
    nRsrcPath tex_alpha_desc[N_MAXNUM_TEXSTAGES];
    bool tex_valid[N_MAXNUM_TEXSTAGES];
    bool tex_dirty[N_MAXNUM_TEXSTAGES];
    bool gen_mipmaps[N_MAXNUM_TEXSTAGES];
    bool highQuality[N_MAXNUM_TEXSTAGES];

    void*           tex_mem_ptr[N_MAXNUM_TEXSTAGES];
    unsigned long   tex_mem_size[N_MAXNUM_TEXSTAGES];
    long   max_w[N_MAXNUM_TEXSTAGES];
    long   max_h[N_MAXNUM_TEXSTAGES];

public:
    static nKernelServer *kernelServer;

    nTexArrayNode();

    virtual ~nTexArrayNode();

    // inherited
    virtual bool SaveCmds(nPersistServer *);
    virtual bool Attach(nSceneGraph2 *);
    virtual void Compute(nSceneGraph2 *);
    virtual void Preload();

    //---------------------------------------------------------------
    //  SetTexture()
    //  Define pixel and alpha channel files for a texture stage.
    //  A NULL alpha name pointer indicates that the texture
    //  has no alpha channel, a NULL pixel name indicates that 
    //  the texture stage should be cleared.
    //  16-Nov-00   floh    created
    //---------------------------------------------------------------
    virtual void SetTexture(int stage, const char *pname, const char *aname);


    //---------------------------------------------------------------
    //  SetTexture()
    //  loading textures from memory
    //  stage - number of stage
    //  ptr - pointer to  texture
    //  size - size of memory chunk occupied by the texture
    //
    //  28-jul-00   ilya    created
    //---------------------------------------------------------------
    virtual void SetTexture(int stage, void* ptr, unsigned long size, 
        long maxw = 256, long maxh = 256);

    //---------------------------------------------------------------
    /**
        @brief Return texture filenames at given stage.
        @stage [in]  Texture stage.
        @pname [out] Pixel filename.
        @aname [out] Alpha channel filename.
        
        16-Nov-00   floh    created
    */
    virtual void GetTexture(int stage, const char *& pname, const char *& aname, bool AbsolutPath=false){
        n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
		if ( AbsolutPath ) {
			pname = this->tex_pixel_desc[stage].GetAbsPath();
			aname = this->tex_alpha_desc[stage].GetAbsPath();
        }	else {       
        pname = this->tex_pixel_desc[stage].GetPath();
        aname = this->tex_alpha_desc[stage].GetPath();
		}        
    };

    //---------------------------------------------------------------
    /**  
        @brief Turn automatic mipmap generation on/off.
        @param stage [in] Texture stage.
        @param b     [in] On/off.
    */
    void SetGenMipMaps(int stage, bool b) {
        n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
        this->gen_mipmaps[stage] = b;
    };
    
    //---------------------------------------------------------------
    /**  
        @brief Check if automatic mipmap generation is on.
        @param stage [in] Texture stage.
        @return Whether mipmap generation is enabled.
    */
    bool GetGenMipMaps(int stage) {
        n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
        return this->gen_mipmaps[stage];
    };

    /// set high quality flag
    void SetHighQuality(int stage, bool b);
    /// get high quality flag
    bool GetHighQuality(int stage);

private:
    /// check if textures are valid, reload them if not
    void checkLoadTextures();
};

//------------------------------------------------------------------------------
/**
    @brief Set the high quality flag for a given texture stage.
    
    Gfx servers may load textures at a reduced quality to improve performance. 
    This flag tells them that this specific texture should be loaded at high 
    quality, even if this could mean a slight performance hit.
*/
inline
void
nTexArrayNode::SetHighQuality(int stage, bool b)
{
    n_assert((stage >= 0) && (stage < N_MAXNUM_TEXSTAGES));
    this->highQuality[stage] = b;
}

//------------------------------------------------------------------------------
/**
    @brief Get high quality flag for a texture stage.
*/
inline
bool
nTexArrayNode::GetHighQuality(int stage)
{
    n_assert((stage >= 0) && (stage < N_MAXNUM_TEXSTAGES));
    return this->highQuality[stage];
}

//------------------------------------------------------------------------------
#endif
