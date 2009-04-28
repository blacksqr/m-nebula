#define N_IMPLEMENTS nOdeUniversalJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeUniversalJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeuniversaljointnode.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"
#include "odephysics/nodejoint.h"

static void n_setanchor( void* slf, nCmd* cmd );
static void n_getanchor( void* slf, nCmd* cmd );
static void n_getanchor2( void* slf, nCmd* cmd );
static void n_setaxis1( void* slf, nCmd* cmd );
static void n_getaxis1( void* slf, nCmd* cmd );
static void n_setaxis2( void* slf, nCmd* cmd );
static void n_getaxis2( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodeuniversaljointnode

  @superclass
  nodejointnode

  @classinfo
  Provides script access to ODE Universal joints.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setanchor_fff", 'SANC', n_setanchor );
  clazz->AddCmd( "fff_getanchor_v", 'GANC', n_getanchor );
  clazz->AddCmd( "fff_getanchor2_v", 'GAN2', n_getanchor2 );
  clazz->AddCmd( "v_setaxis1_fff", 'SAX1', n_setaxis1 );
  clazz->AddCmd( "fff_getaxis1_v", 'GAX1', n_getaxis1 );
  clazz->AddCmd( "v_setaxis2_fff", 'SAX2', n_setaxis2 );
  clazz->AddCmd( "fff_getaxis2_v", 'GAX2', n_getaxis2 );
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
  Set the anchor point.
  For more info lookup dJointSetUniversalAnchor in the ODE manual.
*/
static
void
n_setanchor( void* slf, nCmd* cmd )
{
  nOdeUniversalJointNode* self = (nOdeUniversalJointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeUniversalJoint*)self)->SetAnchor( v );
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
  Get the anchor point.
  For more info lookup dJointGetUniversalAnchor in the ODE manual.
*/
static
void
n_getanchor( void* slf, nCmd* cmd )
{
  nOdeUniversalJointNode* self = (nOdeUniversalJointNode*)slf;
  vector3 v;
  ((nOdeUniversalJoint*)self)->GetAnchor( &v );
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
  Get the anchor point.
  For more info lookup dJointGetUniversalAnchor2 in the ODE manual.
*/
static
void
n_getanchor2( void* slf, nCmd* cmd )
{
  nOdeUniversalJointNode* self = (nOdeUniversalJointNode*)slf;
  vector3 v;
  ((nOdeUniversalJoint*)self)->GetAnchor2( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setaxis1

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set Axis1.
  For more info lookup dJointSetUniversalAxis1 in the ODE manual.
*/
static
void
n_setaxis1( void* slf, nCmd* cmd )
{
  nOdeUniversalJointNode* self = (nOdeUniversalJointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeUniversalJoint*)self)->SetAxis1( v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getaxis1

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get Axis1.
  For more info lookup dJointGetUniversalAxis1 in the ODE manual.
*/
static
void
n_getaxis1( void* slf, nCmd* cmd )
{
  nOdeUniversalJointNode* self = (nOdeUniversalJointNode*)slf;
  vector3 v;
  ((nOdeUniversalJoint*)self)->GetAxis1( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setaxis2

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set Axis2.
  For more info lookup dJointSetUniversalAxis2 in the ODE manual.
*/
static
void
n_setaxis2( void* slf, nCmd* cmd )
{
  nOdeUniversalJointNode* self = (nOdeUniversalJointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeUniversalJoint*)self)->SetAxis2( v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getaxis2

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get Axis2.
  For more info lookup dJointGetUniversalAxis2 in the ODE manual.
*/
static
void
n_getaxis2( void* slf, nCmd* cmd )
{
  nOdeUniversalJointNode* self = (nOdeUniversalJointNode*)slf;
  vector3 v;
  ((nOdeUniversalJoint*)self)->GetAxis2( &v );
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
nOdeUniversalJointNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeJointNode::SaveCmds( ps ) )
  {
    nCmd* cmd;
    vector3 v;
    nOdeUniversalJoint* j = (nOdeUniversalJoint*)this->GetJoint();
    
    // setanchor
    j->GetAnchor( &v );
    cmd = ps->GetCmd( this, 'SANC' );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
    // setaxis1
    j->GetAxis1( &v );
    cmd = ps->GetCmd( this, 'SAX1' );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
    // setaxis2
    j->GetAxis2( &v );
    cmd = ps->GetCmd( this, 'SAX2' );
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
    for ( int axisNum = 0; axisNum < 2; axisNum++ )
    {
      for ( int i = 0; i < numParams; i++ )
      {
        cmd = ps->GetCmd( this, 'SP__' );
        cmd->In()->SetS( paramNames[i] );
        cmd->In()->SetI( axisNum );
        cmd->In()->SetF( this->GetParam( paramNames[i], axisNum ) );
        ps->PutCmd( cmd );
      }
    }

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
