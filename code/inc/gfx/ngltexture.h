#ifndef N_GLTEXTURE_H
#define N_GLTEXTURE_H
//-------------------------------------------------------------------
/**
    @class nGlTexture

    @brief wraps OpenGL texture into nTexture object

    (C) 1999 A.Weissflog
*/
//-------------------------------------------------------------------
#ifndef N_TEXTURE_H
#include "gfx/ntexture.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_GLSERVER_H
#include "gfx/nglserver.h"
#endif
//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nGlTexture
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nGlServer;
class nGlPixelShader;
class N_DLLCLASS nGlTexture : public nTexture {
    
    friend class nGlPixelShader;

public:
    nGlTexture();
    virtual ~nGlTexture();
    virtual bool Load(void);
    virtual void Unload(void);

    /// + 28-jul-2002 by Ilya Kliot
    /// load texure from memory    
    virtual bool loadFromMem(void* p, unsigned long size, 
        long maxw = 256, long maxh = 256);
	
    static nKernelServer *kernelServer;

private:
    bool loadFileIntoTexture(const char *, const char *);
    bool loadFileIntoTextureIL(const char*);	
	/// Convert texture to right format and create gfx texture then;
	bool createTextureIL(const char* name, int imgId, long maxw = -1, long maxh = -1);
	// + 05-Nov-2002 compressed textures support, by Ilya Kliot, Clockwise  Ltd
	bool loadFileIntoTextureDDS(const char*);
    bool loadFileIntoTextureNTX(const char*);	
	/// 14-Jun-04 3d texture support by Ilya, Insightec, Inc
	bool loadFileIntoTexture3D(const char*);
	bool loadVolFileIntoTexture3D(const char*);
	bool load_x_FileIntoTexture3D(const char*);
    
    GLuint tex_id;
    nPixelFormat *pix_fmt;
};
//--------------------------------------------------------------------
#endif


