#ifndef N_S3TC_TEXTURE_H
#define N_S3TC_TEXTURE_H
#include <stdio.h>
#define USE_NFILE

#ifdef USE_NFILE
#include "kernel/nfile.h"
#endif

#ifdef _DEBUG
//#define _DEBUG_DDS
#endif

#ifndef DUMMYUNIONNAMEN
#if defined(__cplusplus) || !defined(NONAMELESSUNION)
#define DUMMYUNIONNAMEN(n)
#else
#define DUMMYUNIONNAMEN(n)      u##n
#endif
#endif

#include "gfx/ns3tcfourcc.h"

//------------------------------------------------------------------------------
/*
	nS3TCCaps
*/
struct nS3TCCaps {
    unsigned long       caps;         // capabilities of surface wanted
    unsigned long       caps2;
    unsigned long       caps3;

    union {
        unsigned long       caps4;
        unsigned long       volumeDepth;
    } DUMMYUNIONNAMEN(1);
};

//------------------------------------------------------------------------------
/**
	nS3TCPixelFormat
*/
struct nS3TCPixelFormat {
    unsigned long       size;                 // size of structure
    unsigned long       flags;                // pixel format flags
    unsigned long       fourCC;               // (FOURCC code)
    union {
        unsigned long   RGBBitCount;          // how many bits per pixel
        unsigned long   YUVBitCount;          // how many bits per pixel
        unsigned long   ZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
        unsigned long   AlphaBitDepth;        // how many bits for alpha channels
        unsigned long   LuminanceBitCount;    // how many bits per pixel
        unsigned long   BumpBitCount;         // how many bits per "buxel", total
        unsigned long   PrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
                                        // format list and if DDPF_D3DFORMAT is set
    } DUMMYUNIONNAMEN(1);
    
	union {
        unsigned long   RBitMask;             // mask for red bit
        unsigned long   YBitMask;             // mask for Y bits
        unsigned long   StencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
        unsigned long   LuminanceBitMask;     // mask for luminance bits
        unsigned long   BumpDuBitMask;        // mask for bump map U delta bits
        unsigned long   Operations;           // DDPF_D3DFORMAT Operations
    } DUMMYUNIONNAMEN(2);
    
	union {
        unsigned long   GBitMask;             // mask for green bits
        unsigned long   UBitMask;             // mask for U bits
        unsigned long   ZBitMask;             // mask for Z bits
        unsigned long   BumpDvBitMask;        // mask for bump map V delta bits
        struct {
            unsigned short    FlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
            unsigned short    BltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
        } MultiSampleCaps;

    } DUMMYUNIONNAMEN(3);

    union {
        unsigned long   BBitMask;             // mask for blue bits
        unsigned long   VBitMask;             // mask for V bits
        unsigned long   StencilBitMask;       // mask for stencil bits
        unsigned long   BumpLuminanceBitMask; // mask for luminance in bump map
    } DUMMYUNIONNAMEN(4);
    
	union {
        unsigned long   RGBAlphaBitMask;      // mask for alpha channel
        unsigned long   YUVAlphaBitMask;      // mask for alpha channel
        unsigned long   LuminanceAlphaBitMask;// mask for alpha channel
        unsigned long   RGBZBitMask;          // mask for Z channel
        unsigned long   YUVZBitMask;          // mask for Z channel
    } DUMMYUNIONNAMEN(5);
};

//------------------------------------------------------------------------------
/**
	colorKey
 */
struct colorKey {
    unsigned long colorSpaceLowValue;   // low boundary of color space that is to
                                        // be treated as Color Key, inclusive
    unsigned long colorSpaceHighValue;  // high boundary of color space that is
                                        // to be treated as Color Key, inclusive
};

//------------------------------------------------------------------------------
/**
	nS3TCDesc
*/
struct nS3TCDesc {
    unsigned long				size;                 // size of the nS3TCDesc structure
    unsigned long               flags;                // determines what fields are valid
    unsigned long               height;               // height of surface to be created
    unsigned long               width;                // width of input surface
    
	union {
        long					pitch;                // distance to start of next line (return value only)
        unsigned long           linearSize;           // Formless late-allocated optimized surface size
    } DUMMYUNIONNAMEN(1);

    union {
        unsigned long           backBufferCount;      // number of back buffers requested
        unsigned long           depth;                // the depth if this is a volume texture 
    } DUMMYUNIONNAMEN(5);
    
	union {
        unsigned long           mipMapCount;          // number of mip-map levels requestde                                                // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
        unsigned long           refreshRate;          // refresh rate (used when display mode is described)
        unsigned long           srcVBHandle;          // The source used in VB::Optimize
    } DUMMYUNIONNAMEN(2);

    unsigned long               alphaBitDepth;        // depth of alpha buffer requested
    unsigned long               reserved;             // reserved
    void*						surface;              // pointer to the associated surface memory
    
	union {
        colorKey				ckCKDestOverlay;    // color key for destination overlay use
        unsigned long           emptyFaceColor;     // Physical color for empty cubemap faces
    } DUMMYUNIONNAMEN(3);

    colorKey					ckCKDestBlt;          // color key for destination blt use
    colorKey					ckCKSrcOverlay;       // color key for source overlay use
    colorKey					ckCKSrcBlt;           // color key for source blt use

    union    {
        nS3TCPixelFormat		pfPixelFormat;        // pixel format description of the surface
        unsigned long           dwFVF;                  // vertex format description of vertex buffers
    } DUMMYUNIONNAMEN(4);

    nS3TCCaps					caps;                // direct draw surface capabilities
    unsigned long               TextureStage;         // stage in multitexture cascade
};

class nS3TCTexture {
public:
	nS3TCTexture(){
		pixels = 0;
		bufsize = 
			numMipmaps = 
			width = 
			height = 			
			blockSize = 
			format = 0;
	}

	virtual ~nS3TCTexture() {
		if (pixels) {
			delete[] pixels;
			pixels = 0;
		}
	}	

#ifdef USE_NFILE
    long GetFormat(nFile* file) {        		
		if (!file) 
			return false; 
		/* verify the type of file */ 
        file->Seek(0, nFile::START);
        file->Read(filecode, 4); 
		if (strncmp(filecode, "DDS ", 4) != 0) {
            file->Seek(0, nFile::START);
			return false; 
		}
		/* get the surface desc */ 	
		file->Read(&s3tc_desc, sizeof(s3tc_desc));       

		width = s3tc_desc.width; 
		height = s3tc_desc.height; 
		//genericImage->components = (ddsd.ddpfPixelFormat.dwFourCC == FOURCC_DXT1) ? 3 : 4; 
		numMipmaps = s3tc_desc.mipMapCount; 

		format = GetS3TCFormatIndex(s3tc_desc.pfPixelFormat.fourCC);

		blockSize = (format == 0) ? 8 : 16; 
		return format;
	}
#else

	long GetFormat(const char* fileName) {
		FILE *fp;
		/* try to open the file */ 
		fp = fopen(fileName, "rb"); 
		if (!fp) 
			return false; 
		/* verify the type of file */ 
		fread(filecode, 1, 4, fp); 
		if (strncmp(filecode, "DDS ", 4) != 0) {
			fclose(fp);
			return false; 
		}
		/* get the surface desc */ 	
		fread(&s3tc_desc, sizeof(s3tc_desc), 1, fp);		
		fclose(fp);

		width = s3tc_desc.width; 
		height = s3tc_desc.height; 
		//genericImage->components = (ddsd.ddpfPixelFormat.dwFourCC == FOURCC_DXT1) ? 3 : 4; 
		numMipmaps = s3tc_desc.mipMapCount; 

		format = GetS3TCFormatIndex(s3tc_desc.pfPixelFormat.fourCC);

		blockSize = (format == 0) ? 8 : 16; 
		return format;
	}
#endif

	nS3TCDesc s3tc_desc;
	unsigned char* pixels;	
	char filecode[4]; 
	long bufsize;
	long numMipmaps;
	long width;
	long height;		
	long blockSize;
	long format;
};

#endif
