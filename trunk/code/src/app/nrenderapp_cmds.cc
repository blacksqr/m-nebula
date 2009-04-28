#define N_IMPLEMENTS nRenderApp
//------------------------------------------------------------------------------
//  (C) 2009	plushy
//------------------------------------------------------------------------------
#include "app/nrenderapp.h"
#include "kernel/npersistserver.h"

//static void n_xxx(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nrenderapp
    
    @superclass
    napplication

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
  //  clazz->AddCmd("v_xxx_v", 'XXXX', n_xxx);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    xxx

    @input
    v

    @output
    v

    @info
    detailed description (for script programmers! no C++ babble)

static
void
n_xxx(void* slf, nCmd* cmd)
{
    nRenderApp* self = (nRenderApp*) slf;
    self->XXX();
}
*/
//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nRenderApp::SaveCmds(nPersistServer* ps)
{
    if (nApplication::SaveCmds(ps))
    {
//        nCmd* cmd = ps->GetCmd(this, 'XXXX');
//        ps->PutCmd(cmd);

        return true;
    }
    return false;
}

