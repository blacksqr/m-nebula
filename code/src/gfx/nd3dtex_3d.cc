#define N_IMPLEMENTS nD3D8Texture
//------------------------------------------------------------------------------
//  ngltex_ntx.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nd3d8texture.h"
#include "gfx/nopenilfile.h"

#ifdef _UNICODE
#undef _UNICODE
#endif

#include "il/il.h"
#include "il/ilu.h"

static unsigned int 
nextPOW2(unsigned long val) {    
    for (unsigned long i = 1; i < (1<<30); i <<= 1) 
		if (val <= i) return i;        
    return i;
}

static unsigned long
GetPOW2(unsigned long val) {    
	long pow = 0;
    while (val) {
		val >>= 1;
		pow++;
    }
	return pow;
}
extern const char *nd3d8_Error(HRESULT hr);

//------------------------------------------------------------------------------
/**
    Load an volume image and create an OpenGL 3Dtexture from it.
*/
bool nD3D8Texture::loadFileIntoTexture3D(const char* fileName) {
	
	bytes_t texture;
	D3DFORMAT dstD3DFormat, srcD3DFormat;
	HRESULT hr;   

	int srcBytesPerPixel;
    int dstBytesPerPixel;
	

	n_assert(0 == this->vol);
    n_assert(this->ref_gs->d3d8Dev);
	
	if (!this->ref_gs->textures_3d) {
		n_printf("Volume textures rendering is not supported '%s'\n", fileName);
		return false;
	}

	long w, h, d;

	if (strstr(fileName, ".vol")) {
		// bind the images to OpenGL
		
		FILE* fvol = fopen(fileName, "rb");
		if (fvol) {			
			long dim[4];
			fread(dim, 4*sizeof(long), 1, fvol);

			w = dim[0];
			h = dim[1];
			d = dim[2];			
			
			switch (dim[3]) {
			case IL_LUMINANCE:
				srcBytesPerPixel = dstBytesPerPixel = 1;
				srcD3DFormat = dstD3DFormat = D3DFMT_L8;
				
				break;
			case IL_LUMINANCE_ALPHA:
				srcBytesPerPixel = dstBytesPerPixel = 2;
				srcD3DFormat = dstD3DFormat = D3DFMT_A8L8;
				
				break;
			}
			size_t	img_sz = w*h*d*srcBytesPerPixel;
			texture.resize(img_sz);
			fread(texture.begin(), img_sz, 1, fvol);

			fclose(fvol);			
		} else {
			return false;
		}
	} else {
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
			long  max_sz = 512;		

			int  components;
			ILenum type;
			ILenum format;
    
			while (load) {
				char fname[N_MAXPATH];
				sprintf(fname, "%s.%d.%s", begin.c_str(), i, end.c_str());

				ILenum il_type = GetILType(fname);

				if (il_type != IL_TYPE_UNKNOWN && ilLoadImage(fname)) {
					
					w = ilGetInteger(IL_IMAGE_WIDTH);
					h = ilGetInteger(IL_IMAGE_HEIGHT);

					if (!i) {
						unsigned char convert = 1;
						switch(ilGetInteger(IL_IMAGE_FORMAT)) {
						case IL_COLOR_INDEX:
						case IL_LUMINANCE:
						case IL_RGB:
							convert = ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
						case IL_BGR:							
							srcD3DFormat     = D3DFMT_R8G8B8;
							srcBytesPerPixel = 3;
							if (this->highQuality)
							{
								dstD3DFormat     = D3DFMT_R8G8B8;
								dstBytesPerPixel = 3;
								n_printf("*** NOTE: loading high quality '%s'\n", fileName);
							} else {
								dstD3DFormat     = D3DFMT_R5G6B5;
								dstBytesPerPixel = 2;
							}
							break;							
						case IL_RGBA:
						case IL_LUMINANCE_ALPHA:
							convert = ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
						case IL_BGRA:
							srcD3DFormat     = D3DFMT_A8R8G8B8;
							srcBytesPerPixel = 4;
							if (this->highQuality)
							{
								dstD3DFormat     = D3DFMT_A8R8G8B8;
								dstBytesPerPixel = 4;
								n_printf("*** NOTE: loading high quality '%s'\n", fileName);
							}
							else
							{
								dstD3DFormat     = D3DFMT_A4R4G4B4;
								dstBytesPerPixel = 2;
							}							
						}

						
						if (!convert) {
							n_printf("nGlTexture: ilConvertImage() failed with '%s'\n", iluErrorString(ilGetError()));
							ilDeleteImages(1, &imgId);
							return false;
						}
					
						
						type       = ilGetInteger(IL_IMAGE_TYPE);
						format     = ilGetInteger(IL_IMAGE_FORMAT);
						components = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    
						// force texture dimensions to power of 2
						newW = nextPOW2(w);
						newH = nextPOW2(h);						
						
						
						if (newW > max_sz) newW = max_sz;
						if (newH > max_sz) newH = max_sz;    
						
						this->SetWidth(w);
						this->SetHeight(h);
						this->SetBytesPerPixel(components);
						this->SetNumMipLevels(1);
						
						img_size = newW * newH * components;
					}				

					if ((w != newW) || (h != newH)) {
						if (!iluScale(newW, newH, 1)) {
							n_printf("nGlTexture: iluScale() failed with '%s'\n", iluErrorString(ilGetError()));
							ilDeleteImages(1, &imgId);
							return false;
						}
						w = newW;
						h = newH;
					}
					
					long offs = texture.size();
					long tex_sz = texture.size() + img_size;					
					texture.resize(tex_sz);
					void* tex_mem = texture.begin() + offs;
					memcpy(tex_mem, ilGetData(), img_size);
					i++;
				} else {
					load = false;
				}				
				d = i;
			} // while			
			ilDeleteImages(1, &imgId);
		}		
	}

	hr = D3DXCreateVolumeTexture(this->ref_gs->d3d8Dev, 
	w, h, d, 
	1,//D3DX_DEFAULT, 
	0, 
	dstD3DFormat,
	D3DPOOL_MANAGED, 
	&(this->vol));

	if (FAILED(hr)) {
		n_printf("nD3D8Texture: loadFileIntoTexture3D() failed with '%s'\n", nd3d8_Error(hr));
		return false;
	}
	D3DBOX box = {0, 25, 0, 256, 0, 32};
	
	int numMipLevels = 1;
	int numLevels    = 1;
//	if (this->gen_mipmaps) { 	
//		numMipLevels = D3DX_DEFAULT;
//		numLevels    = GetPOW2(n_min3(w, h, d));
//	}

	//for (int curLevel = 0; curLevel < numLevels; curLevel++) {
	IDirect3DVolume8* mipVol;
	hr = this->vol->GetVolumeLevel(0, &mipVol);
	if (FAILED(hr)) {
		n_printf("nD3D8Texture: loadFileIntoTexture3D() failed with '%s'\n", nd3d8_Error(hr));
		return false;
	}
	hr = D3DXLoadVolumeFromMemory(mipVol, 0, 0, texture.begin(), srcD3DFormat, w * srcBytesPerPixel, w * h * srcBytesPerPixel, 0, &box, D3DX_FILTER_NONE, 0);
	if (FAILED(hr)) {
		n_printf("nD3D8Texture: loadFileIntoTexture3D() failed with '%s'\n", nd3d8_Error(hr));
		return false;
	}
    return true;
}
                        
        

