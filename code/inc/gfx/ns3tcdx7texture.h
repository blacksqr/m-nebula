#ifndef N_S3TCDX7TEXTURE_H
#define N_S3TCDX7TEXTURE_H

#include <d3dxcore.h>
#include "gfx/ns3tctexture.h"

class nS3TCDX7Texture : public nS3TCTexture {
public:
	nS3TCDX7Texture() : nS3TCTexture(), DX7format(D3DX_SF_UNKNOWN) {}

#ifdef USE_NFILE
    bool Load(nFile* file) {
        file->Seek(0, nFile::END);		
		unsigned long fsize = file->Tell();
		file->Seek(0, nFile::START);
		
		/* verify the type of file */ 
		file->Read(filecode, 4); 
		if (strncmp(filecode, "DDS ", 4) != 0) {			
            file->Close();
			return false; 
		}
		/* get the surface desc */ 	
		file->Read(&s3tc_desc, sizeof(s3tc_desc));		
		
		width = s3tc_desc.width; 
		height = s3tc_desc.height; 
		//genericImage->components = (ddsd.ddpfPixelFormat.dwFourCC == FOURCC_DXT1) ? 3 : 4; 
		numMipmaps = s3tc_desc.mipMapCount;
		format = GetS3TCFormatIndex(s3tc_desc.pfPixelFormat.fourCC);

		switch(format) { 
		case S3TC1: 
			DX7format = D3DX_SF_DXT1; 
			break; 
		case S3TC3: 
			DX7format = D3DX_SF_DXT3; 
			break; 
		case S3TC5: 
			DX7format = D3DX_SF_DXT5; 
			break; 
		default: 
			DX7format = D3DX_SF_UNKNOWN;			
			break;
		}
		if (DX7format) {
			/* how big is it going to be including all mipmaps? */ 
			//bufsize = s3tc_desc.mipMapCount > 1 ? s3tc_desc.linearSize * 2 : s3tc_desc.linearSize; 
			bufsize = fsize - sizeof(s3tc_desc) - 4;
			this->pixels = new unsigned char[bufsize]; 
			long readsize = file->Read(this->pixels, bufsize); 
			n_assert (readsize == bufsize);
		}		

		blockSize = (DX7format == D3DX_SF_DXT1) ? 8 : 16; 
		return DX7format != D3DX_SF_UNKNOWN;
	}
#else
	bool Load(const char* fileName) {
		FILE *fp;
		/* try to open the file */ 
		fp = fopen(fileName, "rb"); 
		if (!fp) 
			return false; 
		
		fseek(fp, 0L, SEEK_END);
		unsigned long fsize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		
		/* verify the type of file */ 
		fread(filecode, 1, 4, fp); 
		if (strncmp(filecode, "DDS ", 4) != 0) {
			fclose(fp);
			return false; 
		}
		/* get the surface desc */ 	
		fread(&s3tc_desc, sizeof(s3tc_desc), 1, fp);		
		
		width = s3tc_desc.width; 
		height = s3tc_desc.height; 
		//genericImage->components = (ddsd.ddpfPixelFormat.dwFourCC == FOURCC_DXT1) ? 3 : 4; 
		numMipmaps = s3tc_desc.mipMapCount;
		format = GetS3TCFormatIndex(s3tc_desc.pfPixelFormat.fourCC);

		switch(format) { 
		case S3TC1: 
			DX7format = D3DX_SF_DXT1; 
			break; 
		case S3TC3: 
			DX7format = D3DX_SF_DXT3; 
			break; 
		case S3TC5: 
			DX7format = D3DX_SF_DXT5; 
			break; 
		default: 
			DX7format = D3DX_SF_UNKNOWN;
			break;
		}
		if (DX7format) {
			/* how big is it going to be including all mipmaps? */ 
			//bufsize = s3tc_desc.mipMapCount > 1 ? s3tc_desc.linearSize * 2 : s3tc_desc.linearSize; 
			bufsize = fsize - sizeof(s3tc_desc) - 4;
			pixels = new unsigned char[bufsize]; 
			fread(pixels, 1, bufsize, fp); 
		}
		/* close the file pointer */ 
		fclose(fp);

		blockSize = (DX7format == D3DX_SF_DXT1) ? 8 : 16; 
		return DX7format != D3DX_SF_UNKNOWN;
	}
#endif  

	D3DX_SURFACEFORMAT DX7format;
};
#endif