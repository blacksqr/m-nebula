#define N_IMPLEMENTS nTextNode
//------------------------------------------------------------------------------
/**
	ntexnode_main.cc
	(C) 2007 Insightec Ltd, -- Ilya
*/

#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "node/ntextnode.h"
#include "gfx/nfont.h"

nNebulaScriptClass(nTextNode, "nvisnode");

//------------------------------------------------------------------------------
/**
	constructor
*/
nTextNode::nTextNode() : nVisNode(), 
	fontRef(this), 
	font_size(16), 
	depth(1.0f), 
	h_align(N_HA_NONE),
	v_align(N_VA_NONE),
	line(0),
	column(0),
	dirty(true),
	snap(false)
{	

	this->SetFont("tahoma", N_FONT_OUTLINE);
}


//------------------------------------------------------------------------------
/**
	destructor
*/
nTextNode::~nTextNode() 
{	
	// invalidate current font
	release_ref(this->fontRef);
}

//------------------------------------------------------------------------------
/**
	set text string for rendering
*/
void nTextNode::SetText(const char* _text) 
{ 	
	this->text.clear();
	if (_text) 
	{
		char  copy_text[N_MAXPATH*4];
		strcpy(copy_text, _text);

		char seps[] = "\t";
		char* tab = "    ";
		char* token = strtok( copy_text, seps );
		bool first = true;

		while( token )
		{
			if (!first)
				this->text += tab;
			else
				first = false;

			this->text += token;
			// Get next token: 
			token = strtok( 0, seps );
		}	

		
		this->measure_text = this->text;
		std::replace(this->measure_text.begin(), this->measure_text.end(), '\n', '\0');
	}		
	this->dirty = true;
}

//------------------------------------------------------------------------------
/**
	set raster position for bitmap and pixmap fonts, resets the alignment
*/
void nTextNode::SetRasterPos(float x, float y) 
{ 
	this->raster_pos.set(x, y); 
	this->v_align = N_VA_NONE;  
	this->h_align = N_HA_NONE;
	this->dirty = true;
}


//------------------------------------------------------------------------------
/**
	get text string for rendering
*/
const char* nTextNode::GetText(void) const 
{ 
	return (this->text.size() ? this->text.c_str() : 0);
}

//------------------------------------------------------------------------------
/**
	returns true if font is a raster font like pixmap or bitmap
*/
bool nTextNode::IsRaster() 
{ 
	return (this->fontRef.isvalid() ? this->fontRef->IsRaster() : false);
}

//------------------------------------------------------------------------------
/** 
	set justified position and line number, for current font
	the horizontal alignment should be none, center, left or right
	the vertical alignment should be none, center, top or bottom and line number 
	in form: top - line number, bottom + line number, center - line number
	for instance: top 2 means that it will be second line down from top,
	bottom 3 third line up from bottom
*/
void nTextNode::SetAlignment(nHorizontalAlign hor, int col, nVerticalAlign ver, int ln) 
{
	this->h_align = hor;
	this->v_align = ver;
	this->line = ln;
	this->fline = (float)ln;
	this->column = col;
	this->fcolumn = (float)col;
	this->dirty = true;
}

//------------------------------------------------------------------------------
/**
	set name and type of the font
*/
bool nTextNode::SetFont(const char* name, nFontType type)
{
	bool res = true;
	// invalidate current mesh
	release_ref(this->fontRef);

	// see if texture already exists and can be shared
	nFont *font = this->refGfx->FindFont(name, type);

	// if not, create a new one
	if (!font) 
	{
		font = this->refGfx->NewFont(name, type);
		n_assert(font);
		res = font->Set(name, type);				
	} 

	if (font && res)
		this->fontRef = font;

	this->dirty = true;
	
	return res;
}

//------------------------------------------------------------------------------
/**
	evaluates position according to alignment
*/
void nTextNode::eval_pos(nSceneGraph2* sceneGraph)
{
	if (this->text.size() && this->font_size)
	{
		this->fontRef->SetFaceSize(this->font_size);		

		if (this->GetSnap())
		{
			nGfxServer* gfx = this->refGfx.get();
			if (gfx && sceneGraph)
			{					 
				this->SetRasterPos(gfx->Project2D(sceneGraph->GetTransform()*vector3()) + 
					this->GetSnapOffset());				
			}			
		}
		else
		{	
			vector2 pix_sz = this->refGfx->GetPixelSize();
			float width = this->fontRef->GetStringAdvance(this->measure_text.c_str())*pix_sz.x;
			float col_width = this->fontRef->GetStringAdvance("A")*pix_sz.x;
			float height = this->fontRef->GetLineHeight()*pix_sz.y;	

			switch(this->GetHAlignment())
			{
			case N_HA_CENTER:
				this->raster_pos.x = -width/2.0f + col_width*this->fcolumn;
				break;
			case N_HA_LEFT:
				this->raster_pos.x = -1.0f + col_width*this->fcolumn;
				break;
			case N_HA_RIGHT:
				this->raster_pos.x = 1.0f - width - col_width*this->fcolumn;
				break;		
			}

			switch(this->GetVAlignment())
			{
			case N_VA_CENTER:
				this->raster_pos.y = -height/2.0f - height*this->fline;
				break;
			case N_VA_TOP:
				this->raster_pos.y = 1.0f - height - height*(float)this->fline;
				break;
			case N_VA_BOTTOM:
				this->raster_pos.y = -1.0f + height*(float)this->fline;
				break;		
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
	Attach the object to the scene.
*/
bool nTextNode::Attach(nSceneGraph2* sceneGraph) {
	if (nVisNode::Attach(sceneGraph)) {
		sceneGraph->AttachVisualNode(this);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
	Updates the internal matrix and computes lod value, finally sets
	the transformation for the current scene graph node.

	@param  sceneGraph      the scene graph who wants us to render
*/
void nTextNode::Compute(nSceneGraph2* sceneGraph) 
{	
	if (this->fontRef.isvalid() && this->text.size())
	{
		this->eval_pos(sceneGraph);
		
		this->fontRef->Render(this->text.c_str(), 
			this->font_size, 
			this->raster_pos,
			this->depth,
			sceneGraph->GetPixelShader());
	}

	nVisNode::Compute(sceneGraph);
}


//------------------------------------------------------------------------------
/**
	@returns font name
*/
const char* nTextNode::GetFontName() const 
{
	if (this->fontRef.isvalid())
	{
		nFont* font = this->fontRef.get();
		return font->GetFontName();
	}
	else
		return 0;
}


//------------------------------------------------------------------------------
/**
	@returns font type
*/
nFontType nTextNode::GetFontType() const 
{ 
	if (this->fontRef.isvalid())
	{
		nFont* font = this->fontRef.get();
		return font->GetFontType();
	} else
		return N_FONT_NONE;
}