#define N_IMPLEMENTS nGlTexture
//------------------------------------------------------------------------------
/**
    ngltex_openil.cc
    
    Texture load function using OpenIL library functions.

    (C) A.Weissflog 2001
*/
#include "gfx/ngltexture.h"
#include "gfx/ns3tcgltexture.h"
#include "gfx/nopenilfile.h"

#ifdef _UNICODE
#undef _UNICODE
#endif

#include "il/il.h"
#include "il/ilu.h"

static unsigned int 
nextPOW2(unsigned int val) {
    unsigned int i;
    for (i = 1; i < (1<<30); i <<= 1) if (val <= i) return i;        
    return i;
}

bool 
nGlTexture::loadFileIntoTextureDDS(const char* fileName){
    nOpenILFile oilFile(this->kernelServer);

    oilFile.Open(fileName);    

	nS3TCGlTexture texture;
	if (!texture.Load(oilFile.GetFile()))
		return false;

	long width = texture.width;
	long height = texture.height;
	long size;
	long offset = 0;

	this->SetWidth(width);
    this->SetHeight(height);
	this->SetBytesPerPixel(texture.bufsize/(width*height));	

	glGenTextures(1, &this->tex_id);
    glBindTexture(GL_TEXTURE_2D, this->tex_id);	
	/* load the mipmaps */ 
	for (long i = 0; i < texture.numMipmaps && (width || height); ++i) { 
		if (width == 0) 
			width = 1; 
		if (height == 0) 
			height = 1; 

		size = ((width + 3) / 4) * ((height + 3) / 4) * texture.blockSize; 
		
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 
			i, 
			texture.GLformat, 
			width, 
			height, 
			0, 
			size, 
			texture.pixels + offset); 		
		offset += size; 
		width >>= 1; 
		height >>= 1;
        if (glGetError() != GL_NO_ERROR) {
            return false;
		}
	}

#ifdef _DEBUG_DDS
    n_printf("nGL Texture: DDS: loaded successfully'%s'\n", fileName);
#endif    
	return true;
}

//------------------------------------------------------------------------------
/**
    Load an image file into an OpenGL texture. Allocate a new OpenGL texture
    and write its name to this->tex_id.

    @param  fileName    filename of the texture file
    @return             true if texture successfully loaded and bound
	+ 05-Nov-2002 Ilya	compressed textures support
*/
bool
nGlTexture::loadFileIntoTextureIL(const char* fileName)
{
    n_assert(UINT_MAX == this->tex_id);

    // load the image
    ILenum il_type = GetILType(fileName);

	if (il_type == IL_DDS && (glCompressedTexImage2DARB != 0)) {
		if (this->loadFileIntoTextureDDS( fileName ))
			return true;
#ifdef _DEBUG
		else
			n_printf("failed to load compresssed texture %s loading in usual way\n", fileName);
#endif		
	}

    nOpenILFile oilFile(this->kernelServer);

    oilFile.Open(fileName);
    if (oilFile.ReadData() < 0)
        return false;

    ILuint imgId;
    ilGenImages(1, &imgId);
    ilBindImage(imgId);

    if (!ilLoadL(il_type, oilFile.GetData(), oilFile.GetSize())) {
        n_printf("nGlTexture: Failed loading image file '%s' as texture!\n", fileName);
        n_printf("OpenIL error: '%s'\n", iluErrorString(ilGetError()));
        ilDeleteImages(1, &imgId);
        return false;
    }

    if (il_type == IL_DDS) {
        iluFlipImage();
    }
    
    if (!this->createTextureIL(fileName, imgId)) {
        ilDeleteImages(1, &imgId);
        return false;
    }

    // gracefully exit on failure
    ilDeleteImages(1, &imgId);
    return true;
}


//------------------------------------------------------------------------------
/**
    Load an image from memory into a opengl texture using DevIL image loading and
    mipmap generation functions.

    @param  p - pointer to memory chunk contains texture 
            size - size of those chunk
    @return             true if texture successfully loaded

    +27-jul-2002   --Ilya Kliot 
*/
bool
nGlTexture::loadFromMem(void* p, unsigned long size, long maxw, long maxh) {
    n_assert(UINT_MAX == this->tex_id);

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
        ilDeleteImages(1, &imgId);
        return false;
    }
    
    // gracefully exit on failure
    ilDeleteImages(1, &imgId);    
    return true;
}

//------------------------------------------------------------------------------
/**
    Create an OpenGL texture using DevIL image manipulation and
    mipmap generation functions.

    @param  name of the texture 
            imgid id of texture in devil
    @return             true if texture successfully loaded

    +02-feb-2003   --Ilya Kliot 
*/
bool nGlTexture::createTextureIL(const char* name, int imgId, long maxw, long maxh) 
{
	ilBindImage(imgId);
    
    // if this is a paletted image, convert it to RGB format
    if (ilGetInteger(IL_IMAGE_FORMAT) == IL_COLOR_INDEX) 
    {
        if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE)) 
        {
            n_printf("nGlTexture: %s ilConvertImage() failed with '%s'\n", name, iluErrorString(ilGetError()));
            return false;
        }
    }

    int imgWidth  = ilGetInteger(IL_IMAGE_WIDTH);
    int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);

    // force texture dimensions to power of 2
    int newW = nextPOW2(imgWidth);
    int newH = nextPOW2(imgHeight);    
    
    
	if (maxw > 0 && newW > maxw) newW = maxw;
	if (maxh > 0 && newH > maxh) newH = maxh;    
	iluImageParameter(ILU_FILTER, ILU_BILINEAR);
    if ((imgWidth != newW) || (imgHeight != newH)) 
    {
        if (!iluScale(newW, newH, 1)) 
        {
            n_printf("nGlTexture: %s iluScale() failed with '%s'\n", name, iluErrorString(ilGetError()));
            return false;
        }
        imgWidth = newW;
        imgHeight = newH;
    }

    // generate mipmaps if necessary
    if (this->gen_mipmaps) 
    {
        if (!iluBuildMipmaps()) 
        {
            n_printf("nGlTexture: %s iluBuildMipmaps() failed with '%s'\n", name, iluErrorString(ilGetError()));
            return false;
        }
    }

    // bind the images to OpenGL
    glGenTextures(1, &this->tex_id);
    glBindTexture(GL_TEXTURE_2D, this->tex_id);

    // for each mipmap level...
    ILint  numMipmaps = ilGetInteger(IL_NUM_MIPMAPS);
    GLint  components = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    GLenum type       = (GLenum) ilGetInteger(IL_IMAGE_TYPE);
    GLenum format     = (GLenum) ilGetInteger(IL_IMAGE_FORMAT);
    
    this->SetWidth(imgWidth);
    this->SetHeight(imgHeight);
    this->SetBytesPerPixel(components);
    this->SetNumMipLevels(numMipmaps + 1);   
    
    for (GLint curLevel = 0; curLevel <= numMipmaps; curLevel++) 
    {
        // need to rebind original image before selecting 
        // mipmap level (is this a design bug in OpenIL?)
        ilBindImage(imgId);
        if (!ilActiveMipmap(curLevel)) 
        {
            n_printf("nGlTexture: %s ilActiveMipmap() failed with '%s'\n", name, iluErrorString(ilGetError()));
            return false;
        }

        glTexImage2D(GL_TEXTURE_2D,							// target
                     curLevel,								// mipmap level-of-detail
                     components,							// components (internal format)
                     ilGetInteger(IL_IMAGE_WIDTH),          // width
                     ilGetInteger(IL_IMAGE_HEIGHT),         // height
                     0,										// border
                     format,								// format (GL_RGB, GL_RGBA...)
                     type,									// type (GL_UNSIGNED_unsigned char...)
                     (GLvoid*) ilGetData());				// pointer to image data
    }
	ilActiveMipmap(0);
    return true;
}
