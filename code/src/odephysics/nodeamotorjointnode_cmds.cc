#define N_IMPLEMENTS nOdeAMotorJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeAMotorJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeamotorjointnode.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"
#include "odephysics/nodejoint.h"

static void n_setmode( void* slf, nCmd* cmd );
static void n_getmode( void* slf, nCmd* cmd );
static void n_setnumaxes( void* slf, nCmd* cmd );
static void n_getnumaxes( void* slf, nCmd* cmd );
static void n_setaxis( void* slf, nCmd* cmd );
static void n_getaxis( void* slf, nCmd* cmd );
static void n_getaxisrel( void* slf, nCmd* cmd );
static void n_setangle( void* slf, nCmd* cmd );
static void n_getangle( void* slf, nCmd* cmd );
static void n_getanglerate( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodeamotorjoint

  @superclass
  nodejointnode

  @classinfo
  Provides script access to ODE AMotor joints.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setmode_s", 'SM__', n_setmode );
  clazz->AddCmd( "s_getmode_v", 'GM__', n_getmode );
  clazz->AddCmd( "v_setnumaxes_i", 'SNA_', n_setnumaxes );
  clazz->AddCmd( "i_getnumaxes_v", 'GNA_', n_getnumaxes );
  clazz->AddCmd( "v_setaxis_iifff", 'SA__', n_setaxis );
  clazz->AddCmd( "fff_getaxis_i", 'GA__', n_getaxis );
  clazz->AddCmd( "i_getaxisrel_i", 'GAR_', n_getaxisrel );
  clazz->AddCmd( "v_setangle_if", 'SANG', n_setangle );
  clazz->AddCmd( "f_getangle_i", 'GANG', n_getangle );
  clazz->AddCmd( "f_getanglerate_i", 'GART', n_getanglerate );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setmode

  @input
  s(AMotorMode)

  @output
  v

  @info
  AMotorMode can be one of these values:
  - user
  - euler
  For more info lookup dJointSetAMotorMode in the ODE manual.
*/
static
void
n_setmode( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  self->SetMode( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getmode

  @input
  v

  @output
  s(AMotorMode)

  @info
  See setmode for a list of valid values for AMotorMode.
*/
static
void
n_getmode( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  cmd->Out()->SetS( self->GetMode() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setnumaxes

  @input
  i(NumAxes)

  @output
  v

  @info
  Set the number of angular axes that will be controlled by the AMotor.
  For more info lookup dJointSetAMotorNumAxes in the ODE manual.
*/
static
void
n_setnumaxes( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  ((nOdeAMotorJoint*)self->GetJoint())->SetNumAxes( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getnumaxes

  @input
  v

  @output
  i(NumAxes)

  @info
  Get the number of angular axes that are controlled by the AMotor.
  For more info lookup dJointGetAMotorNumAxes in the ODE manual.
*/
static
void
n_getnumaxes( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  cmd->Out()->SetI( ((nOdeAMotorJoint*)self->GetJoint())->GetNumAxes() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setaxis

  @input
  i(AxisNum), i(RelMode), f(x), f(y), f(z)

  @output
  v

  @info
  Set an AMotor axis.
  For more info lookup dJointSetAMotorAxis in the ODE manual.
*/
static
void
n_setaxis( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  int anum = cmd->In()->GetI();
  int rel = cmd->In()->GetI();
  vector3 axis;
  axis.x = cmd->In()->GetF();
  axis.y = cmd->In()->GetF();
  axis.z = cmd->In()->GetF();
  ((nOdeAMotorJoint*)self->GetJoint())->SetAxis( anum, rel, axis );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getaxis

  @input
  i(AxisNum)

  @output
  f(x), f(y), f(z)

  @info
  Get an AMotor axis.
  For more info lookup dJointGetAMotorAxis in the ODE manual.
*/
static
void
n_getaxis( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  int anum = cmd->In()->GetI();
  vector3 v;
  ((nOdeAMotorJoint*)self->GetJoint())->GetAxis( anum, &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getaxisrel

  @input
  i(AxisNum)

  @output
  i(RelMode)

  @info
  Get the relative orientation mode for the specified axis.
  For more info lookup dJointGetAMotorAxisRel in the ODE manual.
*/
static
void
n_getaxisrel( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  int anum = cmd->In()->GetI();
  cmd->Out()->SetI( ((nOdeAMotorJoint*)self->GetJoint())->GetAxisRel( anum ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setangle

  @input
  i(AxisNum), f(Angle)

  @output
  v

  @info
  Tell the AMotor what the current angle is along axis AxisNum.
  For more info lookup dJointSetAMotorAngle in the ODE manual.
*/
static
void
n_setangle( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  int anum = cmd->In()->GetI();
  float angle = cmd->In()->GetF();
  ((nOdeAMotorJoint*)self->GetJoint())->SetAngle( anum, angle );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getangle

  @input
  i(AxisNum)

  @output
  f(Angle)

  @info
  Return the current angle for axis AxisNum.
  For more info lookup dJointGetAMotorAngle in the ODE manual.
*/
static
void
n_getangle( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  int anum = cmd->In()->GetI();
  cmd->Out()->SetF( ((nOdeAMotorJoint*)self->GetJoint())->GetAngle( anum ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getanglerate

  @input
  i(AxisNum)

  @output
  f(AngleRate)

  @info
  Return the current angle rate for axis AxisNum.
  For more info lookup dJointGetAMotorAngleRate in the ODE manual.
*/
static
void
n_getanglerate( void* slf, nCmd* cmd )
{
  nOdeAMotorJointNode* self = (nOdeAMotorJointNode*)slf;
  int anum = cmd->In()->GetI();
  cmd->Out()->SetF( ((nOdeAMotorJoint*)self->GetJoint())->GetAngleRate( anum ) );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeAMotorJointNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeJointNode::SaveCmds( ps ) )
  {
    nCmd* cmd;
    nOdeAMotorJoint* j = (nOdeAMotorJoint*)this->GetJoint();
    
    // setnumaxes
    cmd = ps->GetCmd( this, 'SNA_' );
    cmd->In()->SetI( j->GetNumAxes() );
    ps->PutCmd( cmd );
    
    // setmode
    cmd = ps->GetCmd( this, 'SM__' );
    cmd->In()->SetS( this->GetMode() );
    ps->PutCmd( cmd );
    
    vector3 v;
    const int numParams = 11;
    static const char* paramNames[] = { "lostop", "histop", "vel", "fmax",
                                      "fudgefactor", "bounce", "cfm",
                                      "stoperp", "stopcfm", 
                                      "suspensionerp", "suspensioncfm" };
    
    for ( int axisNum = 0; axisNum < 3; axisNum++ )
    {
      j->GetAxis( axisNum, &v );
      int rel = j->GetAxisRel( axisNum );
      float angle = j->GetAngle( axisNum );
      
      // setaxis
      cmd = ps->GetCmd( this, 'SA__' );
      cmd->In()->SetI( axisNum );
      cmd->In()->SetI( rel );
      cmd->In()->SetF( v.x );
      cmd->In()->SetF( v.y );
      cmd->In()->SetF( v.z );
      ps->PutCmd( cmd );
      
      // setangle
      cmd = ps->GetCmd( this, 'SANG' );
      cmd->In()->SetI( axisNum );
      cmd->In()->SetF( angle );
      ps->PutCmd( cmd );
      
      // save parameters
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
