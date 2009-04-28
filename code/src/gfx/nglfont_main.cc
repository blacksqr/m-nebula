#define N_IMPLEMENTS nGlFont
//--------------------------------------------------------------------
//  nglfont_main.cc 
//  (C) Insightec, Inc, Ilya Kliot
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "gfx/nglfont.h"
#include "gfx/nglserver.h"
#include "gfx/npixelshader.h"

//*
#include "ftgl/FTGLBitmapFont.h"
#include "ftgl/FTGLPixmapFont.h"
#include "ftgl/FTGLOutlineFont.h"
#include "ftgl/FTGLPolygonFont.h"
#include "ftgl/FTGLTextureFont.h"
#include "ftgl/FTGLExtrdFont.h"

#define FONT_CACHE "\
`1234567890-=\
~!@#$%^&*()_+\
qwertyuiopasdfghjklzxcvbnm\
QWERTYUIOPASDFGHJKLZXCVBNM\
[]\\;',./\
{}|:\"<>?"


#ifdef _DEBUG
#	pragma comment (lib, "freetype234MT_D.lib")    
#	pragma comment (lib, "ftgl_dynamic_MTD_d.lib")
#else
#	pragma comment (lib, "freetype234MT.lib")    
#	pragma comment (lib, "ftgl_dynamic_MTD.lib")
#endif


//*/

nNebulaClass(nGlFont, "nfont");


//--------------------------------------------------------------------
/**
- created 25-May-2007 - by Ilya, Insightec, Inc
*/
//--------------------------------------------------------------------
nGlFont::nGlFont() : nFont(), font(0)
{
	
}

//--------------------------------------------------------------------
/**
	- created 25-May-2007 - by Ilya, Insightec, Inc
*/
//--------------------------------------------------------------------
nGlFont::~nGlFont() 
{
	if (this->font) 
	{
		delete this->font;
		this->font = 0;
	}
}

//--------------------------------------------------------------------
/**
	set face size of the font
*/
void nGlFont::SetFaceSize(int sz)
{
	if (sz != (int)this->font->FaceSize())
	{
		this->font->FaceSize(sz);
		this->font->Advance(FONT_CACHE);
	}
}

//--------------------------------------------------------------------
/**
	Returns line spacing for the font 
*/
float nGlFont::GetLineHeight() const
{
	n_assert(this->font);
	return this->font->LineHeight();
}


//--------------------------------------------------------------------
/**
	Returns string length
*/
float nGlFont::GetStringAdvance(const char* str) const
{
	n_assert(this->font);
	return this->font->Advance(str);
}

//--------------------------------------------------------------------
/**
	Set font parameters - type and name
	create FTGLFont
	- created 25-May-2007 - by Ilya, Insightec, Inc
*/
bool nGlFont::Set(const char * name, nFontType type)
{
	n_assert("nGlFont::Set font already set" && !this->font);
	if (nFont::Set(name, type))
	{
		switch(type)
		{
		case N_FONT_PIXMAP: 
			this->font = new FTGLPixmapFont(this->os_font_path.c_str());
			break;
		case N_FONT_BITMAP: 
			this->font = new FTGLBitmapFont(this->os_font_path.c_str());
			break;	
		case N_FONT_TEXTURE: 
			this->font = new FTGLTextureFont(this->os_font_path.c_str());
			break;
		case N_FONT_POLYGON: 
			this->font = new FTGLPolygonFont(this->os_font_path.c_str());
			break;
		case N_FONT_EXTRUDED: 
			this->font = new FTGLExtrdFont(this->os_font_path.c_str());
			break;
		case N_FONT_OUTLINE: 
			this->font = new FTGLOutlineFont(this->os_font_path.c_str());
			break;
		default:
			return false;
			break;
		}
		this->font->Advance(FONT_CACHE);
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------
/**
abstract string rendering
- created 25-May-2007 - by Ilya, Insightec, Inc
*/
void nGlFont::Render(const char* str, int sz, vector2 pos, float depth, nPixelShader *ps)
{
	if (this->font && str)
	{
		this->SetFaceSize(sz);
		this->font->Depth(depth);
 		nGlServer* gs = (nGlServer*)this->refGfx.get();
		vector2 pix_sz = gs->GetPixelSize();
		
		if (ps)
		{
			ps->BeginRender(0);		
			ps->Render(0);
		}

		glMatrixMode(GL_MODELVIEW); _glDBG
		glPushMatrix(); 
		char  copy_str[N_MAXPATH];
		strcpy(copy_str, str);

		char seps[] = "\n\r";			
		char* token = strtok( copy_str, seps );			
		int cnt = 0;
		float lnH = this->font->LineHeight()*pix_sz.y;

		while( token )
		{				
			if (this->IsRaster()) 
			{
				glRasterPos2f(pos.x + pix_sz.x, pos.y + pix_sz.y - lnH*(float)cnt); _glDBG
			} 
			else
			{
				glPushMatrix(); 
				glTranslatef(pos.x + pix_sz.x, pos.y + pix_sz.y - lnH*(float)cnt, 0.0f); _glDBG
				glScalef(pix_sz.x, pix_sz.y, 1.0f); _glDBG
			}
			this->font->Render(token);
			if (!this->IsRaster()) 
			{
				glPopMatrix();
			}				
			token = strtok( 0, seps );
			cnt++;				
		}				
		glPopMatrix();		

		// tell pixel shader that we are finished
		if (ps) 
			ps->EndRender();		
	}
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

