#define N_IMPLEMENTS nVisNode
//------------------------------------------------------------------------------
//  nvisnode_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nvisnode.h"

static void n_setfinishedafter(void* slf, nCmd* cmd);
static void n_getfinishedafter(void* slf, nCmd* cmd);
static void n_preload(void* slf, nCmd* cmd);
static void n_sethud(void *, nCmd *);
static void n_gethud(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nvisnode

    @superclass
    nroot

    @classinfo
*/

//------------------------------------------------------------------------------
/**
    @param clazz    used to associate the registered commands with the class.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setfinishedafter_f", 'SFAF', n_setfinishedafter);
    cl->AddCmd("f_getfinishedafter_v", 'GFAF', n_getfinishedafter);
    cl->AddCmd("v_preload_v",          'PRLD', n_preload);
	cl->AddCmd("v_sethud_b",		'STHD', n_sethud);
	cl->AddCmd("b_gethud_v",		'GTHD', n_gethud);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfinishedafter

    @input
    f (FinishedAfterTime)

    @output
    v

    @info
    Set a finished after time, which is just stored in the object.
    Application can use this value to sync their logic with Nebula
    animations. The value will just be stored, and is persistent.
    Other then that, Nebula will ignore it completely.
*/
static
void
n_setfinishedafter(void* slf, nCmd* cmd)
{
    nVisNode* self = (nVisNode*) slf;
    self->SetFinishedAfter(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfinishedafter

    @input
    v

    @output
    f (FinishedAfterTime)

    @info
    The "finished after" time, as defined by 'setfinishedafter'.
*/
static
void
n_getfinishedafter(void* slf, nCmd* cmd)
{
    nVisNode* self = (nVisNode*) slf;
    cmd->Out()->SetF(self->GetFinishedAfter());
}

//------------------------------------------------------------------------------
/**
    @cmd
    preload

    @input
    v

    @output
    v

    @info
    Preload any used resources (textures, meshes, etc...). This may prevent
    stuttering during rendering, as without preload, resources are loaded
    at first rendering.
*/
static
void
n_preload(void* slf, nCmd* /*cmd*/)
{
    nVisNode* self = (nVisNode*) slf;
    self->Preload();
}

//------------------------------------------------------------------------------
/** begin Ilya 22-05-2006
	@cmd
	sethud

	@input
	b -  attach visnode to screen (HUD)

	@output
	none

	@info
	set visnode HUD (head up display) state
*/
static void n_sethud(void *slf, nCmd *cmd) 
{
	nVisNode* self = (nVisNode*) slf;
	self->SetHUD(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/** begin Ilya 22-05-2006
	@cmd
	gethud

	@input
	v

	@output
	b - is visnode HUD type

	@info
	returns visnode HUD (head up display) state
*/
static void n_gethud(void *slf, nCmd *cmd) {

	nVisNode* self = (nVisNode*) slf;
	cmd->Out()->SetB(self->GetHUD());	
}

//------------------------------------------------------------------------------
/**
    fills an nCmd object with the appropriate persistent data and passes the
    nCmd object on to the nPersistServer for output to a file.

    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nVisNode::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setfinishedafter ---
        if (this->GetFinishedAfter() > 0.0f)
        {
            cmd = ps->GetCmd(this, 'SFAF');
            cmd->In()->SetF(this->GetFinishedAfter());
            ps->PutCmd(cmd);
        }

		//--- sethud ---
		cmd = ps->GetCmd(this, 'STHD');
		cmd->In()->SetB(this->GetHUD());
		ps->PutCmd(cmd);
        return true;
    }
    return false;
}
