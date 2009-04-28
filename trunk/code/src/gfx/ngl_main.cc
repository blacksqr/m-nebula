#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_main.cc -- Main-Modul fuer nGlServer
//
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/nenv.h"
#include "gfx/nglserver.h"
#include "gfx/ntexture.h"
#include "gfx/nvertexpool.h"
#include "gfx/nglvertexpool.h"
#include "gfx/nglfont.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nglslpair.h"
#include "gfx/nglslshader.h"

#if defined(__MACOSX__)
#include <Carbon/carbon.h>
#endif

nNebulaClass(nGlServer, "ngfxserver");

//-------------------------------------------------------------------
//  nGlServer()
//  13-Nov-98   floh    created
//  08-Jan-99   floh    aufgeraeumt
//  13-May-99   floh    erzeugt jetzt auch unter X sofort das Fenster
//  18-Jul-99   floh    Objekt-Referenzen eingefuehrt
//  23-Dec-99   floh    + Win32: this->accel
//  08-Sep-00   floh    + vertex pool manager array to zero
//  31-Jul-01   floh    + stencil buffer stuff
//  28-Jan-2007 Ilya	selection support added
//-------------------------------------------------------------------
nGlServer::nGlServer()	: 
	ref_is(kernelServer, this), ref_devdir(this),
	display_open(false),
	in_begin_scene(false),
	in_begin(false),
	render_ok(false),
	is_fullscreen(false),
	text_initialized(false),
	in_begin_text(false),
	in_begin_hud(false),		
	ext_clip_volume_hint(false),	
	ext_texture_env_combine(false),
	ext_texture_env_add(false),
	ext_texture_env_dot3(false),
	ext_blend_color(false),
	ext_bgra(false),
	ext_texture_compression(false),	
	non_power_of2(false),
	ext_filter_anisotropic(false),
	global_fog_enable(false),
	ext_clamp_to_edge(false),
	gl_clear_buffer_bits(0),
	accel(0)
{	
    this->ref_is = "/sys/servers/input";
    this->ref_devdir = kernelServer->New("nroot","/sys/share/display/0");    
	this->num_texture_units = 1;   
    this->text_listbase    = 0;
    this->text_numglyphs   = 0;
    this->text_height      = 0.0f;
    this->text_x           = 0.0f;
    this->text_y           = 0.0f;
    this->alpha_src     = GL_ONE;
    this->alpha_dst     = GL_ONE;
    this->stencil_fail  = GL_KEEP;
    this->stencil_zfail = GL_KEEP;
    this->stencil_pass  = GL_KEEP;
    this->stencil_func  = GL_ALWAYS;
    this->stencil_ref   = 0;
    this->stencil_mask  = 0xffffffff;
    this->stencil_clear = 0;

    n_memset(this->clear_color,0,sizeof(this->clear_color));
    this->cur_vidmode_id = 0;

#   ifdef __WIN32__
    nEnv *env;
    this->hinst = GetModuleHandle(NULL);
    this->hwnd  = NULL;
    this->hdc   = NULL;
    this->hrc   = NULL;	

    // definiere Accelerators:
    //  Alt-Enter -> toggle fullscreen/windowed
    //  Alt-Plus  -> naechst hoehere Aufloesung
    //  Alt-Minus -> naechst niedrigere Aufloesung
    ACCEL acc[3];
    acc[0].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[0].key   = VK_RETURN;
    acc[0].cmd   = N_GLCMD_TOGGLEFULLSCREEN;

    acc[1].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[1].key   = VK_ADD;
    acc[1].cmd   = N_GLCMD_NEXTMODE;

    acc[2].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[2].key   = VK_SUBTRACT;
    acc[2].cmd   = N_GLCMD_PREVMODE;
    this->accel = CreateAcceleratorTable(acc,3);
    
    this->class_registered = false;
    this->quit_requested = false;
    this->classRegister();
	this->winOpen(); _glDBG
    this->enumDisplayModes();
    if ((env = (nEnv *) kernelServer->New("nenv","/sys/env/hwnd")))  env->SetI((int)this->hwnd);

#   elif defined(__LINUX__)
    this->dsp = NULL;
    this->ctx_win = 0;
    this->wm_win = 0;
    this->fs_win = 0; 
    this->resize = false;
    this->keyboard_grabbed = false;
    this->fs  = NULL;
    n_memset(&(this->event),0,sizeof(this->event));
    n_memset(&(this->cx),0,sizeof(this->cx));
    n_memset(&(this->wmDeleteWindow),0,sizeof(this->wmDeleteWindow));
    this->winOpen();
    this->enumDisplayModes();

#   elif defined(__MACOSX__)
    this->winOpen();
#   endif

    n_memset(this->vpool_manager,0,sizeof(this->vpool_manager));
_glDBG
    // initialize projection matrix
    this->SetViewVolume(this->clip_minx, this->clip_maxx,
                        this->clip_miny, this->clip_maxy,
                        this->clip_minz, this->clip_maxz);
_glDBG
    // Default-Display-Mode
    this->SetDisplayMode("dev(0)-type(win)-w(200)-h(200)");
_glDBG
	this->aux_buff_num = 0;
	this->SetClearBuffer(N_COLOR_BUFFER, true);
	this->SetClearBuffer(N_DEPTH_BUFFER, true);
	this->SetClearBuffer(N_STENCIL_BUFFER, true);
}

//-------------------------------------------------------------------
//  ~nGlServer()
//  13-Nov-98   floh    created
//  13-May-99   floh    killt jetzt auch unter X das Fenster
//  18-Jul-99   floh    raeumt "/sys/share/display" korrekt weg
//  21-Jul-99   floh    CloseDisplay()
//  27-Jul-99   floh    + bevor der OpenGL-Kontext ungueltig wird,
//                      muessen die Textur- und VBuffer-Objekte
//                      gekillt werden!
//  23-Dec-99   floh    + this->accel
//  23-Aug-00   floh    + release pixel shaders (this must happen
//                        before the OpenGL context is destroyed!
//  05-Sep-00   floh    + delete meshes
//-------------------------------------------------------------------
nGlServer::~nGlServer()
{
    n_assert(!this->in_begin_scene);
    n_assert(!this->in_begin);

    this->releaseResources();

    release_ref(this->ref_texdir);
	release_ref(this->ref_fontdir);
	release_ref(this->ref_cncvdir);
	release_ref(this->ref_pshaderdir);
	release_ref(this->ref_shaderdir);
	release_ref(this->ref_vxbufdir);
	release_ref(this->ref_ixbufdir);

    int i;
    for (i=0; i<N_NUM_VBTYPES; i++) {
        if (this->vpool_manager[i]) {
            n_delete this->vpool_manager[i];
            this->vpool_manager[i] = NULL;
        }
    }
    this->CloseDisplay();
#   ifdef __LINUX__
    if (! this->is_fullscreen)
#   endif
        this->winClose();
#   ifdef __WIN32__
    this->classUnregister();
    if (this->accel) DestroyAcceleratorTable(this->accel);
#   endif
	release_ref(ref_devdir);    
}

//-------------------------------------------------------------------
//  SetClearColor()
//  12-Aug-99   floh    created
//-------------------------------------------------------------------
void nGlServer::SetClearColor(float r, float g, float b, float a)
{
    this->clear_color[0] = r;
    this->clear_color[1] = g;
    this->clear_color[2] = b;
    this->clear_color[3] = a;
}

//-------------------------------------------------------------------
//  GetClearColor()
//  12-Aug-99   floh    created
//-------------------------------------------------------------------
void nGlServer::GetClearColor(float& r, float& g, float& b, float& a)
{
    r = this->clear_color[0];
    g = this->clear_color[1];
    b = this->clear_color[2];
    a = this->clear_color[3];
}

//-------------------------------------------------------------------
//  SetViewVolume()
//  05-May-99   floh    created
//  04-Jun-99   floh    + interne Projection-Matrix des GfxServers
//                        wird geupdated...
//-------------------------------------------------------------------
void nGlServer::SetViewVolume(float minx, float maxx, float miny, float maxy,
                              float minz, float maxz)
{
    nGfxServer::SetViewVolume(minx,maxx,miny,maxy,minz,maxz);

    glMatrixMode(GL_PROJECTION); _glDBG
    glLoadIdentity();
	if (this->GetOrtho()) 
	{
		glOrtho(minx,maxx,miny,maxy,minz,maxz); _glDBG
	}
	else
	{
		glFrustum(minx,maxx,miny,maxy,minz,maxz); _glDBG
	}
    glMatrixMode(GL_MODELVIEW); _glDBG

    // update Projection-Matrix des Gfx-Servers...
    matrix44 m;
    glGetFloatv(GL_PROJECTION_MATRIX, (float *)&m);_glDBG
    nGfxServer::SetMatrix(N_MXM_PROJECTION,m);
}

//-------------------------------------------------------------------
/**
	NewTexture()
	@brief Create a new texture object.
	17-Feb-99   floh    created
	16-Jul-99   floh    + ref_texdir
	28-Sep-00   floh    + PushCwd()/PopCwd()
	10-Dec-07   ilya    + NewRsrc usage
*/
nTexture* nGlServer::NewTexture(const char *id)
{
	return (nTexture*)this->NewRsrc(id, "ngltexture", this->ref_texdir.get());    
}

//-------------------------------------------------------------------
/**
	NewFont()
	@brief Create a new OpenGL font object.
	25-May-2007   Ilya    created
	10-Dec-07	  Ilya    + NewRsrc usage
*/
nFont* nGlServer::NewFont(const char *name, nFontType type)
{
	return (nFont*)this->NewRsrc(nFont::GetIDString(name, type), "nglfont", 
		this->ref_fontdir.get());    	
}

//-------------------------------------------------------------------
/**
	NewPixelShader()
	@brief Create a new pixel shader object.
	23-Aug-00   floh    created
	28-Sep-00   floh    + PushCwd()/PopCwd()
	10-Dec-07	  Ilya    + NewRsrc usage
*/
nPixelShader* nGlServer::NewPixelShader(const char *id)
{
	return (nPixelShader*)this->NewRsrc(id, "nglpixelshader", this->ref_pshaderdir.get());
}

//-------------------------------------------------------------------
/**
	NewShaderPair()
	@brief Create a shader program object.
	29-Jul-07	Ilya created
	10-Dec-07	Ilya    + NewRsrc usage
*/
nShaderPair* nGlServer::NewShaderPair(const char* id)
{
	return (nShaderPair*)this->NewRsrc(id, "nglslpair", this->ref_shaderdir.get());	
}

//-------------------------------------------------------------------
/**
	NewShader()
	@brief Create a shader object.
	29-Jul-07	Ilya created
	10-Dec-07	Ilya    + NewRsrc usage
*/
nShader* nGlServer::NewShader(const char* id)
{
	return (nShader*)this->NewRsrc(id, "nglslshader", this->ref_shaderdir.get());		
}

//-------------------------------------------------------------------
//  NewVertexPool()
//  08-Sep-00   floh    created
//-------------------------------------------------------------------
nVertexPool *nGlServer::NewVertexPool(nVertexPoolManager *vpm,
                                      nVBufType vbtype,
                                      int vtype,
                                      int size)
{
    // readonly vertex buffer are never rendered and don't require
    // the overhead of wrapping a gl vertex array around it
    nVertexPool *vp = NULL;
    if (N_VBTYPE_READONLY == vbtype) {
        vp = (nVertexPool *) n_new nVertexPool(this,vpm,vbtype,vtype,size);
    } else {
        vp = (nVertexPool *) n_new nGlVertexPool(this,vpm,vbtype,vtype,size);
    }
    vp->Initialize();
    return vp;
}

//-------------------------------------------------------------------
//  NewVertexBuffer()
//  08-Sep-00   floh    created
//  27-Sep-00   floh    + handle WRITEONLY buffer size
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nVertexBuffer *nGlServer::NewVertexBuffer(const char *id,
                                      nVBufType vbuf_type,
                                      int vertex_type,
                                      int num_v)
{
    nVertexBuffer* vb = (nVertexBuffer*)this->NewRsrc(id, "nvertexbuffer", this->ref_vxbufdir.get());		;
    
    // create a vertex bubble object and initialize vertex buffer object
    n_assert((vbuf_type>=0) && (vbuf_type<N_NUM_VBTYPES));
    
    nVertexPoolManager *vpm = this->vpool_manager[vbuf_type];
    n_assert(vpm);

    // special case:
    if (vbuf_type==N_VBTYPE_WRITEONLY) {
        // D3D handles double buffering for us if we specify
        // DISCARDCONTENTS, so we just return the maximal 
        // size for the vertex buffer
        num_v = vpm->GetPreferedPoolSize();
    }

    nVertexBubble *vbubble = vpm->NewVertexBubble(vertex_type,num_v);
    n_assert(vbubble);

    vb->Initialize(this,vbubble);
    return vb;
}

//-------------------------------------------------------------------
//  GetDisplayDesc()
//  22-Feb-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::GetDisplayDesc(int& x0, int& y0, int& x1, int& y1)
{
    x0 = this->disp_x;
    x1 = this->disp_w;
    y0 = this->disp_y;
    y1 = this->disp_h;
    return true;
}

//-------------------------------------------------------------------
//  GetFontDesc()
//  22-Feb-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::GetFontDesc(int& height)
{
    height = (int) this->text_height;
    return true;
}

//-------------------------------------------------------------------
/**
    Check given display mode against database under
    '/sys/share/display/0/modes', if no matching mode is
    found, select a similar mode.

    24-Dec-99   floh    created
                        + oops, hier wurde ein alter Mode-String
                          hochgegeben...
*/
//-------------------------------------------------------------------
bool nGlServer::SetDisplayMode(const char *mode)
{_glDBG
    bool retval = nGfxServer::SetDisplayMode(mode);
	
    // let superclass desect mode string
    if (retval) {

        int w , h, bpp, zbuf, sbuf;
        bool needs_change = false;
        nRoot *cwd;
        nRoot *mode_dir;

        w   = this->disp_w;
        h   = this->disp_h;
        bpp = this->disp_bpp;
		zbuf = this->disp_zbufbits;
		sbuf = this->disp_sbufbits;
        
        // everything is only relevant for fullscreen
        if ( strcmp("full", this->disp_type) !=0 ) return true;

        // traverse display database for the requested mode,
        // if none is found, adjust w,h,bpp to a close match
        cwd = kernelServer->GetCwd();
        mode_dir = this->ref_devdir->Find("modes");
        if (mode_dir) {
            nRoot *act_mode;
            bool direct_hit = false;
            int best_w   = 640;
            int best_h   = 480;
            int best_bpp = 16;
			int best_zbuf = 16;
			int best_sbuf = 8;                
            int min_dist = 1<<30;
            for (act_mode = mode_dir->GetHead();
                 act_mode;
                 act_mode = act_mode->GetSucc())
            {
                nEnv *env;
                int mw = 0;
                int mh = 0;
                int mbpp = 0;
				int mzbuf = 0;
				int msbuf = 0;
                int dist;
                if ((env = (nEnv *) act_mode->Find("w")))   mw = env->GetI();
                if ((env = (nEnv *) act_mode->Find("h")))   mh = env->GetI();
                if ((env = (nEnv *) act_mode->Find("bpp"))) mbpp = env->GetI();
				if ((env = (nEnv *) act_mode->Find("zbuf"))) mzbuf = env->GetI();
				if ((env = (nEnv *) act_mode->Find("sbuf"))) msbuf = env->GetI();
                // direct hit?
                if ((mw == w) && (mh == h) && (mbpp == bpp) && (mzbuf == zbuf) && (msbuf == sbuf)) {
                    this->cur_vidmode_id = atoi(act_mode->GetName());
                    direct_hit = true;
                    break;
                }
                // none, compute distance to requested mode
                dist = abs(mw-w) + abs(mh-h) + abs(mbpp-bpp) + abs(mzbuf-zbuf) + abs(msbuf - sbuf);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_w   = mw;
                    best_h   = mh;
                    best_bpp = mbpp;
					best_zbuf = zbuf;
					best_sbuf = sbuf;
                    this->cur_vidmode_id = atoi(act_mode->GetName());
                }
            }
            // if no direct hit, adjust with "best fit" parameters
            if (!direct_hit) {
                w   = best_w;
                h   = best_h;
                bpp = best_bpp;
				zbuf = best_zbuf; 
				sbuf = best_sbuf; 
                needs_change = true;
            }
        }
        if (needs_change) {
            char mode_str[256];
			sprintf(mode_str,"dev(0)-type(full)-w(%d)-h(%d)-bpp(%d)-zbuf(%d)-sbuf(%d)",
                    w, h, bpp, zbuf, sbuf);            
            retval = nGfxServer::SetDisplayMode(mode_str);
        }
    }
	_glDBG
    return retval;
}

//-------------------------------------------------------------------
/**
    24-Dec-99   floh    taken over from nd3dserver
    28-Sep-00   floh    + PushCwd()/PopCwd()
    01-Aug-01   samuel  now supported in linux
                        (transferred from ngl_windisp.cc)
*/
//-------------------------------------------------------------------
char *nGlServer::nextDisplayMode(char *buf, int buf_size)
{
    char *retval = NULL;
    nRoot *mode_dir;
    mode_dir = this->ref_devdir->Find("modes");
    if (mode_dir) {
        char mode_name[128];
        nEnv *env;
        sprintf(mode_name,"%d/name",this->cur_vidmode_id+1);
        kernelServer->PushCwd(mode_dir);
        env = (nEnv *) kernelServer->Lookup(mode_name);
        if (!env) {
            sprintf(mode_name,"%d/name",this->cur_vidmode_id);
            env = (nEnv *) kernelServer->Lookup("0/name");
        }
        if ((env = (nEnv *) kernelServer->Lookup(mode_name))) {
            strncpy(buf,env->GetS(),buf_size);
            buf[buf_size-1] = 0;
            retval = buf; 
        }
        kernelServer->PopCwd();
    }
    return retval;
}

//-------------------------------------------------------------------
/**
    24-Dec-99   floh    taken over from nd3dserver
    28-Sep-00   floh    + PushCwd()/PopCwd()
    01-Aug-01   samuel  now supported in linux 
                        (transferred from ngl_windisp.cc)
*/
//-------------------------------------------------------------------
char *nGlServer::prevDisplayMode(char *buf, int buf_size)
{
    char *retval = NULL;
    nRoot *mode_dir;
    mode_dir = this->ref_devdir->Find("modes");
    if (mode_dir) {
        char mode_name[128];
        nEnv *env;
        sprintf(mode_name,"%d/name",this->cur_vidmode_id-1);
        kernelServer->PushCwd(mode_dir);
        env = (nEnv *) kernelServer->Lookup(mode_name);
        if (!env) {
            sprintf(mode_name,"%d/name",this->cur_vidmode_id);
            env = (nEnv *) kernelServer->Lookup("0/name");
        }
        if ((env = (nEnv *) kernelServer->Lookup(mode_name))) {
            strncpy(buf,env->GetS(),buf_size);
            buf[buf_size-1] = 0;
            retval = buf; 
        }
        kernelServer->PopCwd();
    }
    return retval;
}

//-------------------------------------------------------------------
/**
    13-Nov-98   floh    created
    16-Mar-99   floh    + InputServer wird jetzt bei jedem OpenDisplay
                          neu ermittelt
    08-Sep-00   floh    + create vertex pool managers
*/
//-------------------------------------------------------------------
bool nGlServer::OpenDisplay(void)
{
	n_assert(!this->in_begin_scene);
    n_assert(!this->in_begin);
	_glDBG

    // display already open?
    if (this->display_open) {
        n_warn("nGlServer: Display already open!");
        return FALSE;
    }

    // initialize width, height
    bool fullscreen;
    int w   = this->disp_w;
    int h   = this->disp_h;
    int bpp = this->disp_bpp;
	int x	= this->disp_x;
	int y	= this->disp_y;
    if (strcmp((const char *)&(this->disp_type),"full")==0) fullscreen=true;
    else                                                    fullscreen=false;
    this->dispOpen(fullscreen, w, h, bpp, x, y);

    // create vertex pool managers
    this->vpool_manager[N_VBTYPE_READONLY] = n_new nVertexPoolManager(this,N_VBTYPE_READONLY,N_READONLYPOOLSIZE);
    this->vpool_manager[N_VBTYPE_WRITEONLY] = n_new nVertexPoolManager(this,N_VBTYPE_WRITEONLY,N_WRITEONLYPOOLSIZE);
    this->vpool_manager[N_VBTYPE_STATIC] = n_new nVertexPoolManager(this,N_VBTYPE_STATIC,N_STATICPOOLSIZE);

    this->render_ok = true;
	_glDBG
    return true;
}

//-------------------------------------------------------------------
/**
    13-Nov-98   floh    created
    16-Mar-99   floh    + InputServer-Pointer wird invalidiert
    08-Sep-00   floh    + destroy vertex pool managers
*/
//-------------------------------------------------------------------
void nGlServer::CloseDisplay(void)
{
    n_assert(!this->in_begin_scene);
    n_assert(!this->in_begin);
    if (this->display_open) this->dispClose();
}


void 
nGlServer::Resize(int x, int y, int w, int h)
{
#ifdef __WIN32__
	if (this->hwnd)
	{		
		MoveWindow(this->hwnd, x, y, w, h, TRUE);
	}
#endif
}


//-------------------------------------------------------------------
/**
	copying pixels from selection buffer into buf

    @param fname  filename to save the screenshot as.
    
    17-Dec-99   floh    created
*/
bool nGlServer::GetFrameBufferPixels(nFrameBufferType bt, int x, int y, int w, int h, uchar* buf) 
{
	n_assert(buf);	
	glReadBuffer(this->GetFrameBufferType(bt)); _glDBG
	glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buf);_glDBG				
	return glGetError() == GL_NO_ERROR;	
}

//-------------------------------------------------------------------
//  Trigger()
//  20-Jul-99   floh    created
//  21-Jul-99   floh    Message-Loop umstrukturiert. Bisher wurde
//                      nach einem GetMessage() mit Ergebnis WM_QUIT
//                      sofort abgebrochen, jetzt wird zuerst
//                      sichergestellt, dass die Message-Loop
//                      wirklich leer ist!
//  24-Dec-99   floh    + support for accelerators
//-------------------------------------------------------------------
bool nGlServer::Trigger(void)
{
    #ifdef __WIN32__
        
        // alle anstehenden WMs abhandeln, und zurueck
        MSG msg;
        while (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) {
            if (GetMessage(&msg,NULL,0,0)) {
                int msg_handled = false;
				//*#@
                if (this->hwnd && this->accel) {
                    msg_handled = TranslateAccelerator(this->hwnd,this->accel,&msg);
                }
				//*/
                if (!msg_handled) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
        return (!this->quit_requested);

    #elif defined(__LINUX__)
        return this->xwin_proc();
    #elif defined(__MACOSX__)
        EventTargetRef theTarget = GetEventDispatcherTarget();
        EventRef theEvent = 0;
        ReceiveNextEvent( 0, NULL, kEventDurationSecond / 10000, true, &theEvent );
        while ( theEvent != 0 ) {
            SendEventToEventTarget( theEvent, theTarget );
            ReleaseEvent( theEvent );
            ReceiveNextEvent( 0, NULL, kEventDurationSecond / 10000, true,
                              &theEvent );
        }
        return !this->quit_requested;
    #else
        #error nGlServer::Trigger() not implemented
    #endif
}



bool loadGLSLLog(GLhandleARB obj, stl_string& logRef) 
{
	static stl_string log;

	int         logLength     = 0;
	int         charsWritten  = 0;	

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength); _glDBG

	if (nCheckGlError())          // check for OpenGL errors
		return false;

	if(logLength < 1 )
		return false;


	if ((size_t)logLength > log.size())
		log.resize(logLength+1);


	glGetInfoLogARB(obj, logLength, &charsWritten, &log[0]); _glDBG
	log[logLength] = 0;

	logRef += log;

	return !nCheckGlError();	
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
