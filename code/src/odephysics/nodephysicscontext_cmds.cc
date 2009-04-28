#define N_IMPLEMENTS nOdePhysicsContext
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//
//  nOdePhysicsContext is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodephysicscontext.h"
#include "kernel/npersistserver.h"

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif


static void n_setgravity( void* slf, nCmd* cmd );
static void n_getgravity( void* slf, nCmd* cmd );
static void n_seterp( void* slf, nCmd* cmd );
static void n_geterp( void* slf, nCmd* cmd );
static void n_setcfm( void* slf, nCmd* cmd );
static void n_getcfm( void* slf, nCmd* cmd );
static void n_impulsetoforce( void* slf, nCmd* cmd );
static void n_setautoenabledepth( void* slf, nCmd* cmd );
static void n_getautoenabledepth( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nodephysicscontext
    
    @superclass
    nroot

    @classinfo
    A physics context corresponds to an ODE world, so go read ODE docs :)
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_setgravity_fff", 'SGRV', n_setgravity );
    clazz->AddCmd( "fff_getgravity_v", 'GGRV', n_getgravity );
    clazz->AddCmd( "v_seterp_f", 'SERP', n_seterp );
    clazz->AddCmd( "f_geterp_v", 'GERP', n_geterp );
    clazz->AddCmd( "v_setcfm_f", 'SCFM', n_setcfm );
    clazz->AddCmd( "f_getcfm_v", 'GCFM', n_getcfm );
    clazz->AddCmd( "fff_impulsetoforce_ffff", 'ITOF', n_impulsetoforce );
    clazz->AddCmd( "v_setautoenabledepth_i", 'SAED', n_setautoenabledepth );
    clazz->AddCmd( "i_getautoenabledepth_v", 'GAED', n_getautoenabledepth );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setgravity

    @input
    f ( gravity on x-axis ), f ( gravity on y-axis ), f ( gravity on z-axis )

    @output
    v

    @info
    Set gravity.
*/
static
void
n_setgravity( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  vector3 vec( x, y, z );
  self->SetGravity( vec );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getgravity

    @input
    v

    @output
    f ( gravity on x-axis ), f ( gravity on y-axis ), f ( gravity on z-axis )

    @info
    Get gravity.
*/
static
void
n_getgravity( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  vector3 vec;
  self->GetGravity( &vec );
  cmd->Out()->SetF( vec.x );
  cmd->Out()->SetF( vec.y );
  cmd->Out()->SetF( vec.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    seterp

    @input
    f ( erp value between 0 and 1 )

    @output
    v

    @info
    Set the global ERP value, that controls how much error correction is 
    performed in each time step. Typical values are in the range 0.1 to 0.8.
*/
static
void
n_seterp( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  self->SetERP( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    geterp

    @input
    v

    @output
    f ( erp value )

    @info
    Get the current ERP value.
*/
static
void
n_geterp( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  cmd->Out()->SetF( self->GetERP() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcfm

    @input
    f ( constraint mixing force value in range 0 to 1 )

    @output
    v

    @info
    Set global CFM value. Typical values are in the range 1e-9 to 1.
*/
static
void
n_setcfm( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  self->SetCFM( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcfm

    @input
    v

    @output
    f ( constraint force mixing value )

    @info
    Get global CFM value.
*/
static
void
n_getcfm( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  cmd->Out()->SetF( self->GetCFM() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    impulsetoforce

    @input
    f ( step size ), 
    f ( x component of impulse ), f ( y component ), f ( z component )

    @output
    f ( x component of force ), f ( y component ), f ( z component )

    @info
    Converts an impulse to a force. Current algorithm simply scales the 
    impulse by 1/stepsize, where stepsize is the step size for the next 
    step that will be taken.
*/
static
void
n_impulsetoforce( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  float step = cmd->In()->GetF();
  float ix = cmd->In()->GetF();
  float iy = cmd->In()->GetF();
  float iz = cmd->In()->GetF();
  vector3 impulse( ix, iy, iz );
  vector3 force;
  self->ImpulseToForce( step, impulse, &force );
  cmd->Out()->SetF( force.x );
  cmd->Out()->SetF( force.y );
  cmd->Out()->SetF( force.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setautoenabledepth

    @input
    i(AutoEnableDepth)

    @output
    v

    @info
    For more info lookup dWorldSetAutoEnableDepthSF1 in the ODE manual.
*/
static
void
n_setautoenabledepth( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  self->SetAutoEnableDepth( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getautoenabledepth

    @input
    v

    @output
    i(AutoEnableDepth)

    @info
    For more info lookup dWorldGetAutoEnableDepthSF1 in the ODE manual.
*/
static
void
n_getautoenabledepth( void* slf, nCmd* cmd )
{
  nOdePhysicsContext* self = (nOdePhysicsContext*)slf;
  cmd->Out()->SetI( self->GetAutoEnableDepth() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOdePhysicsContext::SaveCmds( nPersistServer* ps )
{
  if ( nRoot::SaveCmds( ps ) )
  {
    nCmd* cmd;
    vector3 g;
    
    // setgravity
    cmd = ps->GetCmd( this, 'SGRV' );
    this->GetGravity( &g );
    cmd->In()->SetF( g.x );
    cmd->In()->SetF( g.y );
    cmd->In()->SetF( g.z );
    ps->PutCmd( cmd );
    
    // seterp
    cmd = ps->GetCmd( this, 'SERP' );
    cmd->In()->SetF( this->GetERP() );
    ps->PutCmd( cmd );
    
    // setcfm
    cmd = ps->GetCmd( this, 'SCFM' );
    cmd->In()->SetF( this->GetCFM() );
    ps->PutCmd( cmd );
    
    // setautoenabledepth
    cmd = ps->GetCmd( this, 'SAED' );
    cmd->In()->SetI( this->GetAutoEnableDepth() );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

