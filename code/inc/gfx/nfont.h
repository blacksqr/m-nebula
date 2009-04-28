#ifndef N_FONT_H
#define N_FONT_H

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#include "gfx/ngfxtypes.h"
#include "mathlib/vector.h"
#include "util/nstl.h"

class nPixelShader;
class nGfxServer;

//------------------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nFont
#include "kernel/ndefdllclass.h"
//------------------------------------------------------------------------------
/**
	@class nFont

	@brief nGfxServer-neutral font object

	The nFont class is the superclass for nGfxServer-specific
	font classes. It contains a minimum interface for managing
	a True Type (Freetype lib is used in opengl implementation) font.

	Font defined by 2 parameters - font name, and font type in rendering meaning:
	pixmap font - pixmaps of characters used for rendering
	bitmap font - bitmaps of characters used for rendering	
	texture font - textures generated from chars on properly sized bars used for rendering
	polygon font - polygons generated from chars used for rendering
	extruded font - extruded polygons generated from chars used for rendering
	outline font - line list generated from chars used for rendering

	- created 25-May-2007 - by Ilya, Insightec, Inc

*/
class N_PUBLIC nFont : public nRoot {
public:	
	/// constructor
	nFont();

	/// destructor
	virtual ~nFont() {}

	/// set name and type of the font
	virtual bool Set(const char * name, nFontType type);

	/// set face size of the font
	virtual void SetFaceSize(int sz);

	/// get font name
	virtual const char* GetFontName() const {return  (this->font_name.size() ? this->font_name.c_str() : 0); }

	/// get font type
	virtual nFontType GetFontType() const { return this->font_type; }

	/// Returns line spacing for the font 
	virtual float GetLineHeight() const;

	/// Returns string length
	virtual float GetStringAdvance(const char* str) const;

	/// render string str
	virtual void Render(const char* str, int sz, vector2 pos, float depth, nPixelShader* ps);

	/// is font raster type
	virtual bool IsRaster() const { return this->raster; }

public:	
	/// creates string from name and type
	static const char* GetIDString(const char * name, nFontType type);

	/// convert string to font type
	static nFontType Str2Type(const char* type);

	/// convert font type to string
	static const char* Type2Str(nFontType type);

public:	
	static nKernelServer *kernelServer;	

protected:	
	const char* getFontPath();

	stl_string	font_name;
	nFontType	font_type;	
	stl_string	os_font_path;
	bool		raster;

	nAutoRef<nGfxServer> refGfx;

private:
	static stl_string os_fonts_path;	
};

#endif

