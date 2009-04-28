#define N_IMPLEMENTS nOdeSliderJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeSliderJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodesliderjointnode.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"
#include "odephysics/nodejoint.h"

static void n_setaxis( void* slf, nCmd* cmd );
static void n_getaxis( void* slf, nCmd* cmd );
static void n_getposition( void* slf, nCmd* cmd );
static void n_getpositionrate( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodesliderjointnode

  @superclass
  nodejointnode

  @classinfo
  Provides script access to ODE Slider joints.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setaxis_fff", 'SAX_', n_setaxis );
  clazz->AddCmd( "fff_getaxis_v", 'GAX_', n_getaxis );
  clazz->AddCmd( "f_getposition_v", 'GPOS', n_getposition );
  clazz->AddCmd( "f_getpositionrate_v", 'GPR_', n_getpositionrate );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setaxis

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set the slider axis.
  For more info lookup dJointSetSliderAxis in the ODE manual.
*/
static
void
n_setaxis( void* slf, nCmd* cmd )
{
  nOdeSliderJointNode* self = (nOdeSliderJointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeSliderJoint*)self)->SetAxis( v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getaxis

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the slider axis.
  For more info lookup dJointGetSliderAxis in the ODE manual.
*/
static
void
n_getaxis( void* slf, nCmd* cmd )
{
  nOdeSliderJointNode* self = (nOdeSliderJointNode*)slf;
  vector3 v;
  ((nOdeSliderJoint*)self)->GetAxis( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getposition

  @input
  v

  @output
  f(Position)

  @info
  Get the slider's linear position.
  For more info lookup dJointGetSliderPosition in the ODE manual.
*/
static
void
n_getposition( void* slf, nCmd* cmd )
{
  nOdeSliderJointNode* self = (nOdeSliderJointNode*)slf;
  cmd->Out()->SetF( ((nOdeSliderJoint*)self)->GetPosition() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getpositionrate

  @input
  v

  @output
  f(PositionRate)

  @info
  For more info lookup dJointGetSliderPositionRate in the ODE manual.
*/
static
void
n_getpositionrate( void* slf, nCmd* cmd )
{
  nOdeSliderJointNode* self = (nOdeSliderJointNode*)slf;
  cmd->Out()->SetF( ((nOdeSliderJoint*)self)->GetPositionRate() );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeSliderJointNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeJointNode::SaveCmds( ps ) )
  {
    vector3 v;
    ((nOdeSliderJoint*)this->GetJoint())->GetAxis( &v );
    
    // setaxis
    nCmd* cmd = ps->GetCmd( this, 'SAX_' );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
    const int numParams = 11;
    static const char* paramNames[] = { "lostop", "histop", "vel", "fmax",
                                      "fudgefactor", "bounce", "cfm",
                                      "stoperp", "stopcfm", 
                                      "suspensionerp", "suspensioncfm" };
    
    // save parameters
    for ( int i = 0; i < numParams; i++ )
    {
      cmd = ps->GetCmd( this, 'SP__' );
      cmd->In()->SetS( paramNames[i] );
      cmd->In()->SetI( 0 );
      cmd->In()->SetF( this->GetParam( paramNames[i], 0 ) );
      ps->PutCmd( cmd );
    }

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
