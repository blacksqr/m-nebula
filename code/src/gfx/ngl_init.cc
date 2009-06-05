#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_init.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "gfx/nglserver.h"

#ifdef _UNICODE
#undef _UNICODE
#endif

#include "gfx/nglext.h"
#include "gfx/nglslprogram.h"

extern char *nGlServer_version;

#ifdef __WIN32__
#define glCullParameterfvEXT this->procCullParameterfvEXT
#endif

//-------------------------------------------------------------------
//  hasExtension()
//  19-Jun-99   floh    aus "Using OpenGL Extensions"
//-------------------------------------------------------------------
bool nGlServer::hasExtension(const char *extName)
{
    return isExtensionSupported(extName);
}

//-------------------------------------------------------------------
//  initExtensions()
//  Fragt Extensions ab und bereitet deren Benutzung vor.
//  19-Jun-99   floh    created
//  20-Jun-99   floh    + EXT_clip_volume_hint
//  21-Jun-99   floh    + EXT_cull_vertex
//  23-Aug-00   floh    - EXT_cull_vertex
//                      + ARB_multitexture
//                      + EXT_texture_env_combine
//  28-Aug-00   floh    + EXT_texture_env_add
//  08-Sep-00   floh    + NV_vertex_array_range
//  02-Oct-00   floh    + Matrox & 3Dfx workaround: multitexturing disabled
//  04-Oct-00   floh    + pixelshader display list generation workaround flag
//                      + EXT_blend_color
//  15-Dec-00   floh    + removed support for NV_vertex_array_range
//  22-Jul-01   floh    + EXT_texture_env_dot3
//  29-Aug-01   floh    + EXT_bgra
//  05-Nov-02   Ilya    + ARB_texture_compression
//  14-Jun-04 	Ilya    + EXT_texture_3d
//-------------------------------------------------------------------
void nGlServer::initExtensions(void)
{
	::initExtensions();
    #ifdef GL_ARB_texture_compression
    if (this->hasExtension("GL_ARB_texture_compression")) 
	{
        if (this->hasExtension("GL_EXT_texture_compression_s3tc")) 
		{
            this->ext_texture_compression = true;
            n_printf("using GL_ARB_texture_compression\n");
            #ifdef __WIN32__
          //  this->glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");
            if (glCompressedTexImage2DARB == NULL) 
			{
                n_printf("failed to obtain proc address for GL_ARB_texture_compression()\n");
                this->ext_texture_compression = false;
            }
            #endif
        } 
		else 
		{
            this->ext_texture_compression = false;
        n_printf("no support for GL_EXT_texture_compression_s3tc\n");
        }
    } 
	else 
	{
        this->ext_texture_compression = false;
        n_printf("no support for GL_ARB_texture_compression\n");
    }
    #else
    #error "GL_ARB_texture_compression or GL_EXT_texture_compression_s3tc not supported by build environment"
    #endif
	#ifdef GL_SGIS_texture_edge_clamp
	if (this->hasExtension("GL_EXT_texture_edge_clamp")) 
	{
        this->ext_clamp_to_edge = true;		
        n_printf("using GL_EXT_texture_edge_clamp\n");
    } 
	else 
	{
        this->ext_clamp_to_edge = false;
        n_printf("no support for GL_EXT_texture_edge_clamp\n");
    }
    #endif


	#ifdef GL_EXT_texture_filter_anisotropic
	if (this->hasExtension("GL_EXT_texture_filter_anisotropic")) 
	{
        this->ext_filter_anisotropic = true;
  		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &this->max_anisotropy);
        n_printf("using GL_EXT_texture_filter_anisotropic\n");
    } 
	else 
	{
        this->ext_filter_anisotropic = false;
        n_printf("no support for GL_EXT_texture_filter_anisotropic\n");
    }
	#else
	#error "GL_EXT_texture_filter_anisotropic not supported by build environment"
	#endif

	#ifdef GL_ARB_texture_non_power_of_two
	if (this->hasExtension("GL_ARB_texture_non_power_of_two")) 
	{
        this->non_power_of2 = true;		
        n_printf("using GL_ARB_texture_non_power_of_two\n");
    } 
	else 
	{
        this->non_power_of2 = false;
        n_printf("no support for GL_ARB_texture_non_power_of_two\n");
    }

	#else
	#error "GL_ARB_texture_non_power_of_two not supported by build environment"
	#endif

    // EXT_compiled_vertex_array
    #ifdef GL_EXT_compiled_vertex_array
    if (glLockArraysEXT && glUnlockArraysEXT) 
	{        
        n_printf("using GL_EXT_compiled_vertex_array\n");        
    } 
	else 
	{        
        n_printf("no support for GL_EXT_compiled_vertex_array\n");
    }
    #else
    #error "GL_EXT_compiled_vertex_array not supported by build environment"
    #endif

    // EXT_clip_volume_hint
    #ifdef GL_EXT_clip_volume_hint
    if (this->hasExtension("GL_EXT_clip_volume_hint")) 
	{
        this->ext_clip_volume_hint = true;
        n_printf("using GL_EXT_clip_volume_hint\n");
    } 
	else 
	{
        this->ext_clip_volume_hint = false;
        n_printf("no support for GL_EXT_clip_volume_hint\n");
    }    
    #else
    #error "GL_EXT_clip_volume_hint not support by build environment"
    #endif

    // ARB_multitexture
    #ifdef GL_ARB_multitexture
    if (glActiveTextureARB && glClientActiveTextureARB) 
	{        
        n_printf("using GL_ARB_multitexture\n");        
    } 
	else 
	{        
        n_printf("no support for GL_ARB_multitexture\n");
    }
    #else
    #error "GL_ARB_multitexture not supported by build environment"
    #endif

    // EXT_texture_env_combine
    #if defined(GL_EXT_texture_env_combine) || defined(GL_ARB_texture_env_combine)
    if (this->hasExtension("GL_EXT_texture_env_combine")) 
	{
        this->ext_texture_env_combine = true;
        n_printf("using GL_EXT_texture_env_combine\n");
    } 
	else if (this->hasExtension("GL_ARB_texture_env_combine")) 
	{
        this->ext_texture_env_combine = true;
        n_printf("using GL_ARB_texture_env_combine\n");
    } 
	else 
	{
        this->ext_texture_env_combine = false;
        n_printf("no support for GL_EXT_texture_env_combine\n");
    }
    #else
    #error "GL_EXT_texture_env_combine not supported by build environment"
    #endif

    // EXT_texture_env_add
    #if defined(GL_EXT_texture_env_add) || defined(GL_ARB_texture_env_add)
    if (this->hasExtension("GL_EXT_texture_env_add")) 
	{
        this->ext_texture_env_add = true;
        n_printf("using GL_EXT_texture_env_add\n");
    } 
	else if (this->hasExtension("GL_ARB_texture_env_add")) 
	{
        this->ext_texture_env_add = true;
        n_printf("using GL_ARB_texture_env_add\n");
    } 
	else 
	{
        this->ext_texture_env_add = false;
        n_printf("no support for GL_EXT_texture_env_add\n");
    }
    #else
    #error "GL_EXT_texture_env_add not supported by build environment"
    #endif

    // EXT_texture_env_dot3
    #if defined(GL_EXT_texture_env_dot3) || defined(GL_ARB_texture_env_dot3)
    if (this->hasExtension("GL_EXT_texture_env_dot3")) 
	{
        this->ext_texture_env_dot3 = true;
        n_printf("using GL_EXT_texture_env_dot3\n");
    } 
	else if (this->hasExtension("GL_ARB_texture_env_dot3")) 
	{
        this->ext_texture_env_dot3 = true;
        n_printf("using GL_ARB_texture_env_dot3\n");
    } 
	else 
	{
        this->ext_texture_env_dot3 = false;
        n_printf("no support for GL_EXT_texture_env_dot3\n");
    }
    #else
    #error "GL_EXT_texture_env_dot3 not supported by build environment"
    #endif

    // EXT_blend_color
    #ifdef GL_EXT_blend_color
    if (this->hasExtension("GL_EXT_blend_color")) 
	{
        this->ext_blend_color = true;
        n_printf("using GL_EXT_blend_color\n");        
    } 
	else 
	{
        this->ext_blend_color = false;
        n_printf("no support for GL_EXT_blend_color\n");
    }
    #else
    #error "GL_EXT_blend_color not supported by build environment"
    #endif

    // EXT_bgra
    #ifdef GL_EXT_bgra
    if (this->hasExtension("GL_EXT_bgra"))
    {
        this->ext_bgra = true;
        n_printf("using GL_EXT_bgra\n");
    }
    else
    {
        this->ext_bgra = false;
        n_printf("no support for GL_EXT_bgra\n");
    }
    #else
    #error "GL_EXT_bgra not support by build environment"
    #endif
}    
//#define VERBOSE_EXTENSIONS
//-------------------------------------------------------------------
//  initGl()
//  Initialisiert frisch erzeugten GL-Kontext.
//  16-Nov-98   floh    created
//  13-May-99   floh    oops, hier war noch glNormalize() angeschaltet 
//  18-Jun-99   floh    + Alphatest
//  18-Jul-99   floh    + /sys/share/display Database cleaner initialisiert
//  23-Aug-00   floh    + query number of multitexture stages
//  08-Sep-00   floh    + initialize vertex pool managers
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//  18-Mar-01   floh    + initialize OpenIL stuff
//-------------------------------------------------------------------
void nGlServer::initGl(void)
{    
    char buf[N_MAXPATH];    
    
	 // fill display database
    kernelServer->PushCwd(this->ref_devdir.get());
    nEnv* env = (nEnv *) kernelServer->New("nenv","name");
    env->SetS((const char *) glGetString(GL_RENDERER));

    env = (nEnv *) kernelServer->New("nenv","desc");
    env->SetS((const char *) glGetString(GL_VERSION));
	const char* ext = (const char *)glGetString(GL_EXTENSIONS);
    env = (nEnv *) kernelServer->New("nenv","extensions");
    env->SetS(ext);
    kernelServer->PopCwd();
    
    // Versions-Info schreiben
    n_printf("gl_vendor:   %s\n", glGetString(GL_VENDOR));
    n_printf("gl_renderer: %s\n", glGetString(GL_RENDERER));
    n_printf("gl_version:  %s\n", glGetString(GL_VERSION));
	n_printf("glsl_version:  %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION_ARB));
	
  	glGetIntegerv(GL_AUX_BUFFERS, &this->aux_buff_num);
	n_printf("number of aux buffers: %d\n", this->aux_buff_num);
	n_printf("number of sample buffers: %d\n", this->disp_samples);
#ifdef VERBOSE_EXTENSIONS
    n_printf("gl_extensions:\n");
    
    int i = 0;
	int cnt = 1;
	int lncnt = 1;
	char c;

    do {
        c = *ext++;
        if ((c == ' ') || (c == 0)) {
            buf[i] = 0;

			if (lncnt > 1)
				n_printf("\t\t\t");


            n_printf("%s", buf);

			if (!(cnt%2)) {
				n_printf("\n");
				lncnt=1;
			} else {
				lncnt++;
			}

			cnt++;
			

            i = 0;
        } else buf[i++] = c;
    } while (c);
#endif
    // initialize extensions
    this->initExtensions();

    // query number of texture units
    #ifdef GL_ARB_multitexture
	if (glActiveTextureARB) {
		GLint num_tus;
  		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&num_tus);
		if (num_tus > N_MAXNUM_TEXSTAGES) {
			n_printf("max texture units: %d\n", num_tus);
			num_tus = N_MAXNUM_TEXSTAGES;
		}
		this->num_texture_units = num_tus;
		
	} else {
		this->num_texture_units = 1;
	}
    #else
        this->num_texture_units = 1;
    #endif

	
  	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->max_texture_dim);
	n_printf("using max texture dimension: %d\n", this->max_texture_dim);
		
    n_printf("using max texture units: %d\n", this->num_texture_units);

	n_printf("max vector uniform components %d\n", nGlSlProgram::maxVertexUniformComponents());

	n_printf("max vertex attribs %d\n",  nGlSlProgram::maxVertexAttribs());

	n_printf("max fragment texture units %d\n",  nGlSlProgram::maxFragmentTextureUnits());

	n_printf("max vertex texture units %d\n",  nGlSlProgram::maxVertexTextureUnits());

	n_printf("max combined texture units %d\n",  nGlSlProgram::maxCombinedTextureUnits());

	n_printf("max max varying floats %d\n",  nGlSlProgram::maxVaryingFloats());

	n_printf("max fragment uniform components %d\n",  nGlSlProgram::maxFragmentUniformComponents());


	int max_samples;
  	glGetIntegerv(GL_SAMPLE_BUFFERS_ARB, &max_samples);

	n_printf("max samples %d\n", max_samples);

   
    // set initial render states
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    nSetGlCapability(true, GL_CULL_FACE);
    glClearDepth(1.0);
    nSetGlCapability(true, GL_DEPTH_TEST);
    nSetGlCapability(false, GL_NORMALIZE);
    nSetGlCapability(false, GL_FOG);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
