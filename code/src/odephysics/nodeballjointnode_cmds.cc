#define N_IMPLEMENTS nOdeBallJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeBallJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeballjointnode.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"
#include "odephysics/nodejoint.h"

static void n_setanchor( void* slf, nCmd* cmd );
static void n_getanchor( void* slf, nCmd* cmd );
static void n_getanchor2( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodeballjointnode

  @superclass
  nodejointnode

  @classinfo
  Provides script access to ODE ball-socket joints.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setanchor_fff", 'SA__', n_setanchor );
  clazz->AddCmd( "fff_getanchor_v", 'GA__', n_getanchor );
  clazz->AddCmd( "fff_getanchor2_v", 'GA2_', n_getanchor2 );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setanchor

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set the joint anchor point.
  For more info lookup dJointSetBallAnchor in the ODE manual.
*/
static
void
n_setanchor( void* slf, nCmd* cmd )
{
  nOdeBallJointNode* self = (nOdeBallJointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeBallJoint*)self->GetJoint())->SetAnchor( v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getanchor

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the joint anchor point.
  For more info lookup dJointGetBallAnchor in the ODE manual.
*/
static
void
n_getanchor( void* slf, nCmd* cmd )
{
  nOdeBallJointNode* self = (nOdeBallJointNode*)slf;
  vector3 v;
  ((nOdeBallJoint*)self->GetJoint())->GetAnchor( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getanchor2

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the joint anchor point.
  For more info lookup dJointGetBallAnchor2 in the ODE manual.
*/
static
void
n_getanchor2( void* slf, nCmd* cmd )
{
  nOdeBallJointNode* self = (nOdeBallJointNode*)slf;
  vector3 v;
  ((nOdeBallJoint*)self->GetJoint())->GetAnchor2( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeBallJointNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeJointNode::SaveCmds( ps ) )
  {
    vector3 v;
    ((nOdeBallJoint*)this->GetJoint())->GetAnchor( &v );
    
    nCmd* cmd = ps->GetCmd( this, 'SA__' );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
