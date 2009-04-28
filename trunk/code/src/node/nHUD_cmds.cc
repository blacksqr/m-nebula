#define N_IMPLEMENTS nHUD

//------------------------------------------------------------------------------
//  node/nlenseflare_cmds.cc
//  author: Jeremy Bishop
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

#include "node/nHUD.h"
#include "kernel/npersistserver.h"

static void n_beginHUD(void *, nCmd *);
static void n_setcolor(void *, nCmd *);
static void n_setposition(void *, nCmd *);
static void n_endHUD(void *, nCmd *);


//------------------------------------------------------------------------------
/**
    @scriptclass
    nHUD

    @superclass
    nvisnode

    @classinfo
    Creates A HUD a the defined position, color and size. HUD is only a bitmap. Use several HUDs to display amounts....
*/
void
n_initcmds(nClass *cl)
{
    cl->BeginCmds();

    cl->AddCmd("v_setcolor_ffff",      'SCOL', n_setcolor);
	cl->AddCmd("v_setposition_ffff",   'SPOS', n_setposition);    

    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcolor

    @input
    f (Red), f (Grn), f (Blu), f (Alpha)

    @output
    v

    @info
    sets the HUD color 
*/
static
void
n_setcolor(void *o, nCmd *cmd)
{
    float fTmp1 = cmd->In()->GetF();    // 1.
    float fTmp2 = cmd->In()->GetF();    // 1.
    float fTmp3 = cmd->In()->GetF();    // 1.
    float fTmp4 = cmd->In()->GetF();    // 1.

    nHUD *self = static_cast<nHUD*>(o);
    self->SetColor(fTmp1, fTmp2, fTmp3, fTmp4);
}
//------------------------------------------------------------------------------
/**
    @cmd
    setposition

    @input
	f (Upper Left X), f (Upper Left Y), f (Lower Right X), f (Lower Right Y)

    @output
    v

    @info
    sets the HUD position 
*/
static
void
n_setposition(void *o, nCmd *cmd)
{
    float fTmp1 = cmd->In()->GetF();    // 1.
    float fTmp2 = cmd->In()->GetF();    // 1.
    float fTmp3 = cmd->In()->GetF();    // 1.
    float fTmp4 = cmd->In()->GetF();    // 1.

    nHUD *self = static_cast<nHUD*>(o);
    self->SetPosition(fTmp1, fTmp2, fTmp3, fTmp4);
}


//------------------------------------------------------------------------------
//  2001.09.25  gamingrat    created
//------------------------------------------------------------------------------
bool
nHUD::SaveCmds(nPersistServer *fs)
{
    if (nVisNode::SaveCmds(fs))
    {
        nCmd *cmd = 0;

        if ((cmd = fs->GetCmd(this, 'SCOL')))
        {
            //vector4 vTmp = GetBlindColor();
            vector4 vTmp;
			vTmp.x=1.0f;
			vTmp.y=1.0f;
			vTmp.z=1.0f;
			vTmp.w=1.0f;
            cmd->In()->SetF(vTmp.x);
            cmd->In()->SetF(vTmp.y);
            cmd->In()->SetF(vTmp.z);
            cmd->In()->SetF(vTmp.w);

            fs->PutCmd(cmd);
        }
		
		if ((cmd = fs->GetCmd(this, 'SPOS')))
        {
            
            vector4 vTmp;
			vTmp.x=1.0f;
			vTmp.y=1.0f;
			vTmp.z=1.0f;
			vTmp.w=1.0f;
            cmd->In()->SetF(vTmp.x);
            cmd->In()->SetF(vTmp.y);
            cmd->In()->SetF(vTmp.z);
            cmd->In()->SetF(vTmp.w);

            fs->PutCmd(cmd);
        }        

        return true;
    }
    else
        return false;
}
//------------------------------------------------------------------------------
