#define N_IMPLEMENTS nTextNode
//-------------------------------------------------------------------
//  ntextnode_cmds.cc
//  (C) 2007 Insightec Ltd, -- Ilya Kliot
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/ntextnode.h"
#include "gfx/nfont.h"

static void n_settext(void*, nCmd*);
static void n_gettext(void*, nCmd*);
static void n_setsize(void*, nCmd*);
static void n_getsize(void*, nCmd*);
static void n_israster(void*, nCmd*);
static void n_setrasterpos(void*, nCmd*);
static void n_getrasterpos(void*, nCmd*);
static void n_setalign(void*, nCmd*);
static void n_getalign(void*, nCmd*);
static void n_setdepth(void*, nCmd*);
static void n_getdepth(void*, nCmd*);
static void n_setfont(void*, nCmd*);
static void n_getfontname(void*, nCmd*);
static void n_getfonttype(void*, nCmd*);
static void n_setsnap(void*, nCmd*);
static void n_getsnap(void*, nCmd*);
static void n_setsnapoffs(void*, nCmd*);
static void n_getsnapoffs(void*, nCmd*);

struct str2halign_t {
	const char		  *str;
	nHorizontalAlign   val;
};

// horizontal alignment parameter translation table
static struct str2halign_t str2halign_table[] =
{	
	{"center", N_HA_CENTER},
	{"left", N_HA_LEFT},	
	{"right", N_HA_RIGHT},
	{"none", N_HA_NONE},
	{0, N_HA_NONE},
};

struct str2valign_t {
	const char		*str;
	nVerticalAlign   val;
};

// vertical alignment parameter translation table
static struct str2valign_t str2valign_table[] =
{	
	{"center", N_VA_CENTER},
	{"top", N_VA_TOP},	
	{"bottom", N_VA_BOTTOM},
	{"none", N_VA_NONE},
	{0, N_VA_NONE},
};

//--------------------------------------------------------------------
/**
	str2halign
	- created 12-Jun-2007 - by Ilya, Insightec, Inc
*/
static nHorizontalAlign str2halign(const char *str)
{
	int i=0;
	struct str2halign_t *p = 0;
	while (p = &(str2halign_table[i++]), p->str) 
	{
		if (strcmp(p->str, str) == 0) return p->val;
	}	
	return N_HA_NONE;
}

//--------------------------------------------------------------------
/**
	str2valign
	- created 12-Jun-2007 - by Ilya, Insightec, Inc
*/
static nVerticalAlign str2valign(const char *str)
{
	int i=0;
	struct str2valign_t *p = 0;
	while (p = &(str2valign_table[i++]), p->str) 
	{
		if (strcmp(p->str, str) == 0) return p->val;
	}	
	return N_VA_NONE;
}

//--------------------------------------------------------------------
/**
	halign2str()
	- created 12-Jun-2007 - by Ilya, Insightec, Inc
*/
static const char* halign2str(nHorizontalAlign val)
{
	int i = 0;
	struct str2halign_t *p = 0;
	while (p = &(str2halign_table[i++]), p->str) 
	{
		if (p->val == val) return p->str;

	}		
	return "none";
}

//--------------------------------------------------------------------
/**
	valign2str()
	- created 12-Jun-2007 - by Ilya, Insightec, Inc
*/
static const char* valign2str(nVerticalAlign val)
{
	int i = 0;
	struct str2valign_t *p = 0;
	while (p = &(str2valign_table[i++]), p->str) 
	{
		if (p->val == val) return p->str;

	}		
	return "none";
}


//------------------------------------------------------------------------------
/**
	@scriptclass
	ntextnode

	@superclass
	nvisnode

	@classinfo
	object used to render string with given text using font was set

	Font defined by 2 parameters - font name, and font type in rendering meaning:
		pixmap - pixmaps of characters used for rendering
		bitmap - bitmaps of characters used for rendering		
		texture - textures generated from chars on properly sized bars used for rendering
		polygon - polygons generated from chars used for rendering
		extruded - extruded polygons generated from chars used for rendering
		outline - line list generated from chars used for rendering

	also fonts size and raster position could be defined
	default font is tahoma, size is 16 and raster pos 0:0
*/

void n_initcmds(nClass *cl)
{
	cl->BeginCmds();
	cl->AddCmd("v_settext_s",				'STTX',	n_settext);
	cl->AddCmd("s_gettext_v",				'GTTX',	n_gettext);
	cl->AddCmd("v_setsize_i",				'SFSZ',	n_setsize);
	cl->AddCmd("i_getsize_v",				'GFSZ',	n_getsize);
	cl->AddCmd("b_israster_v",				'ISRS',	n_israster);
	cl->AddCmd("v_setrasterpos_ff",			'STRP',	n_setrasterpos);
	cl->AddCmd("ff_getrasterpos_v",			'GTRP',	n_getrasterpos);
	cl->AddCmd("v_setalign_sisi",			'STAL',	n_setalign);
	cl->AddCmd("sisi_getalign_v",			'GHAL',	n_getalign);
	cl->AddCmd("v_setdepth_f",				'STDP',	n_setdepth);
	cl->AddCmd("f_getdepth_v",				'GTDP',	n_getdepth);
	cl->AddCmd("b_setfont_ss",				'STFN',	n_setfont);
	cl->AddCmd("s_getfontname_v",			'GTFN',	n_getfontname);
	cl->AddCmd("s_getfonttype_v",			'GTFT',	n_getfonttype);	
	cl->AddCmd("v_setsnap_b",				'STSN',	n_setsnap);
	cl->AddCmd("b_getsnap_v",				'GTSN',	n_getsnap);
	cl->AddCmd("v_setsnapoffs_ff",			'SSOF',	n_setsnapoffs);
	cl->AddCmd("ff_getsnapoffs_v",			'GSOF',	n_getsnapoffs);
	cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
	@cmd
	settext

	@input
	s - text

	@output
	v - none

	@info
	set text string for rendering
*/
static void n_settext(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;   
	self->SetText(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
	@cmd
	gettext

	@input
	v - none

	@output
	s - text

	@info
	get text for rendering
*/
static void n_gettext(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;
	const char* s = self->GetText();
	if (!s)
		s = "";
	cmd->Out()->SetS(s);	
}


//------------------------------------------------------------------------------
/**
	@cmd
	setsize

	@input
	i - size

	@output
	v - none

	@info
	set font size for rendering
*/
static void n_setsize(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;   
	self->SetSize(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
	@cmd
	getsize

	@input
	v - none

	@output
	i - size

	@info
	get font size
*/
static void n_getsize(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;	
	cmd->Out()->SetI(self->GetSize());	
}

//------------------------------------------------------------------------------
/**
	@cmd
	israster

	@input
	v - none

	@output
	b - is current font raster type (pixmap or bitmap)

	@info
	check about raster type of the font
*/
static void n_israster(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;	
	cmd->Out()->SetB(self->IsRaster());	
}

//------------------------------------------------------------------------------
/**
	@cmd
	setrasterpos

	@input
	f f - x, y

	@output
	v - none

	@info
	set text raster position for bitmap and pixmap fonts rendering	
*/
static void n_setrasterpos(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;   
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	self->SetRasterPos(x, y);
}

//------------------------------------------------------------------------------
/**
	@cmd
	getrasterpos

	@input
	v - none

	@output
	f f - x, y

	@info
	get text raster position
*/
static void n_getrasterpos(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;	
	vector2 pos = self->GetRasterPos();
	cmd->Out()->SetF(pos.x);
	cmd->Out()->SetF(pos.y);
}


//------------------------------------------------------------------------------
/**
	@cmd
	setalign

	@input
	s(horizontal alignment) i(column) s(vertical alignment) i(line number)

	@output
	v - none

	@info
	set justified position and line number, for current font
	the horizontal alignment should be none, center, left or right and column number

	the vertical alignment should be none, center, top or bottom and line number 
	in form: top - line number, bottom + line number, center - line number
	for instance: top 2 means that it will be second line down from top,
	bottom 3 third line up from bottom
*/
static void n_setalign(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;		
	const char* halign = cmd->In()->GetS();
	int cl = cmd->In()->GetI();
	const char* valign = cmd->In()->GetS();
	int ln = cmd->In()->GetI();
	self->SetAlignment(str2halign(halign), cl, str2valign(valign), ln);
}

//------------------------------------------------------------------------------
/**
	@cmd
	getalign

	@input
	v - none	

	@output
	s(horizontal alignment) s(vertical alignment) i(line number)

	@info
	get justified position and line number, of the current text
	the horizontal alignment could be none, center, left or right
	the vertical alignment could be none, center, top or bottom 
*/
static void n_getalign(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;	
	cmd->Out()->SetS(halign2str(self->GetHAlignment()));
	cmd->Out()->SetI(self->GetColumn());
	cmd->Out()->SetS(valign2str(self->GetVAlignment()));
	cmd->Out()->SetI(self->GetLine());
}

//------------------------------------------------------------------------------
/**
	@cmd
	setdepth

	@input
	f  - depth

	@output
	v - none

	@info
	set depth for extruded font rendering	
*/
static void n_setdepth(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;   	
	self->SetDepth(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
	@cmd
	getdepth

	@input
	v - none

	@output
	f  - depth

	@info
	set depth of extruded font
*/
static void n_getdepth(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;		
	cmd->Out()->SetF(self->GetDepth());
	
}

//------------------------------------------------------------------------------
/**
	@cmd
	setfont

	@input
	s s - font name, font type 
	@output
	v - none

	@info
	set font name (tahoma, arial etc.) and font type - one of:
		pixmap - pixmaps of characters used for rendering
		bitmap - bitmaps of characters used for rendering		
		texture - textures generated from chars on properly sized bars used for rendering
		polygon - polygons generated from chars used for rendering
		extruded - extruded polygons generated from chars used for rendering
		outline - line list generated from chars used for rendering
*/
static void n_setfont(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;   
	const char* name = cmd->In()->GetS();
	const char* type = cmd->In()->GetS();	
	cmd->Out()->SetB(self->SetFont(name, nFont::Str2Type(type)));
}

//------------------------------------------------------------------------------
/**
	@cmd
	getfontname

	@input
	v - none

	@output
	s - font name

	@info
*/
static void n_getfontname(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;	
	cmd->Out()->SetS(self->GetFontName());
}

//------------------------------------------------------------------------------
/**
	@cmd
	getfonttype

	@input
	v - none

	@output
	s - font type
	@info
*/
static void n_getfonttype(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;	
	cmd->Out()->SetS(nFont::Type2Str(self->GetFontType()));
}


//------------------------------------------------------------------------------
/**
	@cmd
	setsnap

	@input
	b -  snap

	@output
	v - none

	@info
	set calculation of raster position according to unprojected parent n3dnode 
	and some 2d offset on/off
*/
static void n_setsnap(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;   
	self->SetSnap(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
	@cmd
	getsnap

	@input
	v - none

	@output
	b - state of snap

	@info
	get state of snap
*/
static void n_getsnap(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;	
	cmd->Out()->SetB(self->GetSnap());	
}

//------------------------------------------------------------------------------
/**
	@cmd
	setsnapoffs

	@input
	f f -  x, y offset from unprojected point

	@output
	v - none

	@info
	set calculation of raster position according to unprojected parent n3dnode 
	and some 2d offset on/off
*/
static void n_setsnapoffs(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;   
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	self->SetSnapOffset(vector2(x,y));
}

//------------------------------------------------------------------------------
/**
	@cmd
	getsnap

	@input
	v - none

	@output
	f f -  x, y offset from unprojected point

	@info
	get snap offset x, y
*/
static void n_getsnapoffs(void* o, nCmd* cmd) 
{
	nTextNode* self = (nTextNode*) o;	
	vector2 offs = self->GetSnapOffset();
	cmd->Out()->SetF(offs.x);	
	cmd->Out()->SetF(offs.y);	
}

//------------------------------------------------------------------------------
/**
	Emit commands to make this object persistent.

	@param  fs      file server which makes the object persistent
	@return         true if all ok
*/
bool nTextNode::SaveCmds(nPersistServer *ps){
	bool retval = false;
	if (nVisNode::SaveCmds(ps)) 
	{	
		nCmd *cmd;

		//---  settext ---        
		if (this->GetText())
		{
			cmd = ps->GetCmd(this, 'STTX');
			cmd->In()->SetS(this->GetText());
			ps->PutCmd(cmd);
		}	

		//---  setsize ---
		cmd = ps->GetCmd(this, 'SFSZ');
		cmd->In()->SetI(this->GetSize());
		ps->PutCmd(cmd);

		//---  setrasterpos ---
		vector2 pos = this->GetRasterPos();
		cmd = ps->GetCmd(this, 'STRP');
		cmd->In()->SetF(pos.x);
		cmd->In()->SetF(pos.y);
		ps->PutCmd(cmd);

		//---  setdepth ---		
		cmd = ps->GetCmd(this, 'STDP');
		cmd->In()->SetF(this->GetDepth());		
		ps->PutCmd(cmd);


		//---  setsnap ---		
		cmd = ps->GetCmd(this, 'STSN');
		cmd->In()->SetB(this->GetSnap());		
		ps->PutCmd(cmd);

		//---  setoffset ---		
		pos = this->GetSnapOffset();
		cmd = ps->GetCmd(this, 'SSOF');
		cmd->In()->SetF(pos.x);
		cmd->In()->SetF(pos.y);
		ps->PutCmd(cmd);

		//--- setfont ---
		if (this->GetFontName())
		{
			cmd = ps->GetCmd(this, 'STFN');
			cmd->In()->SetS(this->GetFontName());
			cmd->In()->SetS(nFont::Type2Str(this->GetFontType()));
			ps->PutCmd(cmd);
		}	

		retval = true;
	}
	return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
