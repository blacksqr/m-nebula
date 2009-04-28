#ifndef N_GFXSERVER_H
#define N_GFXSERVER_H
//--------------------------------------------------------------------
/**
    @class nGfxServer

    @brief base class for gfx servers

    Wraps around OpenGL and Direct3D, with the intention to
    have a simple, somewhat OpenGL like interface for managing the
    display and fast rendering of vertex buffers.
  
    (C) 1999 A.Weissflog
*/
//--------------------------------------------------------------------

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_LIGHT_H
#include "gfx/nlight.h"
#endif

#ifndef N_ENV_H
#include "kernel/nenv.h"
#endif

#ifndef N_VERTEXPOOLMANAGER_H
#include "gfx/nvertexpoolmanager.h"
#endif

#ifndef N_TEXTURE_H
#include "gfx/ntexture.h"
#endif

#ifndef N_MOUSECURSOR_H
#include "gfx/nmousecursor.h"
#endif

#undef N_DEFINES
#define N_DEFINES nGfxServer
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nTexture;
class nFont;
class nPixelShader;
class nVertexPool;
class nVertexBuffer;
class nIndexBuffer;
class nProfiler;
class nShader;
class nShaderPair;

#define N_GFX_TEXDIR        "/sys/share/tex"
#define N_GFX_FONTDIR       "/sys/share/font"
#define N_GFX_CNCVDIR       "/sys/share/cncv"
#define N_GFX_PSHADERDIR    "/sys/share/pshader"
#define N_GFX_SHADERDIR    "/sys/share/shader"
#define N_GFX_VXBUFDIR      "/sys/share/vxbuf"
#define N_GFX_IXBUFDIR      "/sys/share/ixbuf"
#define N_GFX_HNDDIR        "/sys/share/hnd"

//-------------------------------------------------------------------
/**
    @class nGfxStat

    @brief A small graphics statistics object.
*/
//-------------------------------------------------------------------
class nGfxStat {
    int val;            // the statistics variable
    nRef<nEnv> ref_env; // the watcher variable
    int min;
    int max;
    float av;
    bool ShowExtra;

public: 
    nGfxStat(nKernelServer *ks, nRoot *owner, const char *name, bool showExtra=false)
        : ref_env(owner)
    {
        char buf[N_MAXPATH];
        sprintf(buf,"/sys/var/%s",name);
        nEnv *e = (nEnv *) ks->New("nenv",buf);
        n_assert(e);
        this->ref_env = e;
        this->Reset();
        this->ShowExtra = showExtra;
    };
    ~nGfxStat() {
		release_ref(ref_env);        
    };

    void Reset(void) {
        this->val = 0;
        this->min = 2147483647;
        this->max = -2147483647;
        this->av  = 0;
    };
    void SetVal(int v) {
        val = v;
        Calc();
    };
    void AddVal(int v) {
        val += v;
        Calc();
    };
    void Flush(void) {
        if (this->ShowExtra) {
            char buff[120];
            sprintf(buff, "%8d,   av:%8.3f,   minmax:%8d, %8d",val,av,min,max);
            this->ref_env->SetS(buff);
        } 
        else 
        this->ref_env->SetI(val);
    };
    void Calc(){
        if (val>max) max=val;
        if (val<min) min=val;
        if (val)     av = (av*9 + val)* 0.1f;
    }
    
private:
    nGfxStat(nRoot *owner) : ref_env(owner) {};
};

//-------------------------------------------------------------------
template <class TYPE, int SIZE> class nMatrixStack {
    int pos;
    TYPE stack[SIZE];

public:
    nMatrixStack() : pos(0) {};
    void Push(TYPE& m) {
        n_assert(pos < SIZE);
        stack[pos++] = m;
    };
    TYPE Pop(void) {
        n_assert(pos > 0);
        return stack[--pos];
    };
};

//-------------------------------------------------------------------
class N_PUBLIC nGfxServer : public nRoot {
public:
    enum {
        N_MAXLIGHTS = 8,
        N_MXSTACKDEPTH = 4,
        N_MAXCURSORS = 8,
    };

    enum nGfxStatType {
        N_GFXSTATS_TRIANGLES,		
        N_GFXSTATS_FRAMERATE,
        N_GFXSTATS_MODELVIEWS,
        N_GFXSTATS_LIGHTS,
        N_GFXSTATS_TEXTURES,
        N_GFXSTATS_TEXTURESIZE,
        N_GFXSTATS_PSHADER_SWITCHES,
        N_GFXSTATS_MESH_SWITCHES,
        N_GFXSTATS_TEXTURE_SWITCHES,
    };

    enum nWindowMode 
	{
        N_WINDOWMODE_UNDEFINED =(0<<0),
        N_WINDOWMODE_ACTIVE =	(1<<0),
        N_WINDOWMODE_HIDDEN =	(1<<1),
		N_WINDOWMODE_TOPMOST =	(1<<2),
		N_WINDOWMODE_FRAMED =	(1<<3)        
    };

public:    
    static nKernelServer *kernelServer;

    static nProfiler *prof_fps;

    nGfxServer();
    virtual ~nGfxServer();

	virtual void Initialize();

    // Display Management
    virtual bool SetDisplayMode(const char *);
    virtual const char *GetDisplayMode(void);
    virtual bool OpenDisplay(void);
    virtual void CloseDisplay(void);
    virtual bool GetDisplayDesc(int& x0, int& y0, int& x1, int& y1);
	virtual void Resize(int x, int y, int w, int h);
	virtual void SetWindowMode(nWindowMode wm)    { n_setbit(wm, this->windowmode); this->update_window();   }
	virtual void UnsetWindowMode(nWindowMode wm)  { n_unsetbit(wm, this->windowmode); this->update_window(); }	
	virtual bool GetWindowMode(nWindowMode wm)	  { return n_getbit(wm, this->windowmode);					 }


    virtual bool GetFontDesc(int& height);
    virtual void SetViewVolume(float minx, float maxx, float miny, float maxy, float minz, float maxz);
	virtual void GetViewVolume(float& minx, float& maxx, float& miny, float& maxy,float& minz, float& maxz);
	virtual vector2 GetPixelSize() const { return this->pixel_size;	}
	virtual void SetOrtho(bool b);
	virtual bool GetOrtho() const;
	virtual bool SetAA(bool aa);
	virtual bool GetAA() const;
	virtual void SetAutoNormalize(bool b) {this->autoNormalize = b;}
	virtual bool GetAutoNormalize() const {return this->autoNormalize;}

    virtual void SetPerspective(float fovDegrees, float aspectRatio, float zNear, float zFar);
    virtual void GetPerspective(float&, float&, float&, float&);
    virtual void SetFOV(float fovDegrees);
    virtual float GetFOV();
    virtual void SetClearColor(float, float, float, float);
    virtual void GetClearColor(float&, float&, float&, float&);
	virtual void SetClearBuffer(nClearBufType buffer, bool clear) { n_bsetbit (clear, (int)buffer, this->clear_buffer_bits); }
	virtual bool GetClearBuffer(nClearBufType buffer) { return n_getbit(buffer, this->clear_buffer_bits); }
    virtual bool Screenshot(const char *, nFrameBufferType bt = N_BUFFER_FRONT);
    virtual bool ScaledScreenshot(const char *, int, int, nFrameBufferType bt = N_BUFFER_FRONT);

	// + 28-Jan-2007 selection support added
	/// copying pixels from buffer into buf, if buf is zero then to disk
	virtual bool GetFrameBufferPixels(nFrameBufferType bt, int x, int y, int w, int h, uchar* buf);

    virtual nColorFormat GetColorFormat(void);
    
	virtual void AskScreenshot(nFrameBufferType bt) { this->screen_shots_ask |= (int)bt; }

    // Scene Management (between OpenDisplay/CloseDisplay)
    virtual bool BeginScene(void);
    virtual void EndScene(void);

    virtual void SetMatrix(nMatrixMode, matrix44&);
	virtual void SetIdentity(nMatrixMode);
    virtual void GetMatrix(nMatrixMode, matrix44&) const;
	virtual const matrix44& GetMatrix(nMatrixMode) const;

    virtual void PushMatrix(nMatrixMode);
    virtual void PopMatrix(nMatrixMode);

	virtual vector2 Project2D(const vector3& v) { return v32v2(this->GetMatrix(N_MXM_PROJECTION) * v); }
	virtual vector3 Project3D(const vector3& v) { return this->GetMatrix(N_MXM_PROJECTION) * v; }

    virtual void SetState(nRState&);
    virtual bool SetClipPlane(int, vector4&);
    virtual bool SetLight(nLight&);
    const nLight& GetLight(int);
    int GetNumLights();

	// primitives
    virtual void Begin(nPrimType);
    virtual void Coord(float, float, float);
    virtual void Norm(float, float, float);
    virtual void Rgba(float, float, float, float);
    virtual void Uv(ulong layer, float, float);
    virtual void End(void);   
    
    // Object Factory
	virtual nRoot*			NewRsrc(const char*, const char*, nRoot*);
    virtual nTexture*		NewTexture(const char*);
	virtual nFont*			NewFont(const char*, nFontType );

    virtual nPixelShader*	NewPixelShader(const char*);
	virtual nShaderPair*	NewShaderPair(const char* id);
	virtual nShader*		NewShader(const char* sh);
    
	virtual nVertexPool*	NewVertexPool(nVertexPoolManager*, nVBufType, int, int);
    virtual nVertexBuffer*	NewVertexBuffer(const char*, nVBufType vbtype, int vtype, int vnum);
    virtual nIndexBuffer*	NewIndexBuffer(const char*);	

    // Resource Sharing and Management
	virtual nRoot*			FindRsrc(const char*, nRoot*);
    virtual nTexture*		FindTexture(const char*);
	virtual nFont*			FindFont(const char*, nFontType );

    virtual nPixelShader*	FindPixelShader(const char*);
	virtual nShaderPair*	FindShaderPair(const char*);
	virtual nShader*		FindShader(const char*);

	virtual nVertexBuffer*	FindVertexBuffer(const char*);
    virtual nIndexBuffer*	FindIndexBuffer(const char*);

    // redundant render state handling
    void SetCurrentPixelShader(nPixelShader *ps);
    nPixelShader* GetCurrentPixelShader(void);
    void SetCurrentVertexBubble(nVertexBubble *vb);
    nVertexBubble* GetCurrentVertexBubble(void);
    void SetCurrentTexture(int stage, nTexture *tex);
    nTexture* GetCurrentTexture(int stage);

    // Text Support
    virtual bool BeginText(void);
    virtual void TextPos(float, float);
    virtual bool Text(const char *);
    virtual bool EndText(void);

	// HUD support
	virtual void BeginHUD(void);
	virtual void EndHUD(void);

    // Window-System-Message-Pump
    virtual bool Trigger(void);

    // Statistics
    void BeginStats(void);
    void SetStats(nGfxStatType t, int n);
    void EndStats(void);

    /// begin defining mouse cursors
    void BeginCursors(int num);
    /// set a single mouse cursor
    void SetCursor(int index, const char* texPath, int xHotspot, int yHotspot);
    /// finish defining mouse cursors
    void EndCursors();
    /// select the current mouse cursor
    void SetCurrentCursor(int index, bool show);
    /// get the current mouse cursor
    int GetCurrentCursor();
    /// show mouse cursor
	virtual void SetCursorShown(bool show) { this->cursorShown = show; }
	/// Check cursor visibility
	virtual bool GetCursorShown() const {return this->cursorShown;}
	/// Set mouse position	
	virtual void SetMousePos(float x, float y);

protected:
    /// load a mouse cursors texture from its path
    void LoadCursors();

    char* getResourceID(const char *, char *, ulong);
    const char* getModeTag(const char *, const char *, char *, int);
    void releaseResources();
	void releaseRsrc(nRef<nRoot>& rsrc);

	void makeModeString();	

	virtual void update_window();
public:
	int disp_w;
	int disp_h;
	int disp_x;
	int disp_y;
	int disp_bpp;
	int disp_zbufbits;          // requested zbuffer bit depth
	int disp_sbufbits;          // requested stencil buffer bit depth
	int disp_samples;

protected: 
	int windowmode;

    // display mode stuff
    char disp_mode[256];
    char disp_type[32];
    char disp_dev[64];

    // unique id for resources 
    ulong unique_id;
    // frustum extents
    float clip_minx,clip_maxx;
    float clip_miny,clip_maxy;
    float clip_minz,clip_maxz;	
  	bool  ortho;
	bool  antialiasing;
	vector2	pixel_size;
	
	double timeStamp;           // time stamp of last frame

    // lighting
    int numLights;
    nLight lights[N_MAXLIGHTS];

    // Shared Resource Directories
    nRef<nRoot> ref_texdir;
	nRef<nRoot> ref_fontdir;
    nRef<nRoot> ref_pshaderdir;
	nRef<nRoot> ref_shaderdir;
    nRef<nRoot> ref_vxbufdir;
    nRef<nRoot> ref_ixbufdir;
	nRef<nRoot> ref_cncvdir;

    // matrices
    matrix44 modelview;
    matrix44 view;         
    matrix44 inv_view;
    matrix44 projection;   
    nMatrixStack<matrix44,N_MXSTACKDEPTH> modelview_stack;
    nMatrixStack<matrix44,N_MXSTACKDEPTH> view_stack;
    nMatrixStack<matrix44,N_MXSTACKDEPTH> invview_stack;
    nMatrixStack<matrix44,N_MXSTACKDEPTH> proj_stack;

    // minimize render state changes
    nVertexBubble *current_vertexbubble;
    nPixelShader *current_pixelshader;
    nTexture *current_texture[N_MAXNUM_TEXSTAGES];
   
	// statistics
    nGfxStat stats_triangles;
	nGfxStat stats_lines;
    nGfxStat stats_framerate;
    nGfxStat stats_modelviews;
    nGfxStat stats_lights;
    nGfxStat stats_textures;
    nGfxStat stats_texturesize;
    nGfxStat stats_pshader_switches;
    nGfxStat stats_mesh_switches;
    nGfxStat stats_texture_switches;
    
    // mouse cursor stuff
    bool cursorShown;
    int currentCursor;
    int numCursors;
    nMouseCursor cursor[N_MAXCURSORS];

	// + 19-Mar-2003 autonormalize stuff by Ilya, Clockwise 
	bool autoNormalize;

	unsigned char screen_shots_ask;

	int clear_buffer_bits;

    /// vertical field of view for frustum (in degrees)
	float fov;	
};

// redundant render state handling
inline
void 
nGfxServer::SetCurrentPixelShader(nPixelShader *ps) 
{
    // ps can be NULL
    if (ps != this->current_pixelshader) {
        this->SetStats(N_GFXSTATS_PSHADER_SWITCHES,1);
    }
    this->current_pixelshader = ps;
}

inline
nPixelShader* 
nGfxServer::GetCurrentPixelShader(void) 
{
    return this->current_pixelshader;
}

inline
void 
nGfxServer::SetCurrentVertexBubble(nVertexBubble *vb) 
{
    // mb can be NULL
    if (vb != this->current_vertexbubble) {
        this->SetStats(N_GFXSTATS_MESH_SWITCHES,1);
    }
    this->current_vertexbubble = vb;
}
inline
nVertexBubble*
nGfxServer::GetCurrentVertexBubble(void) 
{
    return this->current_vertexbubble;
}    

inline
void 
nGfxServer::SetCurrentTexture(int stage, nTexture *tex) 
{
    n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
    if (tex)
    {
        int useCount = tex->GetUseCount();
        if (tex != this->current_texture[stage])
        {
            this->SetStats(N_GFXSTATS_TEXTURE_SWITCHES, 1);
            if (0 == useCount)
            {
                this->SetStats(N_GFXSTATS_TEXTURES, 1);
                this->SetStats(N_GFXSTATS_TEXTURESIZE, tex->GetByteSize());
            }
        }        
        // bump use count
        tex->SetUseCount(++useCount);
    }
    this->current_texture[stage] = tex;
}
inline
nTexture* 
nGfxServer::GetCurrentTexture(int stage) 
{n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
    return this->current_texture[stage];
}

inline
void 
nGfxServer::SetOrtho(bool b) 
{
	this->ortho = b;
	this->SetViewVolume(
		this->clip_minx, this->clip_maxx, 
		this->clip_miny, this->clip_maxy, 
		this->clip_minz, this->clip_maxz);
}
inline
bool 
nGfxServer::GetOrtho() const 
{
	return this->ortho;		
}

inline
bool
nGfxServer::SetAA(bool aa) 
{
	if (this->disp_samples)
	{
		this->antialiasing = aa;
		return true;
	}
	else 
	{
		this->antialiasing = false;
		return false;
	}

}
inline
bool 
nGfxServer::GetAA() const
{
	return this->antialiasing;
}
//--------------------------------------------------------------------
#endif
