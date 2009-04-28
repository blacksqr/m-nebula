#define N_IMPLEMENTS nFont
//--------------------------------------------------------------------
//  nfont_main.cc 
//  (C) Insightec, Inc, Ilya Kliot
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "gfx/nfont.h"

nNebulaClass(nFont, "nroot");
stl_string nFont::os_fonts_path;


//--------------------------------------------------------------------
/**
	- created 25-May-2007 - by Ilya, Insightec, Inc
*/
nFont::nFont() : refGfx(kernelServer, this), font_type(N_FONT_NONE)
{
	this->refGfx     = "/sys/servers/gfx";
	// init os_font_path
	if (0 == nFont::os_fonts_path.size())
	{
	#if defined(__WIN32__)	
		nFont::os_fonts_path = getenv("windir");
		nFont::os_fonts_path += "\\fonts\\";	
	#elif  defined (__linux__)
		nFont::os_fonts_path = "/usr/share/fonts/truetype/";
	#endif
	}	
}

//--------------------------------------------------------------------
/**
	Set font parameters - type and name
	- created 25-May-2007 - by Ilya, Insightec, Inc
*/
bool nFont::Set(const char * name, nFontType type)
{
	n_assert("Font redefinition" && this->font_type == N_FONT_NONE);
	n_assert("Incorrect type" && type != N_FONT_NONE);	

	this->os_font_path = nFont::os_fonts_path;
	this->os_font_path += name;
	this->os_font_path += ".ttf";

	nFile* os_file = this->kernelServer->GetFileServer2()->NewFileObject();
	bool exists = os_file->Exists(this->os_font_path.c_str());
	delete os_file;

	if (exists)
	{
		this->font_type = type;
		this->font_name = name;	
		this->raster = (type == N_FONT_PIXMAP || type == N_FONT_BITMAP);
	}
	else
	{
		this->font_type = N_FONT_NONE;
	}
	return exists;
}

//--------------------------------------------------------------------
/**
	set face size of the font
*/
void nFont::SetFaceSize(int)
{
	n_error("Pure virtual function nFont::SetFaceSize called.");
}

//--------------------------------------------------------------------
/**
	abstract string rendering
	- created 25-May-2007 - by Ilya, Insightec, Inc
*/
void nFont::Render(const char*, int , vector2, float, nPixelShader*)
{
	n_error("Pure virtual function nFont::Render called.");
}

//--------------------------------------------------------------------
/**
	Returns line spacing for the font 
*/
float nFont::GetLineHeight() const
{
	n_error("Pure virtual function nFont::GetLineHeight called.");
	return 0.0f;
}


//--------------------------------------------------------------------
/**
	Returns string length
*/
float nFont::GetStringAdvance(const char* ) const
{
	n_error("Pure virtual function nFont::GetStringAdvance called.");
	return 0.0f;
}


struct str2param {
	const char *str;
	nFontType   val;
};

// font type parameter translation table
static struct str2param str2param_table[] =
{	
	{"pixmap", N_FONT_PIXMAP},
	{"bitmap", N_FONT_BITMAP},	
	{"texture", N_FONT_TEXTURE},
	{"polygon", N_FONT_POLYGON},
	{"extruded", N_FONT_EXTRUDED},
	{"outline",	N_FONT_OUTLINE},
	{0, N_FONT_NONE},
};

//--------------------------------------------------------------------
/**
	str2param()
	- created 25-May-2007 - by Ilya, Insightec, Inc
*/
nFontType nFont::Str2Type(const char *str)
{
	int i = 0;
	struct str2param *p = 0;
	while (p = &(str2param_table[i++]), p->str) 
	{
		if (strcmp(p->str,str) == 0) return p->val;

	}
	n_printf("nFont: string '%s' not accepted as font type!\n", str);    
	return N_FONT_NONE;
}

//--------------------------------------------------------------------
/**
	param2str()
	- created 25-May-2007 - by Ilya, Insightec, Inc
*/
const char* nFont::Type2Str(nFontType val)
{
	int i = 0;
	struct str2param *p = 0;
	while (p = &(str2param_table[i++]), p->val != N_FONT_NONE) 
	{
		if (p->val == val) return p->str;

	}
	n_printf("nFont: unknown font type!\n");	
	return 0;
}

//--------------------------------------------------------------------
/**
creates string from name and type
- created 25-May-2007 - by Ilya, Insightec, Inc
*/
const char* nFont::GetIDString(const char * name, nFontType type)
{
	static stl_string id;
	id = name;
	id += "_";
	id += nFont::Type2Str(type);
	return id.c_str();
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

