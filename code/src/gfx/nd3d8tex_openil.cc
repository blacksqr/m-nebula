#define N_IMPLEMENTS nD3D8Texture
//------------------------------------------------------------------------------
/**
    nd3d8tex_openil.cc
    
    Texture load function using OpenIL library functions.

    (C) A.Weissflog 2001
*/

#include "gfx/nd3d8texture.h"
#include "gfx/nopenilfile.h"

#ifdef _UNICODE
#undef _UNICODE
#endif

#include "il/il.h"
#include "il/ilu.h"

extern const char *nd3d8_Error(HRESULT hr);

static unsigned int 
nextPOW2(unsigned int val) {
    unsigned int i;
    for (i = 1; i < (1<<30); i <<= 1) if (val <= i) return i;        
    return i;
}

#undef NONSQUARE_MIPMAP_WORKAROUND

//------------------------------------------------------------------------------
/**
    Load an image into a d3d8 texture using DevIL image loading and
    mipmap generation functions.

    @param  fileName    filename of the texture file
    @return             true if texture successfully loaded

    24-May-01   leaf    fixed bad logic when no conversion is needed
    24-May-01   leaf    fixed bug when no mipmaps requested
    28-Jul-01   leaf    Updated to DevIL
    15-Aug-01   leaf    fixed mipmap numlevels bug (thanks to Aaron Cram)
    11-Aug-01   floh    workaround for the non-square Mipmap Generation Bug
                        in DevIL (non-square mipmaps are simply not mipmapped,
                        see the define at the top of this file!)
    17-Aug-01   floh    Support for highquality texture loading flag
  + 05-Nov-2002 Ilya	compressed textures support
*/
bool
nD3D8Texture::loadFileIntoTextureIL(const char* fileName)
{
    n_assert(0 == this->tex);
    n_assert(this->ref_gs->d3d8Dev);

    ILenum il_type = GetILType(fileName); 	

	if (il_type == IL_DDS && this->ref_gs->texture_compression)
		if (this->loadFileIntoTextureDDS(fileName))
			return true;
#ifdef _DEBUG
		else
			n_printf("failed to load compresssed texture %s loading in usual way\n", fileName);
#endif		

    nOpenILFile oilFile(this->kernelServer);

    oilFile.Open(fileName);
    if (oilFile.ReadData() < 0)
        return false;

    // load the image
    ILuint imgId;
    ilGenImages(1, &imgId);
    ilBindImage(imgId);
    if (!ilLoadL(il_type, oilFile.GetData(), oilFile.GetSize())) {
        n_printf("nD3D8Texture: Failed loading image file '%s' as texture!\n", fileName);
        n_printf("OpenIL error: '%s'\n", iluErrorString(ilGetError()));
        ilDeleteImages(1, &imgId);    
		return false;
    }

   	if (il_type == IL_DDS) {
        iluFlipImage();
    }
    
	if (!this->createTextureIL(fileName, imgId)) 
	{
		// gracefully exit on failure
        if (this->tex) 
		{
            this->tex->Release();
            this->tex = 0;
		}
        ilDeleteImages(1, &imgId);
        return false;
    }

    // success
    ilDeleteImages(1, &imgId);
    return true;

}

//------------------------------------------------------------------------------
/**
    Load an image from memory into a d3d8 texture using DevIL image loading and
    mipmap generation functions.

    @param  p - pointer to memory chunk contains texture 
            size - size of those chunk
    @return             true if texture successfully loaded

    +27-jul-2002   --Ilya Kliot 
*/
bool
nD3D8Texture::loadFromMem(void* p, unsigned long size, long maxw, long maxh)
{
    n_assert(0 == this->tex);
    n_assert(this->ref_gs->d3d8Dev);

    // load the image
    ILuint imgId;
    ilGenImages(1, &imgId);
    ilBindImage(imgId);
    if (!ilLoadL(IL_BMP, p, size)) {
        n_printf("nGlTexture: Failed loading image from memory as texture!\n");
        n_printf("OpenIL error: '%s'\n", iluErrorString(ilGetError()));
        ilDeleteImages(1, &imgId);
        return false;
    }

	if (!this->createTextureIL("memory", imgId, maxw, maxh)) {
		// gracefully exit on failure
        if (this->tex) {
            this->tex->Release();
            this->tex = 0;
        }
        ilDeleteImages(1, &imgId);
        return false;
    }    

    // success
    ilDeleteImages(1, &imgId);    
    return true;
}

//------------------------------------------------------------------------------
/**
    Create an d3d8 texture using DevIL image manipulation and
    mipmap generation functions.

    @param  name of the texture 
            imgid id of texture in devil
    @return             true if texture successfully loaded

    +02-feb-2003   --Ilya Kliot 
*/
bool nD3D8Texture::createTextureIL(const char* name, int imgId, long maxw, long maxh) {
    // do image format conversion
	D3DFORMAT srcD3DFormat;
    D3DFORMAT dstD3DFormat;
    int srcBytesPerPixel;
    int dstBytesPerPixel;
	HRESULT hr;   
    

    ilBindImage(imgId);

    ILboolean convResult = 1;
    ILint imageFormat = ilGetInteger(IL_IMAGE_FORMAT);
    switch (imageFormat)
    {
        case IL_COLOR_INDEX:            
        case IL_RGB:
		case IL_LUMINANCE:
            // swizzle rgb to bgr format
            convResult = ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
		case IL_BGR:
            srcD3DFormat     = D3DFMT_R8G8B8;
            srcBytesPerPixel = 3;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_R8G8B8;
                dstBytesPerPixel = 3;
                n_printf("*** NOTE: loading high quality '%s'\n", name);
            }
            else
            {
                dstD3DFormat     = D3DFMT_R5G6B5;
                dstBytesPerPixel = 2;
            }
            break;

		case IL_LUMINANCE_ALPHA:
        case IL_RGBA:
            // swizzle rgba to bgra format
            convResult = ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
		case IL_BGRA:
            srcD3DFormat     = D3DFMT_A8R8G8B8;
            srcBytesPerPixel = 4;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_A8R8G8B8;
                dstBytesPerPixel = 4;
                n_printf("*** NOTE: loading high quality '%s'\n", name);
            }
            else
            {
                dstD3DFormat     = D3DFMT_A4R4G4B4;
                dstBytesPerPixel = 2;
            }
            break;
        default:
            // all other formats: convert to BGR
            convResult = ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
            srcD3DFormat     = D3DFMT_R8G8B8;
            srcBytesPerPixel = 3;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_R8G8B8;
                dstBytesPerPixel = 3;
                n_printf("*** NOTE: loading high quality '%s'\n", name);
            }
            else
            {
                dstD3DFormat     = D3DFMT_R5G6B5;
                dstBytesPerPixel = 2;
            }
            break;
    }   

    if (!convResult)
    {
        n_printf("nD3D8Texture: ilConvertImage() failed on file '%s'!\n", name);
        return false;
    }

    // generate mipmaps if necessary    
    int imgWidth  = ilGetInteger(IL_IMAGE_WIDTH);
    int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);

    // force texture dimensions to power of 2
    int newW = nextPOW2(imgWidth);
    int newH = nextPOW2(imgHeight);    

	if (maxw > 0 && newW > maxw) newW = maxw;
	if (maxh > 0 && newH > maxh) newH = maxh;    
    
    if ((imgWidth != newW) || (imgHeight != newH)) {
        if (!iluScale(newW, newH, 1)) {
            n_printf("nGlTexture: iluScale() failed with '%s'\n", iluErrorString(ilGetError()));
            return false;
        }
        imgWidth = newW;
        imgHeight = newH;
    }


    int numMipLevels = 1;
    int numLevels    = 1;
    if (this->gen_mipmaps) { 
            iluBuildMipmaps();
            numMipLevels = D3DX_DEFAULT;
            numLevels    = ilGetInteger(IL_NUM_MIPMAPS) + 1;
    }

    // get relevant image data
	ILint imgFormat = ilGetInteger(IL_IMAGE_FORMAT);
    this->SetWidth(imgWidth);
    this->SetHeight(imgHeight);
    this->SetBytesPerPixel(dstBytesPerPixel);
    this->SetNumMipLevels(numLevels);

    // create a d3d8 texture
    hr = D3DXCreateTexture(
        this->ref_gs->d3d8Dev,      // pDevice
        imgWidth,                   // Width
        imgHeight,                  // Height
        numMipLevels,               // MipLevels
        0,                          // Usage (not as render target)
        dstD3DFormat,               // Format
        D3DPOOL_MANAGED,            // d3d manages the textures for us
        &(this->tex));              // ppTexture
    if (FAILED(hr))
    {
        n_printf("nD3D8Texture: D3DXCreateTexture() failed with '%s'\n", nd3d8_Error(hr));
        return false;
    }

    // load each mipmap level into the texture    
    for (int curLevel = 0; curLevel < numLevels; curLevel++) 
	{
        // need to rebind original image before selecting 
        // mipmap level (is this a design bug in OpenIL?)
        ilBindImage(imgId);
        if (!ilActiveMipmap(curLevel)) {
            n_printf("nD3D8Texture: ilActiveMipmap() failed with '%s'\n", iluErrorString(ilGetError()));
            return false;
        }

        // get the current surface level of the d3d8 texture object
        IDirect3DSurface8* mipSurface;
        hr = this->tex->GetSurfaceLevel(curLevel, &mipSurface);
        if (FAILED(hr))
        {
            n_printf("nD3D8Texture: GetSurfaceLevel() failed with '%s'\n", nd3d8_Error(hr));
            return false;
        }

        // get image data of current mipmap level
        int curWidth        = ilGetInteger(IL_IMAGE_WIDTH);
        int curHeight       = ilGetInteger(IL_IMAGE_HEIGHT);
        int bytesPerRow     = srcBytesPerPixel * curWidth;
        const void* curData = (const void*) ilGetData();

        // fill a RECT structure with width and height
        RECT srcRect;
        srcRect.left   = 0;
        srcRect.top    = 0;
        srcRect.right  = curWidth;
        srcRect.bottom = curHeight;

        // load mipmap image data into surface
        hr = D3DXLoadSurfaceFromMemory(
            mipSurface,         // pDestSurface
            NULL,               // pDestPalette (none)
            NULL,               // pDestRest (entire surface)
            curData,            // pSrcMemory
            srcD3DFormat,       // SrcFormat
            bytesPerRow,        // SrcPitch
            NULL,               // pSrcPalette
            &srcRect,           // pSrcRect
            D3DX_FILTER_NONE,   // Filter (since no scaling takes place, no filter needed)
            0);                 // ColorKey (none)
        if (FAILED(hr))
        {
            n_printf("nD3D8Texture: D3DXLoadSurfaceFromMemory() failed with '%s'\n", nd3d8_Error(hr));
            return false;
        }

        // release the mipSurface (as required by GetSurfaceLevel())
        mipSurface->Release();
        mipSurface = 0;
    }
    return true;
}
//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
