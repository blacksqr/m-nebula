//-------------------------------------------------------------------
//  ngl_init.cc
//  (C) 2006 I. Kliot based on 
// Simple library to load basic extensions
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, 
// <steps3d@narod.ru>
//-------------------------------------------------------------------
#ifdef  __WIN32__
	#include    <windows.h>
#endif

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <malloc.h>
#include    "gfx/nglext.h"
#include	 "kernel/ntypes.h"

                                // multitexture functions
//#ifdef  __WIN32__
                                // under Linux all multitexture is
                                // supported by OpenGL itself

PFNGLACTIVETEXTUREARBPROC        glActiveTextureARB        = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC  glClientActiveTextureARB  = NULL;
PFNGLMULTITEXCOORD1FARBPROC      glMultiTexCoord1f         = NULL;
PFNGLMULTITEXCOORD1FVARBPROC     glMultiTexCoord1fv        = NULL;
PFNGLMULTITEXCOORD2FARBPROC      glMultiTexCoord2f         = NULL;
PFNGLMULTITEXCOORD2FVARBPROC     glMultiTexCoord2fv        = NULL;
PFNGLMULTITEXCOORD3FARBPROC      glMultiTexCoord3f         = NULL;
PFNGLMULTITEXCOORD3FVARBPROC     glMultiTexCoord3fv        = NULL;
PFNGLMULTITEXCOORD4FARBPROC      glMultiTexCoord4f         = NULL;
PFNGLMULTITEXCOORD4FVARBPROC     glMultiTexCoord4fv        = NULL;
//#endif

                                        // fog coord function
PFNGLFOGCOORDFEXTPROC            glFogCoordf               = NULL;

                                        // secondary color functions
PFNGLSECONDARYCOLOR3FPROC        glSecondaryColor3f        = NULL;
PFNGLSECONDARYCOLOR3FVPROC       glSecondaryColor3fv       = NULL;
PFNGLSECONDARYCOLORPOINTERPROC   glSecondaryColorPointer   = NULL;


                                        // register combiners functions
PFNGLCOMBINERPARAMETERFVNVPROC   glCombinerParameterfvNV   = NULL;
PFNGLCOMBINERPARAMETERIVNVPROC   glCombinerParameterivNV   = NULL;
PFNGLCOMBINERPARAMETERFNVPROC    glCombinerParameterfNV    = NULL;
PFNGLCOMBINERPARAMETERINVPROC    glCombinerParameteriNV    = NULL;
PFNGLCOMBINERINPUTNVPROC         glCombinerInputNV         = NULL;
PFNGLCOMBINEROUTPUTNVPROC        glCombinerOutputNV        = NULL;
PFNGLFINALCOMBINERINPUTNVPROC    glFinalCombinerInputNV    = NULL;

                                    // VBO functions
PFNGLBINDBUFFERARBPROC           glBindBufferARB           = NULL;
PFNGLDELETEBUFFERSARBPROC        glDeleteBuffersARB        = NULL;
PFNGLGENBUFFERSARBPROC           glGenBuffersARB           = NULL;
PFNGLISBUFFERARBPROC             glIsBufferARB             = NULL;
PFNGLBUFFERDATAARBPROC           glBufferDataARB           = NULL;
PFNGLBUFFERSUBDATAARBPROC        glBufferSubDataARB        = NULL;
PFNGLGETBUFFERSUBDATAARBPROC     glGetBufferSubDataARB     = NULL;
PFNGLMAPBUFFERARBPROC            glMapBufferARB            = NULL;
PFNGLUNMAPBUFFERARBPROC          glUnmapBufferARB          = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;
PFNGLGETBUFFERPOINTERVARBPROC    glGetBufferPointervARB    = NULL;


#ifdef  __WIN32__
PFNWGLGETEXTENSIONSSTRINGARBPROC        wglGetExtensionsStringARB = NULL;

                                    // p-buffer functions
PFNWGLCREATEPBUFFERARBPROC              wglCreatePbufferARB     = NULL;
PFNWGLGETPBUFFERDCARBPROC               wglGetPbufferDCARB      = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC           wglReleasePbufferDCARB  = NULL;
PFNWGLDESTROYPBUFFERARBPROC             wglDestroyPbufferARB    = NULL;
PFNWGLQUERYPBUFFERARBPROC               wglQueryPbufferARB      = NULL;

                                    // WGL_ARB_pixel_format
PFNWGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB		 = NULL;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC		wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC		wglGetPixelFormatAttribfvARB = NULL;

                                    // WGL_ARB_render_texture
PFNWGLBINDTEXIMAGEARBPROC               wglBindTexImageARB      = NULL;
PFNWGLRELEASETEXIMAGEARBPROC            wglReleaseTexImageARB   = NULL;
PFNWGLSETPBUFFERATTRIBARBPROC           wglSetPbufferAttribARB  = NULL;

                                    // WGL_EXT_swap_control
PFNWGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT      = NULL;
PFNWGLGETSWAPINTERVALEXTPROC            wglGetSwapIntervalEXT   = NULL;
#else
                                    // GLX_SGIX_pbuffer
PFNGLXCREATEGLXPBUFFERSGIXPROC          glXCreateGLXPbufferSGIX  = NULL;
PFNGLXDESTROYGLXPBUFFERSGIXPROC         glXDestroyGLXPbufferSGIX = NULL;
PFNGLXQUERYGLXPBUFFERSGIXPROC           glXQueryGLXPbufferSGIX   = NULL;

                                    // GLX_SGIX_fbconfig
PFNGLXCHOOSEFBCONFIGSGIXPROC            glXChooseFBConfigSGIX          = NULL;
PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC   glXCreateContextWithConfigSGIX = NULL;
#endif

                                    // NV-occlusion query extension
PFNGLGENOCCLUSIONQUERIESNVPROC          glGenOcclusionQueriesNV = NULL;
PFNGLBEGINOCCLUSIONQUERYNVPROC          glBeginOcclusionQueryNV = NULL;
PFNGLENDOCCLUSIONQUERYNVPROC            glEndOcclusionQueryNV   = NULL;
PFNGLGETOCCLUSIONQUERYIVNVPROC          glGetOcclusionQueryivNV = NULL;

                                    // ARB_vertex_program extension
PFNGLGENPROGRAMSARBPROC                 glGenProgramsARB        = NULL;
PFNGLDELETEPROGRAMSARBPROC              glDeleteProgramsARB     = NULL;
PFNGLBINDPROGRAMARBPROC                 glBindProgramARB        = NULL;
PFNGLISPROGRAMARBPROC                   glIsProgramARB          = NULL;

PFNGLPROGRAMSTRINGARBPROC               glProgramStringARB      = NULL;
PFNGLGETPROGRAMIVARBPROC                glGetProgramivARB       = NULL;

PFNGLVERTEXATTRIB4FARBPROC              glVertexAttrib4fARB     = NULL;
PFNGLVERTEXATTRIB4FVARBPROC             glVertexAttrib4fvARB    = NULL;
PFNGLVERTEXATTRIB3FARBPROC              glVertexAttrib3fARB     = NULL;
PFNGLVERTEXATTRIB3FVARBPROC             glVertexAttrib3fvARB    = NULL;

PFNGLVERTEXATTRIBPOINTERARBPROC         glVertexAttribPointerARB        = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC     glEnableVertexAttribArrayARB    = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC    glDisableVertexAttribArrayARB   = NULL;

PFNGLPROGRAMLOCALPARAMETER4FARBPROC     glProgramLocalParameter4fARB    = NULL;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC    glProgramLocalParameter4fvARB   = NULL;

PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC  glGetProgramLocalParameterfvARB = NULL;

PFNGLPROGRAMENVPARAMETER4FARBPROC       glProgramEnvParameter4fARB      = NULL;
PFNGLPROGRAMENVPARAMETER4FVARBPROC      glProgramEnvParameter4fvARB     = NULL;

PFNGLGETPROGRAMENVPARAMETERFVARBPROC    glGetProgramEnvParameterfvARB   = NULL;

                                    // GL_EXT_texture3D
PFNGLTEXIMAGE3DEXTPROC                  glTexImage3DEXT                  = NULL;
PFNGLTEXSUBIMAGE3DEXTPROC				glTexSubImage3DEXT				 = NULL;

                                    // ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE3DARBPROC        glCompressedTexImage3DARB        = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC        glCompressedTexImage2DARB        = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DARBPROC        glCompressedTexImage1DARB        = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC     glCompressedTexSubImage3DARB     = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC     glCompressedTexSubImage2DARB     = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC     glCompressedTexSubImage1DARB     = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC       glGetCompressedTexImageARB       = NULL;

                                    // ARB_point_parameters
PFNGLPOINTPARAMETERFARBPROC             glPointParameterfARB             = NULL;
PFNGLPOINTPARAMETERFVARBPROC            glPointParameterfvARB            = NULL;

                                    // ARB_occlusion_query
PFNGLGENQUERIESARBPROC                  glGenQueriesARB        = NULL;
PFNGLDELETEQUERIESARBPROC               glDeleteQueriesARB     = NULL;
PFNGLISQUERYARBPROC                     glIsQueryARB           = NULL;
PFNGLBEGINQUERYARBPROC                  glBeginQueryARB        = NULL;
PFNGLENDQUERYARBPROC                    glEndQueryARB          = NULL;
PFNGLGETQUERYIVARBPROC                  glGetQueryivARB        = NULL;
PFNGLGETQUERYOBJECTIVARBPROC            glGetQueryObjectivARB  = NULL;
PFNGLGETQUERYOBJECTUIVARBPROC           glGetQueryObjectuivARB = NULL;

                                    // GLSL-specific extensions
                                    // GL_ARB_shader_object
PFNGLDELETEOBJECTARBPROC                glDeleteObjectARB         = NULL;
PFNGLGETHANDLEARBPROC                   glGetHandleARB            = NULL;
PFNGLDETACHOBJECTARBPROC                glDetachObjectARB         = NULL;
PFNGLCREATESHADEROBJECTARBPROC          glCreateShaderObjectARB   = NULL;
PFNGLSHADERSOURCEARBPROC                glShaderSourceARB         = NULL;
PFNGLCOMPILESHADERARBPROC               glCompileShaderARB        = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC         glCreateProgramObjectARB  = NULL;
PFNGLATTACHOBJECTARBPROC                glAttachObjectARB         = NULL;
PFNGLLINKPROGRAMARBPROC                 glLinkProgramARB          = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC            glUseProgramObjectARB     = NULL;
PFNGLVALIDATEPROGRAMARBPROC             glValidateProgramARB      = NULL;
PFNGLUNIFORM1FARBPROC                   glUniform1fARB            = NULL;
PFNGLUNIFORM2FARBPROC                   glUniform2fARB            = NULL;
PFNGLUNIFORM3FARBPROC                   glUniform3fARB            = NULL;
PFNGLUNIFORM4FARBPROC                   glUniform4fARB            = NULL;
PFNGLUNIFORM1IARBPROC                   glUniform1iARB            = NULL;
PFNGLUNIFORM2IARBPROC                   glUniform2iARB            = NULL;
PFNGLUNIFORM3IARBPROC                   glUniform3iARB            = NULL;
PFNGLUNIFORM4IARBPROC                   glUniform4iARB            = NULL;
PFNGLUNIFORM1FVARBPROC                  glUniform1fvARB           = NULL;
PFNGLUNIFORM2FVARBPROC                  glUniform2fvARB           = NULL;
PFNGLUNIFORM3FVARBPROC                  glUniform3fvARB           = NULL;
PFNGLUNIFORM4FVARBPROC                  glUniform4fvARB           = NULL;
PFNGLUNIFORM1IVARBPROC                  glUniform1ivARB           = NULL;
PFNGLUNIFORM2IVARBPROC                  glUniform2ivARB           = NULL;
PFNGLUNIFORM3IVARBPROC                  glUniform3ivARB           = NULL;
PFNGLUNIFORM4IVARBPROC                  glUniform4ivARB           = NULL;
PFNGLUNIFORMMATRIX2FVARBPROC            glUniformMatrix2fvARB     = NULL;
PFNGLUNIFORMMATRIX3FVARBPROC            glUniformMatrix3fvARB     = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC            glUniformMatrix4fvARB     = NULL;
PFNGLGETOBJECTPARAMETERFVARBPROC        glGetObjectParameterfvARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC        glGetObjectParameterivARB = NULL;
PFNGLGETINFOLOGARBPROC                  glGetInfoLogARB           = NULL;
PFNGLGETATTACHEDOBJECTSARBPROC          glGetAttachedObjectsARB   = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC          glGetUniformLocationARB   = NULL;
PFNGLGETACTIVEUNIFORMARBPROC            glGetActiveUniformARB     = NULL;
PFNGLGETUNIFORMFVARBPROC                glGetUniformfvARB         = NULL;
PFNGLGETUNIFORMIVARBPROC                glGetUniformivARB         = NULL;
PFNGLGETSHADERSOURCEARBPROC             glGetShaderSourceARB      = NULL;

                                    // GL_ARB_vertex_shader
PFNGLBINDATTRIBLOCATIONARBPROC          glBindAttribLocationARB   = NULL;
PFNGLGETACTIVEATTRIBARBPROC             glGetActiveAttribARB      = NULL;
PFNGLGETATTRIBLOCATIONARBPROC           glGetAttribLocationARB    = NULL;
PFNGLGETVERTEXATTRIBFVARBPROC           glGetVertexAttribfvARB    = NULL;

                                    // EXT_stencil_two_side
PFNGLACTIVESTENCILFACEEXTPROC           glActiveStencilFaceEXT    = NULL;

				    				// EXT_depth_bounds_test
PFNGLDEPTHBOUNDSEXTPROC			glDepthBoundsEXT                  = NULL;

									// EXT_framebuffer_object
PFNGLISRENDERBUFFEREXTPROC						glIsRenderbufferEXT                      = NULL;
PFNGLBINDRENDERBUFFEREXTPROC					glBindRenderbufferEXT                    = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC					glDeleteRenderbuffersEXT                 = NULL;
PFNGLGENRENDERBUFFERSEXTPROC					glGenRenderbuffersEXT                    = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC					glRenderbufferStorageEXT                 = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			glGetRenderbufferParameterivEXT          = NULL;
PFNGLISFRAMEBUFFEREXTPROC						glIsFramebufferEXT                       = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC						glBindFramebufferEXT                     = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC					glDeleteFramebuffersEXT                  = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC						glGenFramebuffersEXT                     = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				glCheckFramebufferStatusEXT              = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC				glFramebufferTexture1DEXT                = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC				glFramebufferTexture2DEXT                = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC				glFramebufferTexture3DEXT                = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				glFramebufferRenderbufferEXT             = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC	glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC						glGenerateMipmapEXT                      = NULL;
PFNGLBLENDCOLOREXTPROC								glBlendColorEXT = NULL;
PFNGLLOCKARRAYSEXTPROC								glLockArraysEXT = NULL;
PFNGLUNLOCKARRAYSEXTPROC							glUnlockArraysEXT = NULL;


#ifndef	GL_VERSION_1_3										// ARB_transpose matrix
PFNGLLOADTRANSPOSEMATRIXFPROC					glLoadTransposeMatrixfARB                = NULL;
PFNGLLOADTRANSPOSEMATRIXDPROC					glLoadTransposeMatrixdARB                = NULL;
PFNGLMULTTRANSPOSEMATRIXFPROC					glMultTransposeMatrixfARB                = NULL;
PFNGLMULTTRANSPOSEMATRIXDPROC					glMultTransposeMatrixdARB                = NULL;
#endif


////////////////////////// functions ////////////////////////////

static  bool    isExtensionSupported ( const char * ext, const char * extList )
{
    const char * start = extList;
    const char * ptr;

    while ( ( ptr = strstr ( start, ext ) ) != NULL )
    {
                                            // we've found, ensure name is exactly ext
        const char * end = ptr + strlen ( ext );

        if ( isspace ( *end ) || *end == '\0' )
            return true;

        start = end;
    }

    return false;
}

bool    isExtensionSupported ( const char * ext )
{
    initExtensions ();                      // since we will need it for platform-specific extensions

    const char * extensions = (const char *) glGetString ( GL_EXTENSIONS );

    if ( isExtensionSupported ( ext, extensions ) )
	    return true;

#ifdef  __WIN32__                              // check Windoze extensions
    if ( wglGetExtensionsStringARB == NULL )
    	return false;

    return isExtensionSupported ( ext, wglGetExtensionsStringARB ( wglGetCurrentDC () ) );
#else                                       // check GLX extensions
    Display * display = glXGetCurrentDisplay ();
    int       screen  = DefaultScreen        ( display );

    return isExtensionSupported ( ext, glXQueryExtensionsString ( display, screen ) );
#endif
}

const char * getGeneralExtensions ()
{
    initExtensions ();

	return (const char *) glGetString ( GL_EXTENSIONS );
}

const char * getPlatformExtensions ()
{
    initExtensions ();                      // since we will need it for platform-specific extensions

#ifdef  __WIN32__                              // check Windoze extensions
    if ( wglGetExtensionsStringARB == NULL )
    	return "";

    return wglGetExtensionsStringARB ( wglGetCurrentDC () );
#else                                       // check GLX extensions
    Display * display = glXGetCurrentDisplay ();
    int       screen  = DefaultScreen        ( display );

    return glXQueryExtensionsString ( display, screen );
#endif
}

void    assertExtensionsSupported ( const char * extList )
{
    char * str = _strdup ( extList );

    for ( char * ptr = strtok ( str, " ,;\t" ); ptr != NULL; ptr = strtok ( NULL, " ,;\t" ) )
        if ( !isExtensionSupported ( ptr ) )
        {
            fprintf ( stderr, "Error - extension %s not supported.\n", ptr );

            free ( str );
            exit ( 1   );
        }

    free ( str );
}

//
// hide platform specific in one place
//

static void * getProcAddress ( const char * name )
{
	static int proc_cnt = 0;
	static int ln_proc_cnt = 0;
	void * func =  0 ;
#ifdef  __WIN32__
    func = wglGetProcAddress ( name );
#else
    func = (void *)glXGetProcAddressARB ( (const GLubyte *)name );
#endif

	if (!proc_cnt)
		n_printf("functions acquire:\n");

	if (ln_proc_cnt > 1)
		n_printf("\t\t");

	if (func)
		n_printf("%s", name);
	else 
		n_printf("\nno support for %s\n", name);

	if (!(proc_cnt%2)) {
		n_printf("\n");
		ln_proc_cnt=1;
	} else {
		ln_proc_cnt++;
	}

	proc_cnt++;
	return func;
}

#ifdef  __WIN32__
bool    initWin32Extensions ()
{
    wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) getProcAddress ( "wglGetExtensionsStringARB" );

    if ( wglGetExtensionsStringARB == NULL )
        return false;

    const char * exts = wglGetExtensionsStringARB ( wglGetCurrentDC () );

    if ( exts == NULL )
        return false;

    if ( strstr ( exts, "WGL_ARB_pbuffer" ) != NULL )
    {
        wglCreatePbufferARB    = (PFNWGLCREATEPBUFFERARBPROC)     getProcAddress ( "wglCreatePbufferARB"    );
        wglGetPbufferDCARB     = (PFNWGLGETPBUFFERDCARBPROC)      getProcAddress ( "wglGetPbufferDCARB"     );
        wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)  getProcAddress ( "wglReleasePbufferDCARB" );
        wglDestroyPbufferARB   = (PFNWGLDESTROYPBUFFERARBPROC)    getProcAddress ( "wglDestroyPbufferARB"   );
        wglQueryPbufferARB     = (PFNWGLQUERYPBUFFERARBPROC)      getProcAddress ( "wglQueryPbufferARB"     );
    }

    if ( strstr ( exts, "WGL_ARB_pixel_format" ) != NULL )
	{
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)getProcAddress ( "wglChoosePixelFormatARB" );
		wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)getProcAddress ( "wglGetPixelFormatAttribivARB" );
		wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)getProcAddress ( "wglGetPixelFormatAttribfvARB" );
	}

    if ( strstr ( exts, "WGL_ARB_render_texture" ) != NULL )
    {
        wglBindTexImageARB      = (PFNWGLBINDTEXIMAGEARBPROC)     getProcAddress ( "wglBindTexImageARB"     );
        wglReleaseTexImageARB   = (PFNWGLRELEASETEXIMAGEARBPROC)  getProcAddress ( "wglReleaseTexImageARB"  );
        wglSetPbufferAttribARB  = (PFNWGLSETPBUFFERATTRIBARBPROC) getProcAddress ( "wglSetPbufferAttribARB" );
    }

    if ( strstr ( exts, "WGL_EXT_swap_control" ) != NULL )
    {
        wglSwapIntervalEXT      = (PFNWGLSWAPINTERVALEXTPROC)       getProcAddress ( "wglSwapIntervalEXT"    );
        wglGetSwapIntervalEXT   = (PFNWGLGETSWAPINTERVALEXTPROC)    getProcAddress ( "wglGetSwapIntervalEXT" );
    }

    return true;
}
#else
void    initLinuxExtensions ()
{
                                    // GLX_SGIX_pbuffer
    glXCreateGLXPbufferSGIX  = (PFNGLXCREATEGLXPBUFFERSGIXPROC)  getProcAddress ( "glXCreateGLXPbufferSGIX" );
    glXDestroyGLXPbufferSGIX = (PFNGLXDESTROYGLXPBUFFERSGIXPROC) getProcAddress ( "glXDestroyGLXPbufferSGIX" );
    glXQueryGLXPbufferSGIX   = (PFNGLXQUERYGLXPBUFFERSGIXPROC)   getProcAddress ( "glXQueryGLXPbufferSGIX" );

                                    // GLX_SGIX_fbconfig
    glXChooseFBConfigSGIX          = (PFNGLXCHOOSEFBCONFIGSGIXPROC)          getProcAddress ( "glXChooseFBConfigSGIX" );
    glXCreateContextWithConfigSGIX = (PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC) getProcAddress ( "glXCreateContextWithConfigSGIX" );
}
#endif

void    initExtensions ()
{
    static  bool    initialized = false;                    // whether initExtensions has already been called

    if ( initialized )
        return;

//#ifdef  __WIN32__
    glActiveTextureARB       = (PFNGLACTIVETEXTUREARBPROC)         getProcAddress ( "glActiveTextureARB"       );
    glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)   getProcAddress ( "glClientActiveTextureARB" );
    glMultiTexCoord1f        = (PFNGLMULTITEXCOORD1FARBPROC)       getProcAddress ( "glMultiTexCoord1fARB"     );
    glMultiTexCoord1fv       = (PFNGLMULTITEXCOORD1FVARBPROC)      getProcAddress ( "glMultiTexCoord1fvARB"    );
    glMultiTexCoord2f        = (PFNGLMULTITEXCOORD2FARBPROC)       getProcAddress ( "glMultiTexCoord2fARB"     );
    glMultiTexCoord2fv       = (PFNGLMULTITEXCOORD2FVARBPROC)      getProcAddress ( "glMultiTexCoord2fvARB"    );
    glMultiTexCoord3f        = (PFNGLMULTITEXCOORD3FARBPROC)       getProcAddress ( "glMultiTexCoord3fARB"     );
    glMultiTexCoord3fv       = (PFNGLMULTITEXCOORD3FVARBPROC)      getProcAddress ( "glMultiTexCoord3fvARB"    );
    glMultiTexCoord4f        = (PFNGLMULTITEXCOORD4FARBPROC)       getProcAddress ( "glMultiTexCoord4fARB"     );
    glMultiTexCoord4fv       = (PFNGLMULTITEXCOORD4FVARBPROC)      getProcAddress ( "glMultiTexCoord4fvARB"    );
//#endif

    glFogCoordf              = (PFNGLFOGCOORDFEXTPROC)             getProcAddress ( "glFogCoordfEXT"         );

    glSecondaryColor3f       = (PFNGLSECONDARYCOLOR3FPROC)         getProcAddress ( "glSecondaryColor3fEXT"      );
    glSecondaryColor3fv      = (PFNGLSECONDARYCOLOR3FVPROC)        getProcAddress ( "glSecondaryColor3fvEXT"     );
    glSecondaryColorPointer  = (PFNGLSECONDARYCOLORPOINTERPROC)    getProcAddress ( "glSecondaryColorPointerEXT" );

    glCombinerParameterfvNV  = (PFNGLCOMBINERPARAMETERFVNVPROC)    getProcAddress ( "glCombinerParameterfvNV"  );
    glCombinerParameterivNV  = (PFNGLCOMBINERPARAMETERIVNVPROC)    getProcAddress ( "glCombinerParameterivNV"  );
    glCombinerParameterfNV   = (PFNGLCOMBINERPARAMETERFNVPROC)     getProcAddress ( "glCombinerParameterfNV"   );
    glCombinerParameteriNV   = (PFNGLCOMBINERPARAMETERINVPROC)     getProcAddress ( "glCombinerParameteriNV"   );
    glCombinerInputNV        = (PFNGLCOMBINERINPUTNVPROC)          getProcAddress ( "glCombinerInputNV"        );
    glCombinerOutputNV       = (PFNGLCOMBINEROUTPUTNVPROC)         getProcAddress ( "glCombinerOutputNV"       );
    glFinalCombinerInputNV   = (PFNGLFINALCOMBINERINPUTNVPROC)     getProcAddress ( "glFinalCombinerInputNV"   );

    glBindBufferARB           = (PFNGLBINDBUFFERARBPROC)           getProcAddress ( "glBindBufferARB"           );
    glDeleteBuffersARB        = (PFNGLDELETEBUFFERSARBPROC)        getProcAddress ( "glDeleteBuffersARB"        );
    glGenBuffersARB           = (PFNGLGENBUFFERSARBPROC)           getProcAddress ( "glGenBuffersARB"           );
    glIsBufferARB             = (PFNGLISBUFFERARBPROC)             getProcAddress ( "glIsBufferARB"             );
    glBufferDataARB           = (PFNGLBUFFERDATAARBPROC)           getProcAddress ( "glBufferDataARB"           );
    glBufferSubDataARB        = (PFNGLBUFFERSUBDATAARBPROC)        getProcAddress ( "glBufferSubDataARB"        );
    glGetBufferSubDataARB     = (PFNGLGETBUFFERSUBDATAARBPROC)     getProcAddress ( "glGetBufferSubDataARB"     );
    glMapBufferARB            = (PFNGLMAPBUFFERARBPROC)            getProcAddress ( "glMapBufferARB"            );
    glUnmapBufferARB          = (PFNGLUNMAPBUFFERARBPROC)          getProcAddress ( "glUnmapBufferARB"          );
    glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC) getProcAddress ( "glGetBufferParameterivARB" );
    glGetBufferPointervARB    = (PFNGLGETBUFFERPOINTERVARBPROC)    getProcAddress ( "glGetBufferPointervARB"    );

                                    // NV-occlusion query extension
    glGenOcclusionQueriesNV = (PFNGLGENOCCLUSIONQUERIESNVPROC) getProcAddress ( "glGenOcclusionQueriesNV" );
    glBeginOcclusionQueryNV = (PFNGLBEGINOCCLUSIONQUERYNVPROC) getProcAddress ( "glBeginOcclusionQueryNV" );
    glEndOcclusionQueryNV   = (PFNGLENDOCCLUSIONQUERYNVPROC)   getProcAddress ( "glEndOcclusionQueryNV"   );
    glGetOcclusionQueryivNV = (PFNGLGETOCCLUSIONQUERYIVNVPROC) getProcAddress ( "glGetOcclusionQueryivNV" );

                                    // ARB_vertex_program extension
    glGenProgramsARB                = (PFNGLGENPROGRAMSARBPROC)               getProcAddress ( "glGenProgramsARB" );
    glDeleteProgramsARB             = (PFNGLDELETEPROGRAMSARBPROC)            getProcAddress ( "glDeleteProgramsARB" );
    glBindProgramARB                = (PFNGLBINDPROGRAMARBPROC)               getProcAddress ( "glBindProgramARB" );
    glIsProgramARB                  = (PFNGLISPROGRAMARBPROC)                 getProcAddress ( "glIsProgramARB" );
    glProgramStringARB              = (PFNGLPROGRAMSTRINGARBPROC)             getProcAddress ( "glProgramStringARB" );
    glGetProgramivARB               = (PFNGLGETPROGRAMIVARBPROC)              getProcAddress ( "glGetProgramivARB" );
    glVertexAttrib4fARB             = (PFNGLVERTEXATTRIB4FARBPROC)            getProcAddress ( "glVertexAttrib4fARB" );
    glVertexAttrib4fvARB            = (PFNGLVERTEXATTRIB4FVARBPROC)           getProcAddress ( "glVertexAttrib4fvARB" );
    glVertexAttrib3fARB             = (PFNGLVERTEXATTRIB3FARBPROC)            getProcAddress ( "glVertexAttrib3fARB" );
    glVertexAttrib3fvARB            = (PFNGLVERTEXATTRIB3FVARBPROC)           getProcAddress ( "glVertexAttrib3fvARB" );
    glVertexAttribPointerARB        = (PFNGLVERTEXATTRIBPOINTERARBPROC)       getProcAddress ( "glVertexAttribPointerARB" );
    glEnableVertexAttribArrayARB    = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)   getProcAddress ( "glEnableVertexAttribArrayARB" );
    glDisableVertexAttribArrayARB   = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)  getProcAddress ( "glDisableVertexAttribArrayARB" );
    glProgramLocalParameter4fARB    = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)   getProcAddress ( "glProgramLocalParameter4fARB" );
    glProgramLocalParameter4fvARB   = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)  getProcAddress ( "glProgramLocalParameter4fvARB" );
    glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)getProcAddress ( "glGetProgramLocalParameterfvARB" );
    glProgramEnvParameter4fARB      = (PFNGLPROGRAMENVPARAMETER4FARBPROC)     getProcAddress ( "glProgramEnvParameter4fARB" );
    glProgramEnvParameter4fvARB     = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)    getProcAddress ( "glProgramEnvParameter4fvARB" );
    glGetProgramEnvParameterfvARB   = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)  getProcAddress ( "glGetProgramEnvParameterfvARB" );

                                    // GL_EXT_texture3D
    glTexImage3DEXT                 =(PFNGLTEXIMAGE3DEXTPROC)                 getProcAddress ( "glTexImage3DEXT" );
	glTexSubImage3DEXT              =(PFNGLTEXSUBIMAGE3DEXTPROC)              getProcAddress ( "glTexSubImage3DEXT" );

                                    // ARB_texture_compression
    glCompressedTexImage3DARB       = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)      getProcAddress ( "glCompressedTexImage3DARB"    );
    glCompressedTexImage2DARB       = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)      getProcAddress ( "glCompressedTexImage2DARB"    );
    glCompressedTexImage1DARB       = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)      getProcAddress ( "glCompressedTexImage1DARB"    );
    glCompressedTexSubImage3DARB    = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)   getProcAddress ( "glCompressedTexSubImage3DARB" );
    glCompressedTexSubImage2DARB    = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)   getProcAddress ( "glCompressedTexSubImage2DARB" );
    glCompressedTexSubImage1DARB    = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)   getProcAddress ( "glCompressedTexSubImage1DARB" );
    glGetCompressedTexImageARB      = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)     getProcAddress ( "glGetCompressedTexImageARB"   );

                                    // ARB_point_parameters
    glPointParameterfARB             = (PFNGLPOINTPARAMETERFARBPROC)          getProcAddress ( "glPointParameterfARB" );
    glPointParameterfvARB            = (PFNGLPOINTPARAMETERFVARBPROC)         getProcAddress ( "glPointParameterfvARB" );

                                    // ARB_occlusion_query
    glGenQueriesARB        = (PFNGLGENQUERIESARBPROC)        getProcAddress ( "glGenQueriesARB"        );
    glDeleteQueriesARB     = (PFNGLDELETEQUERIESARBPROC)     getProcAddress ( "glDeleteQueriesARB"     );
    glIsQueryARB           = (PFNGLISQUERYARBPROC)           getProcAddress ( "glIsQueryARB"           );
    glBeginQueryARB        = (PFNGLBEGINQUERYARBPROC)        getProcAddress ( "glBeginQueryARB"        );
    glEndQueryARB          = (PFNGLENDQUERYARBPROC)          getProcAddress ( "glEndQueryARB"          );
    glGetQueryivARB        = (PFNGLGETQUERYIVARBPROC)        getProcAddress ( "glGetQueryivARB"        );
    glGetQueryObjectivARB  = (PFNGLGETQUERYOBJECTIVARBPROC)  getProcAddress ( "glGetQueryObjectivARB"  );
    glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC) getProcAddress ( "glGetQueryObjectuivARB" );

                                    // GLSL-specific extensions
                                    // GL_ARB_shader_object
    glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)         getProcAddress ( "glDeleteObjectARB" );
    glGetHandleARB            = (PFNGLGETHANDLEARBPROC)            getProcAddress ( "glGetHandleARB" );
    glDetachObjectARB         = (PFNGLDETACHOBJECTARBPROC)         getProcAddress ( "glDetachObjectARB" );
    glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)   getProcAddress ( "glCreateShaderObjectARB" );
    glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)         getProcAddress ( "glShaderSourceARB" );
    glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)        getProcAddress ( "glCompileShaderARB" );
    glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)  getProcAddress ( "glCreateProgramObjectARB" );
    glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)         getProcAddress ( "glAttachObjectARB" );
    glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)          getProcAddress ( "glLinkProgramARB" );
    glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)     getProcAddress ( "glUseProgramObjectARB" );
    glValidateProgramARB      = (PFNGLVALIDATEPROGRAMARBPROC)      getProcAddress ( "glValidateProgramARB" );
    glUniform1fARB            = (PFNGLUNIFORM1FARBPROC)            getProcAddress ( "glUniform1fARB" );
    glUniform2fARB            = (PFNGLUNIFORM2FARBPROC)            getProcAddress ( "glUniform2fARB" );
    glUniform3fARB            = (PFNGLUNIFORM3FARBPROC)            getProcAddress ( "glUniform3fARB" );
    glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)            getProcAddress ( "glUniform4fARB" );
    glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)            getProcAddress ( "glUniform1iARB" );
    glUniform2iARB            = (PFNGLUNIFORM2IARBPROC)            getProcAddress ( "glUniform2iARB" );
    glUniform3iARB            = (PFNGLUNIFORM3IARBPROC)            getProcAddress ( "glUniform3iARB" );
    glUniform4iARB            = (PFNGLUNIFORM4IARBPROC)            getProcAddress ( "glUniform4iARB" );
    glUniform1fvARB           = (PFNGLUNIFORM1FVARBPROC)           getProcAddress ( "glUniform1fvARB" );
    glUniform2fvARB           = (PFNGLUNIFORM2FVARBPROC)           getProcAddress ( "glUniform2fvARB" );
    glUniform3fvARB           = (PFNGLUNIFORM3FVARBPROC)           getProcAddress ( "glUniform3fvARB" );
    glUniform4fvARB           = (PFNGLUNIFORM4FVARBPROC)           getProcAddress ( "glUniform4fvARB" );
    glUniform1ivARB           = (PFNGLUNIFORM1IVARBPROC)           getProcAddress ( "glUniform1ivARB" );
    glUniform2ivARB           = (PFNGLUNIFORM2IVARBPROC)           getProcAddress ( "glUniform2ivARB" );
    glUniform3ivARB           = (PFNGLUNIFORM3IVARBPROC)           getProcAddress ( "glUniform3ivARB" );
    glUniform4ivARB           = (PFNGLUNIFORM4IVARBPROC)           getProcAddress ( "glUniform4ivARB" );
    glUniformMatrix2fvARB     = (PFNGLUNIFORMMATRIX2FVARBPROC)     getProcAddress ( "glUniformMatrix2fvARB" );
    glUniformMatrix3fvARB     = (PFNGLUNIFORMMATRIX3FVARBPROC)     getProcAddress ( "glUniformMatrix3fvARB" );
    glUniformMatrix4fvARB     = (PFNGLUNIFORMMATRIX4FVARBPROC)     getProcAddress ( "glUniformMatrix4fvARB" );
    glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC) getProcAddress ( "glGetObjectParameterfvARB" );
    glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) getProcAddress ( "glGetObjectParameterivARB" );
    glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)           getProcAddress ( "glGetInfoLogARB" );
    glGetAttachedObjectsARB   = (PFNGLGETATTACHEDOBJECTSARBPROC)   getProcAddress ( "glGetAttachedObjectsARB" );
    glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONARBPROC)   getProcAddress ( "glGetUniformLocationARB" );
    glGetActiveUniformARB     = (PFNGLGETACTIVEUNIFORMARBPROC)     getProcAddress ( "glGetActiveUniformARB" );
    glGetUniformfvARB         = (PFNGLGETUNIFORMFVARBPROC)         getProcAddress ( "glGetUniformfvARB" );
    glGetUniformivARB         = (PFNGLGETUNIFORMIVARBPROC)         getProcAddress ( "glGetUniformivARB" );
    glGetShaderSourceARB      = (PFNGLGETSHADERSOURCEARBPROC)      getProcAddress ( "glGetShaderSourceARB" );

                                        // GL_ARB_vertex_shader
    glBindAttribLocationARB   = (PFNGLBINDATTRIBLOCATIONARBPROC)   getProcAddress ( "glBindAttribLocationARB" );
    glGetActiveAttribARB      = (PFNGLGETACTIVEATTRIBARBPROC)      getProcAddress ( "glGetActiveAttribARB"    );
    glGetAttribLocationARB    = (PFNGLGETATTRIBLOCATIONARBPROC)    getProcAddress ( "glGetAttribLocationARB"  );
    glGetVertexAttribfvARB    = (PFNGLGETVERTEXATTRIBFVARBPROC)    getProcAddress ( "glGetVertexAttribfvARB"  );

                                        // EXT_stencil_two_side
    glActiveStencilFaceEXT    = (PFNGLACTIVESTENCILFACEEXTPROC)    getProcAddress ( "glActiveStencilFaceEXT" );

										// EXT_depth_bounds_test
	glDepthBoundsEXT      = (PFNGLDEPTHBOUNDSEXTPROC)          getProcAddress ( "glDepthBoundsEXT" );

										// EXT_framebuffer_object
	glIsRenderbufferEXT                      = (PFNGLISRENDERBUFFEREXTPROC                     ) getProcAddress ( "glIsRenderbufferEXT" );
	glBindRenderbufferEXT                    = (PFNGLBINDRENDERBUFFEREXTPROC                   ) getProcAddress ( "glBindRenderbufferEXT" );
	glDeleteRenderbuffersEXT                 = (PFNGLDELETERENDERBUFFERSEXTPROC                ) getProcAddress ( "glDeleteRenderbuffersEXT" );
	glGenRenderbuffersEXT                    = (PFNGLGENRENDERBUFFERSEXTPROC                   ) getProcAddress ( "glGenRenderbuffersEXT" );
	glRenderbufferStorageEXT                 = (PFNGLRENDERBUFFERSTORAGEEXTPROC                ) getProcAddress ( "glRenderbufferStorageEXT" );
	glGetRenderbufferParameterivEXT          = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC         ) getProcAddress ( "glGetRenderbufferParameterivEXT" );
	glIsFramebufferEXT                       = (PFNGLISFRAMEBUFFEREXTPROC                      ) getProcAddress ( "glIsFramebufferEXT" );
	glBindFramebufferEXT                     = (PFNGLBINDFRAMEBUFFEREXTPROC                    ) getProcAddress ( "glBindFramebufferEXT" );
	glDeleteFramebuffersEXT                  = (PFNGLDELETEFRAMEBUFFERSEXTPROC                 ) getProcAddress ( "glDeleteFramebuffersEXT" );
	glGenFramebuffersEXT                     = (PFNGLGENFRAMEBUFFERSEXTPROC                    ) getProcAddress ( "glGenFramebuffersEXT" );
	glCheckFramebufferStatusEXT              = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC             ) getProcAddress ( "glCheckFramebufferStatusEXT" );
	glFramebufferTexture1DEXT                = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC               ) getProcAddress ( "glFramebufferTexture1DEXT" );
	glFramebufferTexture2DEXT                = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC               ) getProcAddress ( "glFramebufferTexture2DEXT" );
	glFramebufferTexture3DEXT                = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC               ) getProcAddress ( "glFramebufferTexture3DEXT" );
	glFramebufferRenderbufferEXT             = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC			   ) getProcAddress ( "glFramebufferRenderbufferEXT" );
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) getProcAddress ( "glGetFramebufferAttachmentParameterivEXT" );
	glGenerateMipmapEXT                      = (PFNGLGENERATEMIPMAPEXTPROC					   ) getProcAddress ( "glGenerateMipmapEXT" );
	glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)getProcAddress ( "glBlendColorEXT" );
	glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)getProcAddress("glLockArraysEXT");
	glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)getProcAddress("glUnlockArraysEXT");


#ifndef	GL_VERSION_1_3										// ARB_transpose matrix
	glLoadTransposeMatrixfARB	              = (PFNGLLOADTRANSPOSEMATRIXFPROC                 ) getProcAddress ( "glLoadTransposeMatrixfARB" );
	glLoadTransposeMatrixdARB	              = (PFNGLLOADTRANSPOSEMATRIXDPROC                 ) getProcAddress ( "glLoadTransposeMatrixdARB" );
	glMultTransposeMatrixfARB	              = (PFNGLMULTTRANSPOSEMATRIXFPROC                 ) getProcAddress ( "glMultTransposeMatrixfARB" );
	glMultTransposeMatrixdARB	              = (PFNGLMULTTRANSPOSEMATRIXDPROC                 ) getProcAddress ( "glMultTransposeMatrixdARB" );
#endif	

#ifdef  __WIN32__
    initWin32Extensions ();
#else
    initLinuxExtensions ();
#endif

    initialized = true;
}

void    printfInfo ()               // print info about card, driver, version & etc
{
    const char * vendor    = (const char *)glGetString ( GL_VENDOR   );
    const char * renderer  = (const char *)glGetString ( GL_RENDERER );
    const char * version   = (const char *)glGetString ( GL_VERSION  );

    printf ( "Graphics card and driver information:\n" );
    printf ( "Vendor:   %s\nRenderer: %s\nVersion:  %s\n", vendor, renderer, version );

    if ( isExtensionSupported ( "GL_ARB_multitexture" ) )
    {
        int maxTextureUnits;

        glGetIntegerv ( GL_MAX_TEXTURE_UNITS_ARB,    &maxTextureUnits );

        printf ( "ARB_multitexture supported.\nMax texture units %d.\n", maxTextureUnits );


    }
    if ( isExtensionSupported ( "GL_NV_register_combiners" ) )
    {
        int maxCombiners;

        glGetIntegerv ( GL_MAX_GENERAL_COMBINERS_NV, &maxCombiners    );

        printf ( "NV_register_combiners supported.\nMax general combiners %d.\n", maxCombiners );
    }

    if ( isExtensionSupported ( "GL_ARB_vertex_buffer_object" ) )
        printf ( "VBO supported.\n" );

    if ( isExtensionSupported ( "GL_NV_occlusion_query" ) )
        printf ( "NV_occlusion_query extension supported.\n" );

    if ( isExtensionSupported ( "GL_SGIS_generate_mipmap" ) )
        printf ( "GL_SGIS_generate_mipmap extension supported.\n" );

    if ( isExtensionSupported ( "GL_ARB_vertex_program" ) )
        printf ( "GL_ARB_vertex_program extension supported.\n" );
}

