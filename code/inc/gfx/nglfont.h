#ifndef N_GLFONT_H
#define N_GLFONT_H

#include "gfx/nfont.h"

//------------------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nGlFont
#include "kernel/ndefdllclass.h"

class FTFont;
//------------------------------------------------------------------------------
/**
	@class nGlFont

	@brief wraps OpenGL texture into nTexture object

	The nGlFont class is the superclass for nGfxServer-specific
	font classes. It contains a minimum interface for managing
	a True Type (Freetype lib is used in opengl implementation) font.

	Font defined by 2 parameters - font name, and font type in rendering meaning:
	pixmap font - pixmaps of characters used for rendering
	bitmap font - bitmaps of characters used for rendering
	polygon font - polygons generated from chars used for rendering
	texture font - textures generated from chars on properly sized bars used for rendering
	outline font - line list generated from chars used for rendering

	- created 25-May-2007 - by Ilya, Insightec, Inc
*/
class N_PUBLIC nGlFont : public nFont {
public:	
	/// constructor
	nGlFont();

	/// destructor
	virtual ~nGlFont();

	/// set name and type of the font
	virtual bool Set(const char * name, nFontType type);

	/// set face size of the font
	virtual void SetFaceSize(int sz);

	/// Returns line spacing for the font 
	virtual float GetLineHeight() const;

	/// Returns string length
	virtual float GetStringAdvance(const char* str) const;

	/// render string str
	virtual void Render(const char* str, int sz, vector2 pos, float depth, nPixelShader *ps);

	static nKernelServer *kernelServer;	
private:
	FTFont*		font;
};

#endif

