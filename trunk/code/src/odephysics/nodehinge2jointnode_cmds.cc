#define N_IMPLEMENTS nOdeHinge2JointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeHinge2JointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodehinge2jointnode.h"
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
static void n_getangle1( void* slf, nCmd* cmd );
static void n_getangle1rate( void* slf, nCmd* cmd );
static void n_getangle2rate( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodehinge2jointnode

  @superclass
  nodejointnode

  @classinfo
  Provides script access to ODE Hinge-2 joints.
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
  clazz->AddCmd( "f_getangle1_v", 'GA1_', n_getangle1 );
  clazz->AddCmd( "f_getangle1rate_v", 'GA1R', n_getangle1rate );
  clazz->AddCmd( "f_getangle2rate_v", 'GA2R', n_getangle2rate );
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
  Set hinge-2 anchor.
  For more info lookup dJointSetHinge2Anchor in the ODE manual.
*/
static
void
n_setanchor( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeHinge2Joint*)self->GetJoint())->SetAnchor( v );
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
  Get hinge-2 anchor.
  For more info lookup dJointGetHinge2Anchor in the ODE manual.
*/
static
void
n_getanchor( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  vector3 v;
  ((nOdeHinge2Joint*)self->GetJoint())->GetAnchor( &v );
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
  Get hinge-2 anchor.
  For more info lookup dJointGetHinge2Anchor2 in the ODE manual.
*/
static
void
n_getanchor2( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  vector3 v;
  ((nOdeHinge2Joint*)self->GetJoint())->GetAnchor2( &v );
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
  For more info lookup dJointSetHinge2Axis1 in the ODE manual.
*/
static
void
n_setaxis1( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeHinge2Joint*)self->GetJoint())->SetAxis1( v );
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
  For more info lookup dJointGetHinge2Axis1 in the ODE manual.
*/
static
void
n_getaxis1( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  vector3 v;
  ((nOdeHinge2Joint*)self->GetJoint())->GetAxis1( &v );
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
  For more info lookup dJointSetHinge2Axis2 in the ODE manual.
*/
static
void
n_setaxis2( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  ((nOdeHinge2Joint*)self->GetJoint())->SetAxis2( v );
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
  For more info lookup dJointGetHinge2Axis2 in the ODE manual.
*/
static
void
n_getaxis2( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  vector3 v;
  ((nOdeHinge2Joint*)self->GetJoint())->GetAxis2( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getangle1

  @input
  v

  @output
  f(Angle)

  @info
  For more info lookup dJointGetHinge2Angle1 in the ODE manual.
*/
static
void
n_getangle1( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  cmd->Out()->SetF( ((nOdeHinge2Joint*)self->GetJoint())->GetAngle1() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getangle1rate

  @input
  v

  @output
  f(AngleRate)

  @info
  For more info lookup dJointGetHinge2Angle1Rate in the ODE manual.
*/
static
void
n_getangle1rate( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  cmd->Out()->SetF( ((nOdeHinge2Joint*)self->GetJoint())->GetAngle1Rate() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getangle2rate

  @input
  v

  @output
  f(AngleRate)

  @info
  For more info lookup dJointGetHinge2Angle2Rate in the ODE manual.
*/
static
void
n_getangle2rate( void* slf, nCmd* cmd )
{
  nOdeHinge2JointNode* self = (nOdeHinge2JointNode*)slf;
  cmd->Out()->SetF( ((nOdeHinge2Joint*)self->GetJoint())->GetAngle2Rate() );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeHinge2JointNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeJointNode::SaveCmds( ps ) )
  {
    nCmd* cmd;
    vector3 v;
    nOdeHinge2Joint* j = (nOdeHinge2Joint*)this->GetJoint();

    // setanchor    
    cmd = ps->GetCmd( this, 'SANC' );
    j->GetAnchor( &v );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
    // setaxis1
    cmd = ps->GetCmd( this, 'SAX1' );
    j->GetAxis1( &v );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
    // setaxis2
    cmd = ps->GetCmd( this, 'SAX2' );
    j->GetAxis1( &v );
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
