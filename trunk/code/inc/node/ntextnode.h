#ifndef N_TEXTNODE_H
#define N_TEXTNODE_H

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif
#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif
#ifndef N_STL_H
#include "util/nstl.h"
#endif
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif


#undef N_DEFINES
#define N_DEFINES nTextNode
#include "kernel/ndefdllclass.h"

class nSceneGraph2;
class nPersistServer;
class nFont;
//------------------------------------------------------------------------------
/**
	CLASS
	nTextNode
	created 25-May-2007  -- Ilya Kliot
	(C) 2006 Insightec Ltd, 
*/
class N_PUBLIC nTextNode : public nVisNode {
public:
	/// default constructor
	nTextNode();

	/// default destructor
	virtual ~nTextNode();

	/// persistence
	virtual bool SaveCmds(nPersistServer* fileServer);    

	/// attach to scene 
	virtual bool Attach(nSceneGraph2* sceneGraph);   

	/// update internal state and set scene graph node transform 
	virtual void Compute(nSceneGraph2* sceneGraph);	

	/// set name and type of the font
	virtual bool SetFont(const char* name, nFontType type);

	/// get font name
	virtual const char* GetFontName() const;

	/// get font type
	virtual nFontType GetFontType() const;

	/// set text string for rendering
	virtual void SetText(const char* _text);

	/// get text string 
	virtual const char* GetText(void) const;	

	/// set font size for rendering
	virtual void SetSize(int sz) 
	{ 
		if (sz > 0 && this->font_size != sz) 
		{
			this->font_size = sz; 
			this->dirty = true;
		}
	}

	/// get font size
	virtual int GetSize(void) const { return this->font_size; }

	/// returns true if font is a raster font like pixmap or bitmap
	virtual bool IsRaster();
	
	/// set raster position for bitmap and pixmap fonts, resets the alignment
	virtual void SetRasterPos(float x, float y);
	/// set raster position for bitmap and pixmap fonts, resets the alignment
	virtual void SetRasterPos(const vector2& p) {this->SetRasterPos(p.x, p.y);}

	/// get raster position for bitmap and pixmap fonts
	virtual vector2 GetRasterPos() const { return this->raster_pos; }

	/** 
		set justified position and line number, for current font
		the horizontal alignment should be none, center, left or right + column number
		the vertical alignment should be none, center, top or bottom and line number 
		in form: top - line number, bottom + line number, center - line number
		for instance: top 2 means that it will be second line down from top,
		bottom 3 third line up from bottom
	*/
	virtual void SetAlignment(nHorizontalAlign hor, int col, nVerticalAlign ver, int ln);

	/// returns horizontal alignment none, center, left or right 
	virtual nHorizontalAlign GetHAlignment() const { return this->h_align;}

	/// returns vertical alignment none, center, top or bottom 
	virtual nVerticalAlign GetVAlignment() const { return this->v_align;}

	/// returns line number relatevely to vertical alignment
	virtual int GetColumn() const { return this->column;}

	/// returns line number relatevely to vertical alignment
	virtual int GetLine() const { return this->line;}

	/// set depth for extruded font
	virtual void SetDepth(float d) { this->depth = d; }

	/// get depth of extruded font
	virtual float GetDepth() const { return this->depth; }	

	/// calculate raster position according to unprojected parent n3dnode and some 2d offset
	virtual void SetSnap(bool b) { this->snap = b; }

	/// get snap state
	virtual bool GetSnap() const {return this->snap;}

	/// calculate raster position according to unprojected parent n3dnode and some 2d offset
	virtual void SetSnapOffset(const vector2& offs) {this->offset = offs;}

	/// get snap offset
	virtual const vector2& GetSnapOffset() const {return this->offset;}

protected:
	/// evaluates position according to alignments
	virtual void eval_pos(nSceneGraph2* sceneGraph);	
	
	//members
public:    
	/// nKernelServer dependency. 
	static nKernelServer* kernelServer;

protected:
	stl_string		text;
	stl_string		measure_text;
	nRef<nFont>		fontRef;
	int				font_size;
	float			depth;
	vector2			raster_pos;

	nVerticalAlign			v_align;
	nHorizontalAlign		h_align;
	int				line;
	int				column;

	float			fline;
	float			fcolumn;

	bool			snap;
	vector2			offset;

private:
	bool			dirty;
};

#endif
