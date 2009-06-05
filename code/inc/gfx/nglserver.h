#ifndef N_GLSERVER_H
#define N_GLSERVER_H
//-------------------------------------------------------------------
/**
    @class nGlServer

     @brief gfx server on top of OpenGL 1.1
								+OpenGL 1.5 by Ilya 2004
								+OpenGL 2.0 by Ilya 2006

	+ 05-Nov-2002 compressed textures support, by Ilya Kliot, Clockwise  Ltd
	+ 25-Mar-2003 anisotropic textures filtering support, by Ilya Kliot, Clockwise  Ltd
	+ 14-Jun-2004 3d texture support, by Ilya Kliot, Insightec Inc,	
	+ 04-Dec-2006 non power of 2 texture support, by Ilya Kliot, Insightec Inc,
    (C) 1999 A.Weissflog
*/
//-------------------------------------------------------------------
#include <stdlib.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifdef __WIN32__
#include <windows.h>
#endif

#ifdef __LINUX__
// Minimally, this define is needed to enable the definition of
// some of the GL_EXT_texture_env_dot3 type functions with Mesa
// 4.0 headers.  (This version will be with XFree 4.3, so more
// attention will need to be paid to it then.)
#define GL_GLEXT_LEGACY 1
#endif
#ifdef __MACOSX__
// Define this to force some constants to be initialized from
// the Apple glext.h header.  Apple defines the ARB versions
// of the texture env stuff from OpenGL 1.3 while Nebula uses
// the EXT form from OpenGL 1.2.
#define GL_EXT_texture_env_combine 1
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <Carbon/carbon.h>
#include <AGL/agl.h>
#undef GL_EXT_texture_env_combine
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef __LINUX__
#include <GL/glx.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdCmap.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>
#endif

#include "gfx/nglext.h"
#include "util/nstl.h"

#undef N_DEFINES
#define N_DEFINES nGlServer
#include "kernel/ndefdllclass.h"

bool loadGLSLLog(GLhandleARB obj, stl_string& logRef);

//-------------------------------------------------------------------
class nInputServer;
class nProfiler;

class N_DLLCLASS nGlServer : public nGfxServer {
public:
    static nKernelServer *kernelServer;

    enum {
        N_STATICPOOLSIZE    = 8000,
        N_READONLYPOOLSIZE  = 8000,
        N_WRITEONLYPOOLSIZE = 8000,
    };

    nAutoRef<nInputServer> ref_is;
    nRef<nRoot> ref_devdir;
    
    bool	display_open;
    bool	in_begin_scene;
    bool	in_begin;
    bool	render_ok;
    bool	is_fullscreen;    
    float	clear_color[4];
    int		cur_vidmode_id; 
    int		num_texture_units;
	int		max_texture_dim;	
	int		aux_buff_num;	
	float	max_anisotropy;

    enum {
        N_GLCMD_TOGGLEFULLSCREEN = 1001,
        N_GLCMD_NEXTMODE         = 1002,
        N_GLCMD_PREVMODE         = 1003,
    };

#   ifdef __WIN32__
    char wnd_classname[32];
    HINSTANCE hinst;       
    HWND  hwnd;            
    HDC   hdc;             
    HGLRC hrc;             
    HACCEL accel;

    bool class_registered;
    bool quit_requested;
#   endif
    
#   ifdef __LINUX__
    Display *dsp;
    Window ctx_win;
    Window wm_win;
    Window fs_win;

    XFontStruct *fs;
    XEvent event;
    GLXContext cx; 
    XVisualInfo *xvis;
    Atom wmDeleteWindow;

    bool resize;
    bool focused;
    unsigned int keyboard_grabbed;

    // full screen support status
    bool ext_XF86VIDMODE;
    XF86VidModeModeInfo fs_mode;
    XF86VidModeModeInfo wm_mode;
    int wm_mode_x, wm_mode_y;
#   endif

#   ifdef __MACOSX__
    WindowRef window;
    AGLContext ctx;
    bool quit_requested;
#   endif

    // Text Support
    bool text_initialized;
    bool in_begin_text;
	bool in_begin_hud;
    int text_listbase;
    int text_numglyphs;
    float text_height;
    float text_x, text_y;

    // supported extensions        
    bool ext_clip_volume_hint;          // EXT_clip_volume_hint supported?    
    bool ext_texture_env_combine;       // EXT_texture_env_combine supported?
    bool ext_texture_env_add;           // EXT_texture_env_add supported?
    bool ext_texture_env_dot3;          // EXT_texture_env_dot3 supported?
    bool ext_blend_color;               // EXT_blend_color supported?
    bool ext_bgra;                      // EXT_bgra supported?    	
	bool ext_clamp_to_edge;             // EXT_bgra supported?    
    
	
	// + 25-Mar-2003 anisotropic textures filtering support, by Ilya Kliot, Clockwise  Ltd
	bool  ext_filter_anisotropic;

	bool ext_texture_compression;	
	bool non_power_of2;
    // misc...
    bool global_fog_enable;

    // the vertex pool manager objects for the different types
    // of vertex pools
    nVertexPoolManager *vpool_manager[N_NUM_VBTYPES];

    GLenum alpha_src;
    GLenum alpha_dst;
    GLenum stencil_fail;
    GLenum stencil_zfail;
    GLenum stencil_pass;
    GLenum stencil_func;
    GLint  stencil_ref;
    GLuint stencil_mask;
    GLint  stencil_clear;

	// buffer mask
	int gl_clear_buffer_bits;

public:
    nGlServer();
    virtual ~nGlServer();
	
public:
    // inherited
    virtual bool SetDisplayMode(const char *);
    virtual bool OpenDisplay(void);
    virtual void CloseDisplay(void);
    virtual bool GetDisplayDesc(int& x0, int& y0, int& x1, int& y1);
	virtual void Resize(int x, int y, int w, int h);

    virtual bool GetFontDesc(int& height);
    virtual void SetViewVolume(float, float, float, float, float, float);
    virtual void SetClearColor(float, float, float, float);
    virtual void GetClearColor(float&, float&, float&, float&);    

	virtual void SetClearBuffer(nClearBufType buffer, bool clear) 
	{ 
		nGfxServer::SetClearBuffer(buffer, clear);		
		this->gl_clear_buffer_bits = GL_COLOR_BUFFER_BIT * ((this->clear_buffer_bits & N_COLOR_BUFFER)/N_COLOR_BUFFER);
		this->gl_clear_buffer_bits |= GL_DEPTH_BUFFER_BIT * ((this->clear_buffer_bits & N_DEPTH_BUFFER)/N_DEPTH_BUFFER);
		this->gl_clear_buffer_bits |= GL_STENCIL_BUFFER_BIT * ((this->clear_buffer_bits & N_STENCIL_BUFFER)/N_STENCIL_BUFFER);
		this->gl_clear_buffer_bits |= GL_ACCUM_BUFFER_BIT * ((this->clear_buffer_bits & N_ACCUM_BUFFER)/N_ACCUM_BUFFER);
	}	
   
    virtual bool BeginScene(void);
    virtual void EndScene(void);

    virtual void SetMatrix(nMatrixMode, matrix44&);
	virtual void SetIdentity(nMatrixMode);
    virtual void PushMatrix(nMatrixMode);
    virtual void PopMatrix(nMatrixMode);

    virtual void SetState(nRState&);
    virtual bool SetLight(nLight&);
    virtual bool SetClipPlane(int, vector4&);	

    virtual void Begin(nPrimType);
    virtual void Coord(float, float, float);
    virtual void Norm(float, float, float);
    virtual void Rgba(float, float, float, float);
    virtual void Uv(ulong layer, float, float);
    virtual void End(void);    

	// + 28-Jan-2007 selection support added
	/// copying pixels from selection buffer into buf, if buf is zero then to disk

	GLenum GetFrameBufferType(nFrameBufferType bt) const
	{
		switch(bt) 
		{
		case N_BUFFER_FRONT:
			return GL_FRONT;
		case N_BUFFER_BACK:
			return GL_BACK;
		case N_BUFFER_SELECT:
			return (this->aux_buff_num ? GL_AUX0 : GL_BACK);
		default:
			n_assert("incorrect buffer type" && false);
			return GL_BACK;
			break;
		}
	}
	virtual bool GetFrameBufferPixels(nFrameBufferType bt, int x, int y, int w, int h, uchar* buf);

    virtual nTexture*		NewTexture(const char *id);
	virtual nFont*			NewFont(const char *, nFontType );
    virtual nPixelShader*	NewPixelShader(const char *id);
    virtual nVertexPool*	NewVertexPool(nVertexPoolManager *, nVBufType, int, int);
    virtual nVertexBuffer*	NewVertexBuffer(const char *, nVBufType vbtype, int vtype, int vnum);
	virtual nShaderPair*	NewShaderPair(const char* id);
	virtual nShader*		NewShader(const char* sh);	

	// Text-Support
    virtual bool BeginText(void);
    virtual void TextPos(float, float);
    virtual bool Text(const char *);
    virtual bool EndText(void);

	// HUD support
	virtual void BeginHUD(void);
	virtual void EndHUD(void);

	/// show mouse cursor
	virtual void SetCursorShown(bool show);

    virtual bool Trigger(void);

    // private
    void _reshapeDisplay(long w, long h);
    void initGl(void);
    void initExtensions(void);
    bool hasExtension(const char *);

    bool winOpen(void);
    bool winClose(void);
    void invalidate_context(void);
    bool dispOpen(bool fullscreen, int w, int h, int bpp, int x, int y);
    bool dispClose(void);
    //GLenum getGlFilterMode(nRStateParam);

    char *nextDisplayMode(char *, int);
    char *prevDisplayMode(char *, int);

#ifdef __WIN32__
    void enumDisplayModes(HDC _hdc);
    void enumDisplayModes(void);
    bool classRegister(void);
    bool classUnregister(void);
#endif

#ifdef __LINUX__
    void enumDisplayModes(void);
    bool GetVideoMode (XF86VidModeModeInfo &, int w, int h);
    bool SwitchVideoMode (XF86VidModeModeInfo *, Window, int, int, int, int, bool);
    bool HotKeyVideoMode (KeySym &ks);
    bool GrabInputs ();
    void UngrabInputs ();
    bool xwin_proc(void);
#endif    
protected:
	virtual void update_window();
};


//-------------------------------------------------------------------
/**
	@brief check OpenGL status
	nCheckGlError()
	26-Nov-98   ilya    created
*/
inline bool nCheckGlError(void)
{
    GLenum error;
	bool res = false;
    while ((error = glGetError()) != GL_NO_ERROR)
	{
        n_printf("gl error: %s\n", gluErrorString(error));
		_flushall();
		res = true;
    }
	return res;
}

#ifdef _DEBUG
#define _glDBG { if (nCheckGlError()) n_printf("in file: %s, line %d\n", __FILE__, __LINE__); }
#else
#define _glDBG
#endif


//-------------------------------------------------------------------
/**
	@brief wrap enable disable with check

	27-Oct-06   ilya    created
*/
inline void nSetGlCapability(bool enable, GLenum capability) {
	if (enable)
		glEnable(capability);
	else
		glDisable(capability);

	_glDBG
}


//-------------------------------------------------------------------
/**
	@brief get GL primitive from nebula primitive type

	20-Dec-07   ilya    created
*/
inline GLenum nGetGlPrimType(nPrimType ptype)
{
	return GL_POINTS + (GLint)ptype;
	/*
	GLenum glptype = 0;
	switch(ptype) {
	case N_PTYPE_CONVEX_POLY:	 glptype = GL_POLYGON; break;
	case N_PTYPE_TRIANGLE_LIST:  glptype = GL_TRIANGLES; break;
	case N_PTYPE_TRIANGLE_FAN:   glptype = GL_TRIANGLE_FAN; break;
	case N_PTYPE_TRIANGLE_STRIP: glptype = GL_TRIANGLE_STRIP; break;
	case N_PTYPE_LINE_LIST:      glptype = GL_LINES; break;
	case N_PTYPE_LINE_STRIP:     glptype = GL_LINE_STRIP; break;
	case N_PTYPE_POINT_LIST:     glptype = GL_POINTS; break;
	default: n_error("nGlServer::GetGlPrimType(): Unknown primitive type!"); break;
	}
	return glptype;
	*/
}

inline GLenum nGetGlStencilOp(nRStateParam op)
{
	n_assert(op >= N_STENCILOP_KEEP && op <= N_STENCILOP_DECR);
	static GLenum ops[] = { GL_KEEP, GL_ZERO, GL_REPLACE, GL_INVERT, GL_INCR, GL_DECR };
	return ops[op-N_STENCILOP_KEEP];
/*
	GLenum glOp = GL_KEEP;
	switch (op)
	{
		case N_STENCILOP_KEEP:      glOp = GL_KEEP; break;
		case N_STENCILOP_ZERO:      glOp = GL_ZERO; break;
		case N_STENCILOP_REPLACE:   glOp = GL_REPLACE; break;
		case N_STENCILOP_INVERT:    glOp = GL_INVERT; break;
		case N_STENCILOP_INCR:      glOp = GL_INCR; break;
		case N_STENCILOP_DECR:      glOp = GL_DECR; break;
		default:                    glOp = GL_KEEP; break;
	}
	return glOp;
	*/
}

//-------------------------------------------------------------------
/**
	@brief get GL compare func from nebula gfx state

	20-Dec-07   ilya    created
*/
inline GLenum nGetGlCmp(nRStateParam nCmp/*, GLenum def = GL_LESS*/)
{
//	GLenum glCmp = def;
//	if (nCmp >= N_CMP_NEVER && nCmp <= N_CMP_ALWAYS)
//	{
	n_assert(nCmp >= N_CMP_NEVER && nCmp <= N_CMP_ALWAYS);
	return (GL_NEVER + (nCmp - N_CMP_NEVER));
//	}
	/*
	switch (nCmp) {
		case N_CMP_NEVER:           glCmp = GL_NEVER; break;
		case N_CMP_LESS:            glCmp = GL_LESS; break;
		case N_CMP_EQUAL:           glCmp = GL_EQUAL; break;
		case N_CMP_LESSEQUAL:       glCmp = GL_LEQUAL; break;
		case N_CMP_GREATER:         glCmp = GL_GREATER; break;
		case N_CMP_NOTEQUAL:        glCmp = GL_NOTEQUAL; break;
		case N_CMP_GREATEREQUAL:    glCmp = GL_GEQUAL; break;
		case N_CMP_ALWAYS:          glCmp = GL_ALWAYS; break;
		default:                    glCmp = GL_LESS; break;
	}
	*/
	//return glCmp;
}

inline void nDisableTexArrays(int n) 
{
	for (int i = 0; i < n; i++)
	{
  		glClientActiveTextureARB(GLenum(GL_TEXTURE0_ARB + i));
  		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

inline void nSwitchGlCapsOff() 
{
	nSetGlCapability(false, GL_LINE_SMOOTH);
	nSetGlCapability(false, GL_POINT_SMOOTH);
	nSetGlCapability(false, GL_DITHER);
	nSetGlCapability(false, GL_LIGHTING);
	nSetGlCapability(false, GL_ALPHA_TEST);
	nSetGlCapability(false, GL_STENCIL_TEST);
	nSetGlCapability(false, GL_BLEND);
	nSetGlCapability(false, GL_FOG);
	nSetGlCapability(false, GL_TEXTURE_2D);
	nSetGlCapability(true,	GL_COLOR_MATERIAL);	
}
//-------------------------------------------------------------------
#endif    

 
