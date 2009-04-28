#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_render.cc -- Implementiert nGlServer Render-Routinen
//
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------
#include "gfx/nglserver.h"
#include "gfx/ntexture.h"
#include "gfx/nlight.h"

#ifdef _DEBUG
//#define  _DEBUG_AUX
#endif
//-------------------------------------------------------------------
//  _reshapeDisplay()
//  13-Nov-98   floh    created
//  05-May-99   floh    benutzt jetzt die mit SetViewVolume()
//                      definierten Werte
//  20-Jun-99   floh    macht nicht mehr an der Projection-Matrix
//                      rum, das passiert jetzt in SetViewVolume()
//-------------------------------------------------------------------
void nGlServer::_reshapeDisplay(long w, long h)
{
    this->disp_w  = w;
    this->disp_h = h;
    glViewport(0,0,w,h);
}
	
//-------------------------------------------------------------------
//  BeginScene()
//  13-Nov-98   floh    created
//  04-Dec-98   floh    schaltet alle Lichtquellen ab
//  07-Dec-98   floh    + Profiling
//  20-Jan-00   floh    + turn off lights
//  23-Mar-00   floh    + disable fog
//  28-Apr-00   floh    + calls nGfxServer::BeginScene()
//  30-Jul-01   samuel  + added linux window resizing code
//-------------------------------------------------------------------
bool nGlServer::BeginScene(void)
{
    if (nGfxServer::BeginScene()) {
        n_assert(!this->in_begin_scene);
        n_assert(!this->in_begin);

        if (this->display_open && this->render_ok) 
		{
#ifdef __LINUX__
            if (this->resize) 
            {
                this->resize = false;
                XResizeWindow(this->dsp,this->ctx_win,
	                  this->disp_w,
	                  this->disp_h);
                // Choice between geometry sliding off screen or slight gaps
                // at the window borders when resizing quickly.
                // Choosing the latter by ensuring that GLX waits for X to update
                // window size before we update the glViewport
                glXWaitX ();
                this->_reshapeDisplay(this->disp_w, this->disp_h);
            }
#endif		
			_glDBG
			glDepthMask(1); _glDBG
			glColorMask(1, 1, 1, 1); _glDBG
			
			glClearColor(this->clear_color[0],
						 this->clear_color[1],
						 this->clear_color[2],
						 this->clear_color[3]); _glDBG

			glClearStencil(this->stencil_clear); _glDBG			
			glClear(this->gl_clear_buffer_bits);_glDBG

            for (int gl_light = GL_LIGHT0; gl_light<(GL_LIGHT0+nGfxServer::N_MAXLIGHTS); gl_light++) 
            {
                nSetGlCapability(false, (GLenum)gl_light);
            }		

			nSetGlCapability(this->disp_samples && this->GetAA(), GL_MULTISAMPLE_ARB);			

            nSetGlCapability(false, GL_FOG);			
            this->global_fog_enable = false;
            this->in_begin_scene = true;
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------
//  EndScene()
//  13-Nov-98   floh    created
//  07-Dec-98   floh    + Profiling
//  28-Apr-00   floh    + call end scene
//-------------------------------------------------------------------
void nGlServer::EndScene(void)
{
    n_assert(this->in_begin_scene);
    n_assert(!this->in_begin);
    if (this->display_open && this->render_ok) 
	{   		
		glViewport(0, 0, this->disp_w, this->disp_h); _glDBG
#				ifdef __WIN32__
		SwapBuffers(this->hdc); _glDBG
#				elif defined(__LINUX__)
		glXSwapBuffers(this->dsp, this->ctx_win);
#				elif defined(__MACOSX__)
		aglSwapBuffers(ctx);
#				endif
		this->in_begin_scene = false;        
    }
    nGfxServer::EndScene();
}

//-------------------------------------------------------------------
/**
	@brief begin HUD rendering
	BeginHUD()
	start render in screen coords -1 : 1
*/
void nGlServer::BeginHUD(void)
{
	n_assert("previous HUD was not finished" && !this->in_begin_hud);
	this->in_begin_hud = true;
	glMatrixMode(GL_MODELVIEW); _glDBG
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION); _glDBG
	glPushMatrix();
	glLoadIdentity();
}

//-------------------------------------------------------------------
/**
	@brief end HUD rendering
	EndHUD()
	fnish render in screen coords -1 : 1
*/
void nGlServer::EndHUD(void)
{
	n_assert("HUD was not started" && this->in_begin_hud);
	this->in_begin_hud = false;
	glMatrixMode(GL_PROJECTION); _glDBG
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); _glDBG
	glPopMatrix();
}

//-------------------------------------------------------------------
/**
	@brief Set the specified matrix.
	SetMatrix()
	13-Nov-98	floh	created
	14-Dec-98   floh    ruft jetzt nGfxServer::SetMatrix() auf
	28-Apr-00   floh    Statistics: Modelview changes
*/
void nGlServer::SetMatrix(nMatrixMode mode, matrix44& m)
{
    // die matrix44 hat zufaellig das selbe Memory
    // Layout wie die OpenGL Matrizen
    float *glm = &(m.M11);
	switch(mode) {
		case N_MXM_MODELVIEW:
			glMatrixMode(GL_MODELVIEW); _glDBG
			glLoadMatrixf(glm); _glDBG
			break;
		case N_MXM_PROJECTION:	
			glMatrixMode(GL_PROJECTION); _glDBG
			glLoadMatrixf(glm); _glDBG
			break;
		default:
			break;
	}
	nGfxServer::SetMatrix(mode,m);
}

//-------------------------------------------------------------------
/**
	@brief Set the identity matrix.

	- 22-May-2007   Ilya Created
*/
void nGlServer::SetIdentity(nMatrixMode mode)
{	
	switch(mode) {
		case N_MXM_MODELVIEW:
			glMatrixMode(GL_MODELVIEW); _glDBG
			glLoadIdentity();
			break;
		case N_MXM_PROJECTION:	
			glMatrixMode(GL_PROJECTION); _glDBG
			glLoadIdentity();
			break;
		default:
			break;
	}
	nGfxServer::SetIdentity(mode);
}

//-------------------------------------------------------------------
//  PushMatrix()
//  02-May-00   floh    created
//-------------------------------------------------------------------
void nGlServer::PushMatrix(nMatrixMode mode)
{
    switch(mode) {
        case N_MXM_MODELVIEW:
            glMatrixMode(GL_MODELVIEW);_glDBG
            glPushMatrix();
            break;
        case N_MXM_PROJECTION:
            glMatrixMode(GL_PROJECTION);_glDBG
            glPushMatrix();
            break;
        default:
            break;
    }
    nGfxServer::PushMatrix(mode);
}

//-------------------------------------------------------------------
//  PopMatrix()
//  02-May-00   floh    created
//-------------------------------------------------------------------
void nGlServer::PopMatrix(nMatrixMode mode)
{
    switch (mode) {
        case N_MXM_MODELVIEW:
            glMatrixMode(GL_MODELVIEW);_glDBG
            glPopMatrix();
            break;
        case N_MXM_PROJECTION:
            glMatrixMode(GL_PROJECTION);_glDBG
            glPopMatrix();
            break;
        default: 
            break;
    }
    nGfxServer::PopMatrix(mode);
}

//-------------------------------------------------------------------
//  Begin()
//  13-Nov-98   floh    created
//  26-Jan-00   floh    + N_PTYPE_POINT_LIST
//  01-Mar-00   floh    + N_PTYPE_LINE_STRIP
//-------------------------------------------------------------------
void nGlServer::Begin(nPrimType ptype)
{
    n_assert(this->in_begin_scene);    
    glBegin(nGetGlPrimType(ptype));_glDBG
    this->in_begin = TRUE;
}

//-------------------------------------------------------------------
//  End()
//  13-Nov-98   floh    created
//-------------------------------------------------------------------
void nGlServer::End(void)
{
    n_assert(this->in_begin_scene);
    if (this->in_begin) {
		glEnd();_glDBG
		this->in_begin = FALSE;
	}
}

//-------------------------------------------------------------------
//  Coord()
//  13-Nov-98   floh    created
//-------------------------------------------------------------------
void nGlServer::Coord(float x, float y, float z)
{
    glVertex3f(x,y,z);_glDBG
}

//-------------------------------------------------------------------
//  Norm()
//  13-Nov-98   floh    created
//-------------------------------------------------------------------
void nGlServer::Norm(float nx, float ny, float nz)
{
    glNormal3f(nx,ny,nz);_glDBG
}

//-------------------------------------------------------------------
//  Rgba()
//  13-Nov-98   floh    created
//-------------------------------------------------------------------
void nGlServer::Rgba(float r, float g, float b, float a)
{
    glColor4f(r,g,b,a);_glDBG
}

//-------------------------------------------------------------------
//  Uv()
//  13-Nov-98   floh    created
//-------------------------------------------------------------------
void nGlServer::Uv(ulong, float u, float v)
{
    glTexCoord2f(u,v);_glDBG
}

//-------------------------------------------------------------------
//  SetClipPlane()
//  08-Jan-00   floh    created
//-------------------------------------------------------------------
bool nGlServer::SetClipPlane(int i, vector4& v)
{
    double dv[4];
    dv[0] = v.x;
    dv[1] = v.y;
    dv[2] = v.z;
    dv[3] = v.w;
    glClipPlane((GLenum)(GL_CLIP_PLANE0+i),dv);_glDBG
    return true;
}

//-------------------------------------------------------------------
//  SetState()
//  13-Nov-98   floh    created
//  29-Nov-98   floh    N_RS_TEXTUREHANDLE, N_RS_VBUFFERHANDLE,
//                      N_RS_MATERIALHANDLE
//  02-Dec-98   floh    Bugfix bei N_CMP_*
//  13-Dec-98   floh    + N_TBLEND_REPLACE, N_TBLEND_BLEND
//  14-Dec-98   floh    + ruft jetzt die Renderstate-Handler
//                        unter ../hnd auf
//                      + N_RS_AMBIENT, N_RS_DIFFUSE, N_RS_SPECULAR,
//                        N_RS_EMISSION
//  15-Dec-98   floh    + unbekannte Renderstates werden jetzt 
//                        ignoriert
//  08-Jan-99   floh    + VBuffer rendert der GfxServer jetzt
//                        selbst.
//  10-Jan-99   floh    + nMaterial-Objekte rendert der GfxServer jetzt
//                        selbst
//  10-Feb-99   floh    + RenderState-Handler gekillt...
//  20-Jun-99   floh    + N_RS_DONTCLIP
//  21-Jun-99   floh    + ignoriert Specular und Shininess
//  31-Jul-99   floh    + TexGen-Renderstates
//  25-Aug-99   floh    + Glow
//  08-Jan-00   floh    + N_RS_CLIPPLANEENABLE/N_RS_CLIPPLANEDISABLE
//  10-Jan-00   floh    + N_RS_CULLMODE
//  23-Mar-00   floh    + N_RS_FOG*
//  15-May-00   floh    + N_RS_COLORMATERIAL
//-------------------------------------------------------------------
void nGlServer::SetState(nRState& rs)
{
	_glDBG
    // a setstate generally flushes the current pixel shader
    this->SetCurrentPixelShader(NULL);
    switch(rs.rtype) {

        case N_RS_TEXTUREHANDLE:
            {
                // flush current texture
                int i;
                for (i=0; i<N_MAXNUM_TEXSTAGES; i++) {
                    this->SetCurrentTexture(i,NULL);
                };
                if (NULL == rs.rstate.p) nSetGlCapability(false, GL_TEXTURE_2D);
            }
            break;
    
        case N_RS_ZWRITEENABLE:
            if (rs.rstate.r == N_TRUE) glDepthMask(1);
            else                       glDepthMask(0);
            break;

        case N_RS_ZFUNC:			
            glDepthFunc(nGetGlCmp(rs.rstate.r));
            break;

        case N_RS_ZBIAS:
            {
                float f = rs.rstate.f;
                if (f > 0.0f) {
                    glPolygonOffset(0.0f,f);
				}
				nSetGlCapability(f > 0.0f, GL_POLYGON_OFFSET_POINT);
                nSetGlCapability(f > 0.0f, GL_POLYGON_OFFSET_LINE);
                nSetGlCapability(f > 0.0f, GL_POLYGON_OFFSET_FILL);                
            }
            break;

        case N_RS_ALPHABLENDENABLE:
            nSetGlCapability(rs.rstate.r == N_TRUE, GL_BLEND);
            break;

        case N_RS_ALPHASRCBLEND:
            switch (rs.rstate.r) {
                case N_ABLEND_ZERO:         this->alpha_src = GL_ZERO; break;
                case N_ABLEND_ONE:          this->alpha_src = GL_ONE;  break;
                case N_ABLEND_SRCCOLOR:     this->alpha_src = GL_SRC_COLOR; break;
                case N_ABLEND_INVSRCCOLOR:  this->alpha_src = GL_ONE_MINUS_SRC_COLOR; break;
                case N_ABLEND_SRCALPHA:     this->alpha_src = GL_SRC_ALPHA; break;
                case N_ABLEND_INVSRCALPHA:  this->alpha_src = GL_ONE_MINUS_SRC_ALPHA; break;
                case N_ABLEND_DESTALPHA:    this->alpha_src = GL_DST_ALPHA; break;
                case N_ABLEND_INVDESTALPHA: this->alpha_src = GL_ONE_MINUS_DST_ALPHA; break;
                case N_ABLEND_DESTCOLOR:    this->alpha_src = GL_DST_COLOR; break;
                case N_ABLEND_INVDESTCOLOR: this->alpha_src = GL_ONE_MINUS_DST_COLOR; break;
                default:                    this->alpha_src = GL_ZERO; break;
            };
            glBlendFunc(this->alpha_src, this->alpha_dst);
            break;

        case N_RS_ALPHADESTBLEND:
            switch (rs.rstate.r) {
                case N_ABLEND_ZERO:         this->alpha_dst = GL_ZERO; break;
                case N_ABLEND_ONE:          this->alpha_dst = GL_ONE;  break;
                case N_ABLEND_SRCCOLOR:     this->alpha_dst = GL_SRC_COLOR; break;
                case N_ABLEND_INVSRCCOLOR:  this->alpha_dst = GL_ONE_MINUS_SRC_COLOR; break;
                case N_ABLEND_SRCALPHA:     this->alpha_dst = GL_SRC_ALPHA; break;
                case N_ABLEND_INVSRCALPHA:  this->alpha_dst = GL_ONE_MINUS_SRC_ALPHA; break;
                case N_ABLEND_DESTALPHA:    this->alpha_dst = GL_DST_ALPHA; break;
                case N_ABLEND_INVDESTALPHA: this->alpha_dst = GL_ONE_MINUS_DST_ALPHA; break;
                case N_ABLEND_DESTCOLOR:    this->alpha_dst = GL_DST_COLOR; break;
                case N_ABLEND_INVDESTCOLOR: this->alpha_dst = GL_ONE_MINUS_DST_COLOR; break;
                default:                    this->alpha_dst = GL_ZERO; break;
            };
            glBlendFunc(this->alpha_src, this->alpha_dst);
            break;

        case N_RS_LIGHTING:
            nSetGlCapability(rs.rstate.r == N_TRUE, GL_LIGHTING);
            break;

        case N_RS_FOGMODE:
            switch (rs.rstate.r) {
                case N_FOGMODE_OFF:
                    nSetGlCapability(false, GL_FOG);
                    this->global_fog_enable = false;
                    break;
                case N_FOGMODE_LINEAR: 
                    nSetGlCapability(true, GL_FOG);
                    glFogi(GL_FOG_MODE,GL_LINEAR); 
                    this->global_fog_enable = true;
                    break;
                case N_FOGMODE_EXP:
                    nSetGlCapability(true, GL_FOG);
                    glFogi(GL_FOG_MODE,GL_EXP); 
                    this->global_fog_enable = true;
                    break;
                case N_FOGMODE_EXP2: 
                    nSetGlCapability(true, GL_FOG);
                    glFogi(GL_FOG_MODE,GL_EXP2); 
                    this->global_fog_enable = true;
                    break;
                default: break;
            }
            break;
        
        case N_RS_FOGSTART:
            glFogf(GL_FOG_START,rs.rstate.f);
            break;

        case N_RS_FOGEND:
            glFogf(GL_FOG_END,rs.rstate.f);
            break;

        case N_RS_FOGDENSITY:
            glFogf(GL_FOG_DENSITY,rs.rstate.f);
            break;

        case N_RS_FOGCOLOR:
            glFogfv(GL_FOG_COLOR,(const float *)rs.rstate.p);
            break;

        case N_RS_DONTCLIP:
            {
                #ifdef GL_EXT_clip_volume_hint
                if (this->ext_clip_volume_hint) {
                    GLenum h;
                    if (rs.rstate.r == N_TRUE) {
                        h = GL_FASTEST;    // ist das korrekt???
                    } else {
                        h = GL_NICEST;
                    }
                    glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT,h);
                }
                #endif
            }
            break;

        case N_RS_CLIPPLANEENABLE:
            nSetGlCapability(true, (GLenum)(GL_CLIP_PLANE0+rs.rstate.i));
            break;

        case N_RS_CLIPPLANEDISABLE:
            nSetGlCapability(false, (GLenum)(GL_CLIP_PLANE0+rs.rstate.i));
            break;

        case N_RS_CULLMODE:
            switch (rs.rstate.r) 
            {
                case N_CULL_NONE:
                    nSetGlCapability(false, GL_CULL_FACE);
                    break;
                case N_CULL_CW:
                    glFrontFace(GL_CW);
                    nSetGlCapability(true, GL_CULL_FACE);
                    break;
                case N_CULL_CCW:
                    glFrontFace(GL_CCW);
                    nSetGlCapability(true, GL_CULL_FACE);
                    break;
                default: break;
            }
            break;

        case N_RS_NORMALIZENORMALS:
            nSetGlCapability(rs.rstate.r == N_TRUE, GL_NORMALIZE);
            break;

        case N_RS_STENCILENABLE:
			if (this->disp_sbufbits)
				nSetGlCapability(rs.rstate.r == N_TRUE, GL_STENCIL_TEST);
            break;

        case N_RS_STENCILFAIL:
			/*
            switch (rs.rstate.r)
            {
                case N_STENCILOP_KEEP:      this->stencil_fail = GL_KEEP; break;
                case N_STENCILOP_ZERO:      this->stencil_fail = GL_ZERO; break;
                case N_STENCILOP_REPLACE:   this->stencil_fail = GL_REPLACE; break;
                case N_STENCILOP_INVERT:    this->stencil_fail = GL_INVERT; break;
                case N_STENCILOP_INCR:      this->stencil_fail = GL_INCR; break;
                case N_STENCILOP_DECR:      this->stencil_fail = GL_DECR; break;
                default:                    this->stencil_fail = GL_KEEP; break;
            }
			*/
			if (this->disp_sbufbits)
				glStencilOp(this->stencil_fail = nGetGlStencilOp(rs.rstate.r), this->stencil_zfail, this->stencil_pass);
            break;

        case N_RS_STENCILZFAIL:
			/*
            switch (rs.rstate.r)
            {
                case N_STENCILOP_KEEP:      this->stencil_zfail = GL_KEEP; break;
                case N_STENCILOP_ZERO:      this->stencil_zfail = GL_ZERO; break;
                case N_STENCILOP_REPLACE:   this->stencil_zfail = GL_REPLACE; break;
                case N_STENCILOP_INVERT:    this->stencil_zfail = GL_INVERT; break;
                case N_STENCILOP_INCR:      this->stencil_zfail = GL_INCR; break;
                case N_STENCILOP_DECR:      this->stencil_zfail = GL_DECR; break;
                default:                    this->stencil_zfail = GL_KEEP; break;
            }
			*/
            glStencilOp(this->stencil_fail, this->stencil_zfail = nGetGlStencilOp(rs.rstate.r), 
				this->stencil_pass);
            break;

        case N_RS_STENCILPASS:
			/*
            switch (rs.rstate.r)
            {
                case N_STENCILOP_KEEP:      this->stencil_pass = GL_KEEP; break;
                case N_STENCILOP_ZERO:      this->stencil_pass = GL_ZERO; break;
                case N_STENCILOP_REPLACE:   this->stencil_pass = GL_REPLACE; break;
                case N_STENCILOP_INVERT:    this->stencil_pass = GL_INVERT; break;
                case N_STENCILOP_INCR:      this->stencil_pass = GL_INCR; break;
                case N_STENCILOP_DECR:      this->stencil_pass = GL_DECR; break;
                default:                    this->stencil_pass = GL_KEEP; break;
            }
			*/
            glStencilOp(this->stencil_fail, this->stencil_zfail, 
				this->stencil_pass = nGetGlStencilOp(rs.rstate.r));
            break;

        case N_RS_STENCILFUNC:			
            glStencilFunc(this->stencil_func = nGetGlCmp(rs.rstate.r/*, GL_ALWAYS*/), 
				this->stencil_ref, this->stencil_mask);
            break;

        case N_RS_STENCILREF:
            this->stencil_ref = rs.rstate.i;
            glStencilFunc(this->stencil_func, this->stencil_ref, this->stencil_mask);
            break;

        case N_RS_STENCILMASK:
            this->stencil_mask = rs.rstate.i;
            glStencilFunc(this->stencil_func, this->stencil_ref, this->stencil_mask);
            break;

        case N_RS_STENCILCLEAR:
            // clear value for stencil buffer
            this->stencil_clear = rs.rstate.i;
            break;

        case N_RS_COLORWRITEENABLE:
            if (rs.rstate.r == N_TRUE) glColorMask(1, 1, 1, 1);
            else                      glColorMask(0, 0, 0, 0);
            break;

        default: break;
    }
	_glDBG
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
