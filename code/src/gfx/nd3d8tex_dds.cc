#define N_IMPLEMENTS nD3D8Texture
//------------------------------------------------------------------------------
//  nd3d8tex_dds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nd3d8texture.h"
#include "gfx/ns3tctexture.h"
#include "gfx/nopenilfile.h"

extern const char *nd3d8_Error(HRESULT hr);

//------------------------------------------------------------------------------
/**
    Load a compressed texture into a d3d texture.

    @param  fileName    filename of texture file
    @return             true if everything ok, false if texture could not be
                        loaded (most probably because the file does not exist)
*/
bool
nD3D8Texture::loadFileIntoTextureDDS(const char* fileName)
{
    n_assert(0 == this->tex);
    n_assert(this->ref_gs->d3d8Dev);

    nOpenILFile oilFile(this->kernelServer);

    if (!oilFile.Open(fileName)) 
        return false;

	nS3TCTexture txt;

	long index = txt.GetFormat(oilFile.GetFile());
    if (index < 0 || index > S3TC5) {
		return false;
    }

    if (!this->ref_gs->s3tc_formats[index]) {
		return false;
    }
    
    if(oilFile.ReadData() == -1) return false;

	this->SetWidth(txt.width);
    this->SetHeight(txt.height);
	this->SetBytesPerPixel(txt.s3tc_desc.pfPixelFormat.RGBBitCount/8);	

    HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(
        this->ref_gs->d3d8Dev,
        oilFile.GetData(),
        oilFile.GetSize(),
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        this->gen_mipmaps ? D3DX_DEFAULT : 1,
        0,
        D3DFMT_UNKNOWN,
		D3DPOOL_DEFAULT,//D3DPOOL_MANAGED,
        D3DX_FILTER_NONE,
        D3DX_FILTER_BOX,
        0,
        NULL,
        NULL,
        &(this->tex));
    if (FAILED(hr))
    {
        n_printf("nD3D8Texture: D3DXCreateTextureFromFileExA() failed with '%s'\n", nd3d8_Error(hr));
        return false;
    }
#ifdef _DEBUG_DDS
    n_printf("nD3D8 Texture: DDS: loaded successfully'%s'\n", fileName);
#endif    
    return true;
}

//------------------------------------------------------------------------------
