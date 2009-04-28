#define N_IMPLEMENTS nOdeHingeJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeHingeJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodehingejointnode.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"
#include "odephysics/nodejoint.h"

static void n_setanchor( void* slf, nCmd* cmd );
static void n_setaxis( void* slf, nCmd* cmd );
static void n_getanchor( void* slf, nCmd* cmd );
static void n_getanchor2( void* slf, nCmd* cmd );
static void n_getaxis( void* slf, nCmd* cmd );
static void n_getangle( void* slf, nCmd* cmd );
static void n_getanglerate( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodehingejointnode

  @superclass
  nodejointnode

  @classinfo
  Provides script access to ODE Hinge joints.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setanchor_fff", 'SANC', n_setanchor );
  clazz->AddCmd( "v_setaxis_fff", 'SAX_', n_setaxis );
  clazz->AddCmd( "fff_getanchor_v", 'GANC', n_getanchor );
  clazz->AddCmd( "fff_getanchor2_v", 'GAN2', n_getanchor2 );
  clazz->AddCmd( "fff_getaxis_v", 'GAX_', n_getaxis );
  clazz->AddCmd( "f_getangle_v", 'GANG', n_getangle );
  clazz->AddCmd( "f_getanglerate_v", 'GAR_', n_getanglerate );
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
  Set hinge anchor.
  For more info lookup dJointSetHingeAnchor in the ODE manual.
*/
static
void
n_setanchor( void* slf, nCmd* cmd )
{
  nOdeHingeJointNode* self = (nOdeHingeJointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeHingeJoint*)self)->SetAnchor( v );
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
  Set the hinge axis.
  For more info lookup dJointSetHingeAxis in the ODE manual.
*/
static
void
n_setaxis( void* slf, nCmd* cmd )
{
  nOdeHingeJointNode* self = (nOdeHingeJointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeHingeJoint*)self)->SetAxis( v );
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
  Get the hinge anchor point.
  For more info lookup dJointGetHingeAnchor in the ODE manual.
*/
static
void
n_getanchor( void* slf, nCmd* cmd )
{
  nOdeHingeJointNode* self = (nOdeHingeJointNode*)slf;
  vector3 v;
  ((nOdeHingeJoint*)self)->GetAnchor( &v );
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
  Get the hinge anchor point.
  For more info lookup dJointGetHingeAnchor2 in the ODE manual.
*/
static
void
n_getanchor2( void* slf, nCmd* cmd )
{
  nOdeHingeJointNode* self = (nOdeHingeJointNode*)slf;
  vector3 v;
  ((nOdeHingeJoint*)self)->GetAnchor2( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
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
  Get the hinge axis.
  For more info lookup dJointGetHingeAxis in the ODE manual.
*/
static
void
n_getaxis( void* slf, nCmd* cmd )
{
  nOdeHingeJointNode* self = (nOdeHingeJointNode*)slf;
  vector3 v;
  ((nOdeHingeJoint*)self)->GetAxis( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getangle

  @input
  v

  @output
  f(Angle)

  @info
  Get the hinge angle.
  For more info lookup dJointGetHingeAngle in the ODE manual.
*/
static
void
n_getangle( void* slf, nCmd* cmd )
{
  nOdeHingeJointNode* self = (nOdeHingeJointNode*)slf;
  cmd->Out()->SetF( ((nOdeHingeJoint*)self)->GetAngle() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getanglerate

  @input
  v

  @output
  f(AngleRate)

  @info
  For more info lookup dJointGetHingeAngleRate in the ODE manual.
*/
static
void
n_getanglerate( void* slf, nCmd* cmd )
{
  nOdeHingeJointNode* self = (nOdeHingeJointNode*)slf;
  cmd->Out()->SetF( ((nOdeHingeJoint*)self)->GetAngleRate() );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeHingeJointNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeJointNode::SaveCmds( ps ) )
  {
    nCmd* cmd;
    vector3 v;
    nOdeHingeJoint* j = (nOdeHingeJoint*)this->GetJoint();
    
    // setanchor    
    cmd = ps->GetCmd( this, 'SANC' );
    j->GetAnchor( &v );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
    // setaxis
    cmd = ps->GetCmd( this, 'SAX_' );
    j->GetAxis( &v );
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
