#ifndef N_S3TC_FOURCC_H
#define N_S3TC_FOURCC_H

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((unsigned long)(unsigned char)(ch0) | ((unsigned long)(unsigned char)(ch1) << 8) |   \
                ((unsigned long)(unsigned char)(ch2) << 16) | ((unsigned long)(unsigned char)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

/*
 * FOURCC codes for DX compressed-texture pixel formats
 */
#ifndef FOURCC_DXT1
#define FOURCC_DXT1  (MAKEFOURCC('D','X','T','1'))
#endif
#ifndef FOURCC_DXT2
#define FOURCC_DXT2  (MAKEFOURCC('D','X','T','2'))
#endif
#ifndef FOURCC_DXT3
#define FOURCC_DXT3  (MAKEFOURCC('D','X','T','3'))
#endif
#ifndef FOURCC_DXT4
#define FOURCC_DXT4  (MAKEFOURCC('D','X','T','4'))
#endif
#ifndef FOURCC_DXT5
#define FOURCC_DXT5  (MAKEFOURCC('D','X','T','5'))
#endif

#ifndef S3TC_COMPRESSION
#define S3TC_COMPRESSION
#define S3TC1 0
#define S3TC2 1
#define S3TC3 2
#define S3TC4 3
#define S3TC5 4
#define S3TC_ERR -1
#endif

inline long GetS3TCFormatIndex(unsigned long fourcc) {
	long format = S3TC_ERR;
	switch(fourcc) { 
	case FOURCC_DXT1: 
		format = S3TC1; 
		break; 
	case FOURCC_DXT2: 
		format = S3TC2; 
		break; 
	case FOURCC_DXT3: 
		format = S3TC3; 
		break; 
	case FOURCC_DXT4: 
		format = S3TC4; 
		break; 
	case FOURCC_DXT5: 
		format = S3TC5; 
		break; 
	default: 		
		format = S3TC_ERR;			
		break;
	}	
	return format;
}

#endif