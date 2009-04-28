#define N_IMPLEMENTS nObserver2
//-------------------------------------------------------------------
//  misc/nobs_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "misc/nobserver2.h"

static void n_start(void *, nCmd *);
static void n_stop(void *, nCmd *);
static void n_startsinglestep(void *, nCmd *);
static void n_rendersinglestep(void *, nCmd *);
static void n_setsleep(void *, nCmd *);
static void n_getsleep(void *, nCmd *);
static void n_setgrid(void *, nCmd *);
static void n_getgrid(void *, nCmd *);
static void n_renderframe(void *, nCmd *);
static void n_togglevis(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nobserver

    @superclass
    nroot

    @classinfo
    Flexible replacement for gfxserver.exe. The following 
    objects have to be initialized:
         FIXME WHICH ONES??? TMI
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_start_v",             'STRT', n_start);
    cl->AddCmd("v_stop_v",              'STOP', n_stop);
    cl->AddCmd("v_startsinglestep_v",   'SRTS', n_startsinglestep);
    cl->AddCmd("b_rendersinglestep_v",  'RSNG', n_rendersinglestep);
    cl->AddCmd("v_setsleep_f",          'SSLP', n_setsleep);
    cl->AddCmd("f_getsleep_v",          'GSLP', n_getsleep);
    cl->AddCmd("v_setgrid_b",           'SGRD', n_setgrid);
    cl->AddCmd("b_getgrid_v",           'GGRD', n_getgrid);
    cl->AddCmd("v_renderframe_f",       'RFRM', n_renderframe);
    cl->AddCmd("b_togglevis_v",         'TOGV', n_togglevis);
    cl->EndCmds();
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
    Starts the rendering loop. The method returns if: 
    - the window is closed 
    - the 'exit' script command is executed 
    - the 'stop' command of the nobserver was used 
*/
static void n_start(void *o, nCmd *)
{
    nObserver2 *self = (nObserver2 *) o;
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
    Sets the internal stop flag. 
*/
static void n_stop(void *o, nCmd *)
{
    nObserver2 *self = (nObserver2 *) o;
    self->Stop();
}

//------------------------------------------------------------------------------
/**
    @cmd
    startsinglestep

    @input
    v

    @output
    v

    @info
    Starts observer in single step mode. 
    For example: 
    /sys/servers/time.lockdeltat 0.2 
    .startsinglestep 
    while {.rendersinglestep == "true"} { 
    # do what you want 
    } 
    /sys/servers/time.unlockdeltat 
*/
static void n_startsinglestep(void *o, nCmd *)
{
    nObserver2 *self = (nObserver2 *) o;
    self->StartSingleStep();
}

//------------------------------------------------------------------------------
/**
    @cmd
    rendersinglestep

    @input
    v

    @output
    b (Running)

    @info
    Renders a single frame in the single frame mode. See 
    'startsinglestep' for a code eaxample. 
*/
static void n_rendersinglestep(void *o, nCmd *cmd)
{
    nObserver2 *self = (nObserver2 *) o;
    cmd->Out()->SetB(self->RenderSingleStep());
}

//------------------------------------------------------------------------------
/**
    @cmd
    togglevis

    @input
    v

    @output
    b (Current Visibility)

    @info
    Toggles the observer's visualization of the CD volumes. 
*/
static void n_togglevis(void *o, nCmd *cmd)
{
    nObserver2 *self = (nObserver2 *) o;
    {
        bool f = (bool) self->GetVisualize();
        self->SetVisualize( !f );
		cmd->Out()->SetB( !f );
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsleep

    @input
    f (SleepTime)

    @output
    v

    @info
    Defines the multitasking friendly sleep time per 
    frame in seconds. Default is 0.02. 
*/
static void n_setsleep(void *o, nCmd *cmd)
{
    nObserver2 *self = (nObserver2 *) o;
    {
        float f = cmd->In()->GetF();
        self->SetSleep(f);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsleep

    @input
    v

    @output
    f (SleepTime)

    @info
    Returns the by 'setsleep' defined value. 
*/
static void n_getsleep(void *o, nCmd *cmd)
{
    nObserver2 *self = (nObserver2 *) o;
    {
        float f = (float) self->GetSleep();
        cmd->Out()->SetF(f);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    setgrid

    @input
    b (ShowGrid)

    @output
    v

    @info
    Activates and deactivates the floor grid. 
*/
static void n_setgrid(void *o, nCmd *cmd)
{
    nObserver2 *self = (nObserver2 *) o;
    self->SetGrid(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getgrid

    @input
    v

    @output
    b (ShowGrid)

    @info
    Returns the by 'setgrid' defined value. 
*/
static void n_getgrid(void *o, nCmd *cmd)
{
    nObserver2 *self = (nObserver2 *) o;
    cmd->Out()->SetB(self->GetGrid());
}

//------------------------------------------------------------------------------
/**
    @cmd
    renderframe

    @input
    f (TimeStamp)

    @output
    v

    @info
    Render a single frame with given timestamp.
*/
static void n_renderframe(void *o, nCmd *cmd)
{
    nObserver2 *self = (nObserver2 *) o;
    self->RenderFrame(cmd->In()->GetF());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
