#define N_IMPLEMENTS nGlTexture
//------------------------------------------------------------------------------
//  ngltex_ntx.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/ngltexture.h"
#include "gfx/nopenilfile.h"

#ifdef _UNICODE
#undef _UNICODE
#endif

#ifndef NO_STLPORT
#include "stlport/string"
#include "stlport/vector"
#else 
#include <string>
#include <vector>
#endif

#include "il/il.h"
#include "il/ilu.h"
#include "util/nstl.h"

static unsigned int 
nextPOW2(unsigned int val) 
{
    unsigned int i;
    for (i = 1; i < (1<<30); i <<= 1) if (val <= i) return i;        
    return i;
}


static bool isgray(unsigned char* img, long w, long h, long bps) 
{
	for (long i = 0; i < w*h; i++) 
	{
		if (img[0] != img[1] || img[0] != img[2] || img[1] != img[2])
			return false;
		img += bps;
	}
	return true;
}

#define VOL_HEADER_SIZE (sizeof(long)*5)

bool nGlTexture::loadVolFileIntoTexture3D(const char* fileName) 
{
	nFileServer2* fs = this->kernelServer->GetFileServer2();

	bool enable3d = (strstr(fileName, ".img") == 0);

	if (glTexImage3DEXT || !enable3d)
	{	
		if (fs->FileExists(fileName)) 
		{
			nFile* fvol = fs->NewFileObject();
			fvol->Open(fileName, "rb");
			if (fvol->IsOpen()) 
			{				
				int fsize = fvol->Size();
				if (fsize > VOL_HEADER_SIZE )
				{				
					// bind the images to OpenGL				
					int width = 0;
					int height = 0;
					int depth = 0;
					int bpc = 0;
					int components = 0;

					fvol->GetInt(width);
					fvol->GetInt(height);
					fvol->GetInt(depth);
					fvol->GetInt(bpc);
					fvol->GetInt(components);

					GLenum format;			
					switch (components) 
					{
					case 1:
						format = GL_LUMINANCE;
						break;
					case 2:
						format = GL_LUMINANCE_ALPHA;				
						break;
					case 3:
						format = GL_RGB;
						break;
					case 4:
						format = GL_RGBA;
						break;
					default:
						n_assert(0 && "Undefined format");
						return false;
						break;
					}

					GLenum type = GL_UNSIGNED_BYTE;
					switch (bpc)
					{
					case 1:
						type = GL_UNSIGNED_BYTE;
						break;
					case 2:
						type = GL_UNSIGNED_SHORT;
						break;	
					default:
						n_assert(0 && "Undefined type");
						break;
					}

					GLenum internal_format = GL_LUMINANCE;

					switch (format) 
					{
					case GL_LUMINANCE:
						switch(type) 
						{
						case GL_UNSIGNED_BYTE:
							internal_format = GL_LUMINANCE;
							break;
						case GL_UNSIGNED_SHORT:
							internal_format = GL_LUMINANCE16;
							break;
						default:
							n_assert(0 && "Undefined internal format");
							break;
						}				
						break;
					case GL_LUMINANCE_ALPHA:
						switch(type) 
						{
						case GL_UNSIGNED_BYTE:
							internal_format = GL_LUMINANCE_ALPHA;
							break;
						case GL_UNSIGNED_SHORT:
							internal_format = GL_LUMINANCE16_ALPHA16;
							break;
						default:
							n_assert(0 && "Undefined internal format");
							break;
						}				
						break;
					case GL_RGB:
						switch(type) 
						{
						case GL_UNSIGNED_BYTE:
							internal_format = GL_RGB;
							break;
						case GL_UNSIGNED_SHORT:
							internal_format = GL_RGB16;
							break;
						default:
							n_assert(0 && "Undefined internal format");
							break;
						}
						break;
					case GL_RGBA:
						switch(type) 
						{
						case GL_UNSIGNED_BYTE:
							internal_format = GL_RGBA;
							break;
						case GL_UNSIGNED_SHORT:
							internal_format = GL_RGBA16;
							break;
						default:
							n_assert(0 && "Undefined internal format");
							break;
						}
						break;
					}			
					
					nGlServer* gs = (this->ref_gs.isvalid() ? (nGlServer*)this->ref_gs.get() : 0);

					if (!gs->non_power_of2) 
					{
						if ((unsigned int)width != nextPOW2(width) || 
							(unsigned int)height != nextPOW2(height) || 
							(unsigned int)depth != nextPOW2(depth)) 
						{
							n_printf("The non power of 2 textures are unsupported by hardware\n");
						}
					}				
					

					glGenTextures(1, &this->tex_id);
					GLint test_width = 1, test_height = 1, test_depth = 1;

					int new_width = nextPOW2(width);
					int new_height = nextPOW2(height);

					if (enable3d)
					{					
						glBindTexture(GL_TEXTURE_3D_EXT, this->tex_id);
						glTexImage3DEXT(GL_PROXY_TEXTURE_3D, 0, internal_format, new_width, new_height, depth, 0, format, type, 0);
						
						glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &test_width);
						glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &test_height);
						glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &test_depth);
					}
					else
					{
						glBindTexture(GL_TEXTURE_2D, this->tex_id);
						glTexImage2D(GL_PROXY_TEXTURE_2D, 0, internal_format, new_width, new_height, 0, format, type, 0);

						glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &test_width);
						glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &test_height);						
					}
					if (test_width == 0 || test_height == 0 || test_depth == 0) 
					{
						n_printf("Invalid dimensions of texture %dx%dx%d\n", new_width, new_height, depth);
						this->Unload();
					} 
					else 
					{
						if (enable3d)
							glTexImage3DEXT(GL_TEXTURE_3D_EXT,	0, internal_format, new_width, new_height, depth, 0, format, type, 0);
						else
							glTexImage2D(GL_TEXTURE_2D,	0, internal_format, new_width, new_height, 0, format, type, 0);

						size_t	pixnum = width*height;
						size_t	img_sz = pixnum*bpc*components;

						byte_buffer texture;
						texture.resize(img_sz);
						if (enable3d)
						{		
							this->SetWidth(new_width);
							this->SetHeight(new_height);
							this->SetDepth(depth);							

							for (int i = 0; i < depth; i++) 
							{
								fvol->Read(texture.get_buffer(), img_sz);
								ILuint imgId = iluGenImage();
								ilBindImage(imgId);
								ilConvertImage(format, type);
								iluScale(width, height,1);								
								ilSetData(texture.get_buffer());
								iluImageParameter(ILU_FILTER, ILU_BILINEAR);
								iluScale(new_width, new_height, 1);
								glTexSubImage3DEXT(GL_TEXTURE_3D_EXT,	0, 0, 0, i, new_width, new_height, 1, format, type, ilGetData());
								ilDeleteImages(1, &imgId);
							}
						}
						else
						{
							this->SetWidth(new_width);
							this->SetHeight(new_height);

							fvol->Read(texture.get_buffer(), img_sz);
							ILuint imgId = iluGenImage();
							ilBindImage(imgId);
							ilConvertImage(format, type);							
							iluScale(width, height,1);
							ilSetData(texture.get_buffer());
							iluImageParameter(ILU_FILTER, ILU_BILINEAR);
							iluScale(new_width, new_height,1);
							glTexSubImage2D(GL_TEXTURE_2D,	0, 0, 0, new_width, new_height, format, type, ilGetData());
							ilDeleteImages(1, &imgId);
						}
						texture.clear();					
					}
				}
				else
				{
					n_printf("file %s is too small (%d bytes)\n", fileName, fsize);
					this->SetWidth(0);
					this->SetHeight(0);
					this->SetDepth(0);			
				}

				fvol->Close();

				GLenum err = glGetError();			
				

				delete fvol;
				fvol = 0;
				return  err == GL_NO_ERROR;	
			} 
			else 
			{
				delete fvol;
				fvol = 0;
				n_printf("file %s could not be opened\n", fileName);				
			}
		} 
		else 
		{
			n_printf("file %s could not be found\n", fileName);			
		}
	}
	else
	{
		n_printf("glTexImage3DEXT function is not initialized\n");
	}
	return false;
}

bool nGlTexture::load_x_FileIntoTexture3D(const char* fileName) {
	GLenum target = (glTexImage3DEXT ? GL_TEXTURE_3D_EXT : GL_TEXTURE_2D);	
	stl_string ftemp = fileName;
	size_t pos = ftemp.find(".x.");
	if (pos != stl_string::npos) {
		stl_string begin = ftemp.substr(0, pos);
		stl_string end = ftemp.substr(pos+3);

		ILuint imgId = iluGenImage();
		ilBindImage(imgId);

		bool load = true;
		long i = 0;
		long imgWidth = 0, imgHeight = 0, newW = 0, newH = 0;
		size_t img_size;		

		GLint  components;
		ILenum type;
		ILenum format;
		byte_buffer texture;
		byte_buffer fileData;

		nFileServer2* fs = this->kernelServer->GetFileServer2();
		nGlServer* gs = (this->ref_gs.isvalid() ? (nGlServer*)this->ref_gs.get() : 0);
		int max_sz = (gs ? gs->max_texture_dim : 512);

		while (load) {
			char fname[N_MAXPATH];
			sprintf(fname, "%s.%d.%s", begin.c_str(), i, end.c_str());
			ILenum il_type = GetILType(fname);
			

			if (fs->FileExists(fname) && il_type != IL_TYPE_UNKNOWN) {					

				nFile* file = fs->NewFileObject();

				file->Open(fname, "rb");

				file->Seek(0, nFile::START);
                file->Seek(0, nFile::END);
                long fsize = file->Tell();
                file->Seek(0, nFile::START);

				fileData.resize(fsize);
				file->Read(fileData.get_buffer(), fsize); 
				file->Close();		
				delete file;
				file = 0;
				
				if (ilLoadL(il_type, fileData.get_buffer(), fsize)) {
					
					imgWidth  = ilGetInteger(IL_IMAGE_WIDTH);
					imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);					

					if (!i) {

						components = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
						format     = ilGetInteger(IL_IMAGE_FORMAT);
						bool gray = isgray(ilGetData(), imgWidth, imgHeight, components);

						unsigned char convert = 1;
						switch(format) {
						case IL_COLOR_INDEX:
						case IL_BGR:	
							if (gray)
								convert = ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
							else
								convert = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
							break;
						case IL_RGB:								
							if (gray)
								convert = ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
							break;
						case IL_BGRA:
							if (gray)
								convert = ilConvertImage(IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);							
							else
								convert = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
							break;
						case IL_RGBA:
							if (gray)
								convert = ilConvertImage(IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);							
							break;
						}

						
						if (!convert) {
							n_printf("nGlTexture: ilConvertImage() failed with '%s'\n", iluErrorString(ilGetError()));
							ilDeleteImages(1, &imgId);
							return false;
						}
					
						
						type       = ilGetInteger(IL_IMAGE_TYPE);
						format     = ilGetInteger(IL_IMAGE_FORMAT);
						components = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
						
						if (gs->non_power_of2) {
							newW = imgWidth;
							newH = imgHeight;						
						} else {
							// force texture dimensions to power of 2
							newW = nextPOW2(imgWidth);
							newH = nextPOW2(imgHeight);						
						}						
						
						if (newW > max_sz) newW = max_sz;
						if (newH > max_sz) newH = max_sz;    
						
						this->SetWidth(newW);
						this->SetHeight(newH);

						this->SetBytesPerPixel(components);
						this->SetNumMipLevels(1);
						
						img_size = newW * newH * components;
					} // !i

					if (((ILenum)ilGetInteger(IL_IMAGE_FORMAT) != format || 
						(ILenum)ilGetInteger(IL_IMAGE_TYPE) != type)  &&  !ilConvertImage(format, type)) {

						n_printf("nGlTexture: ilConvertImage() failed with '%s'\n", iluErrorString(ilGetError()));
						ilDeleteImages(1, &imgId);
						return false;
					}

					if ((imgWidth != newW) || (imgHeight != newH)) {
						if (!iluScale(newW, newH, 1)) {
							n_printf("nGlTexture: iluScale() failed with '%s'\n", iluErrorString(ilGetError()));
							ilDeleteImages(1, &imgId);
							return false;
						}
						imgWidth = newW;
						imgHeight = newH;
					}
					
					long offs = texture.size();
					long tex_sz = texture.size() + img_size;					
					texture.resize(tex_sz);
					void* tex_mem = texture.get_buffer() + offs;
					memcpy(tex_mem, ilGetData(), img_size);
					i++;						
				} else {
					load = false;
				}				
			}			
		} // while

		if (i) {
			glGenTextures(1, &this->tex_id);
			glBindTexture(target, this->tex_id);
			long depth = nextPOW2(i);
			if (depth != i) {
				depth = nextPOW2(i/2);
				if (depth > max_sz)
					depth = max_sz;
			}			
			this->SetDepth(depth);
			glTexImage3DEXT(GL_TEXTURE_3D_EXT, 0, format, imgWidth, imgHeight, depth, 0, format, type, texture.get_buffer());
			
			texture.clear();
		}

		GLenum err = glGetError();
		ilDeleteImages(1, &imgId);		
		return  i && (err == GL_NO_ERROR);	
		
	} else {
		n_printf("The %s is unsupported type file\n", fileName);
		return false;
	}
}


//------------------------------------------------------------------------------
/**
    Load an volume image and create an OpenGL 3Dtexture from it.
*/
bool nGlTexture::loadFileIntoTexture3D(const char* fileName) {
	if (!glTexImage3DEXT) {
		n_printf("The 3d texture is unsupported by hardware \n");
		return false;
	}
	

	if (strstr(fileName, ".vol") || strstr(fileName, ".img")) {
		return this->loadVolFileIntoTexture3D(fileName);			
	} else {
		stl_string ftemp = fileName;
		size_t pos = ftemp.find(".x.");
		if (pos != stl_string::npos) {
			return this->load_x_FileIntoTexture3D(fileName);
		} else {
			n_printf("The %s is unsupported type file\n", fileName);
			return false;
		}
	}	
}
