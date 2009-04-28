#define N_IMPLEMENTS nOdeTest
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//------------------------------------------------------------------------------
#include "odetests/nodetest.h"
#include "kernel/npersistserver.h"

static void n_start(void* slf, nCmd* cmd);
static void n_stop(void* slf, nCmd* cmd);
static void n_setgrid(void* slf, nCmd* cmd);
static void n_getgrid(void* slf, nCmd* cmd);
static void n_setsleep(void* slf, nCmd* cmd);
static void n_getsleep(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nOdeTest
    
    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_start_v",            'STRT', n_start);
    clazz->AddCmd("v_stop_v",             'STOP', n_stop);
    clazz->AddCmd("v_setgrid_b",          'SGRD', n_setgrid);
    clazz->AddCmd("b_getgrid_v",          'GGRD', n_getgrid);
    clazz->AddCmd("v_setsleep_f",         'SSLP', n_setsleep);
    clazz->AddCmd("f_getsleep_v",         'GSLP', n_getsleep);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    start

    @input
    v

    @output
    v

    @info
    Start simulating the world.
*/
static
void
n_start(void* slf, nCmd* cmd)
{
    nOdeTest* self = (nOdeTest*) slf;
    self->Start();
}

//------------------------------------------------------------------------------
/**
    @cmd
    stop

    @input
    v

    @output
    v

    @info
    Stop simulating the world.
*/
static
void
n_stop(void* slf, nCmd* cmd)
{
    nOdeTest* self = (nOdeTest*) slf;
    self->Stop();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getgrid

    @input
    v

    @output
    b

    @info
    Get the value last set by 'setgrid'.
*/
static
void
n_getgrid(void* slf, nCmd* cmd)
{
    nOdeTest* self = (nOdeTest*) slf;
    cmd->Out()->SetB( self->GetGrid() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setgrid

    @input
    b (true to display, false otherwise)

    @output
    v

    @info
    Display a grid in the viewport (only works in OpenGL).
*/
static
void
n_setgrid(void* slf, nCmd* cmd)
{
    nOdeTest* self = (nOdeTest*) slf;
    self->SetGrid( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsleep

    @input
    v

    @output
    f

    @info
    Get the amount of time the world should be suspended for each frame.
*/
static
void
n_getsleep(void* slf, nCmd* cmd)
{
    nOdeTest* self = (nOdeTest*) slf;
    cmd->Out()->SetF( self->GetSleep() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsleep

    @input
    f

    @output
    v

    @info
    Set the amount of time the world should be suspended for after each frame.
    This is mainly usefull when running in a window, so that other tasks can
    run as well.
*/
static
void
n_setsleep(void* slf, nCmd* cmd)
{
    nOdeTest* self = (nOdeTest*) slf;
    self->SetSleep( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOdeTest::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        //nCmd* cmd = ps->GetCmd(this, 'XXXX');
        //ps->PutCmd(cmd);

        return true;
    }
    return false;
}

