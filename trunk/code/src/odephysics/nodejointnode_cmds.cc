#define N_IMPLEMENTS nOdeJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodejointnode.h"
#include "kernel/npersistserver.h"
#include "odephysics/nodejoint.h"
#include "mathlib/vector.h"

static void n_initjoint( void* slf, nCmd* cmd );
static void n_setautocleanup( void* slf, nCmd* cmd );
static void n_getautocleanup( void* slf, nCmd* cmd );
static void n_attachto( void* slf, nCmd* cmd );
static void n_getjointtype( void* slf, nCmd* cmd );
static void n_getbody( void* slf, nCmd* cmd );
static void n_enablefeedback( void* slf, nCmd* cmd );
static void n_isfeedbackenabled( void* slf, nCmd* cmd );
static void n_getfeedback( void* slf, nCmd* cmd );
static void n_setparam( void* slf, nCmd* cmd );
static void n_getparam( void* slf, nCmd* cmd );

#ifdef ODE_BREAKABLE_JOINTS
static void n_setbreakable( void* slf, nCmd* cmd );
static void n_isbreakable( void* slf, nCmd* cmd );
static void n_setbreakmode( void* slf, nCmd* cmd );
static void n_getbreakmode( void* slf, nCmd* cmd );
static void n_setbreakmodeflag( void* slf, nCmd* cmd );
static void n_getbreakmodeflag( void* slf, nCmd* cmd );
static void n_setbreakforce( void* slf, nCmd* cmd );
static void n_getbreakforce( void* slf, nCmd* cmd );
static void n_setbreaktorque( void* slf, nCmd* cmd );
static void n_getbreaktorque( void* slf, nCmd* cmd );
#endif // ODE_BREAKABLE_JOINTS

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodejointnode

  @superclass
  nroot

  @classinfo
  Provides script access to ODE joints.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_initjoint_s", 'IJ__', n_initjoint );
  clazz->AddCmd( "v_setautocleanup_b", 'SAC_', n_setautocleanup );
  clazz->AddCmd( "b_getautocleanup_v", 'GAC_', n_getautocleanup );
  clazz->AddCmd( "v_attachto_ss", 'AT__', n_attachto );
  clazz->AddCmd( "s_getjointtype_v", 'GJT_', n_getjointtype );
  clazz->AddCmd( "s_getbody_i", 'GB__', n_getbody );
  clazz->AddCmd( "v_enablefeedback_b", 'EFB_', n_enablefeedback );
  clazz->AddCmd( "b_isfeedbackenabled_v", 'IFBE', n_isfeedbackenabled );
  clazz->AddCmd( "ffffffffffff_getfeedback_v", 'GF__', n_getfeedback );
  clazz->AddCmd( "v_setparam_sif", 'SP__', n_setparam );
  clazz->AddCmd( "f_getparam_si", 'GP__', n_getparam );
#ifdef ODE_BREAKABLE_JOINTS  
  clazz->AddCmd( "v_setbreakable_b", 'SBR_', n_setbreakable );
  clazz->AddCmd( "b_isbreakable_v", 'ISBR', n_isbreakable );
  clazz->AddCmd( "v_setbreakmode_i", 'SBRM', n_setbreakmode );
  clazz->AddCmd( "i_getbreakmode_v", 'GBRM', n_getbreakmode );
  clazz->AddCmd( "v_setbreakmodeflag_sb", 'SBMF', n_setbreakmodeflag );
  clazz->AddCmd( "b_getbreakmodeflag_s", 'GBMF', n_getbreakmodeflag );
  clazz->AddCmd( "v_setbreakforce_ifff", 'SBF_', n_setbreakforce );
  clazz->AddCmd( "fff_getbreakforce_i", 'GBF_', n_getbreakforce );
  clazz->AddCmd( "v_setbreaktorque_ifff", 'SBT_', n_setbreaktorque );
  clazz->AddCmd( "fff_getbreaktorque_i", 'GBT_', n_getbreaktorque );
#endif // ODE_BREAKABLE_JOINTS  
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  initjoint

  @input
  s(PhysicsContext)

  @output
  v 

  @info
  Initialise the joint, must be called before trying to use any of the other 
  commands.
*/
static
void
n_initjoint( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  self->InitJoint( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setautocleanup

  @input
  b(True/False)

  @output
  v

  @info
  If enabled the lightweight C++ joint will be destroyed along with the node.
  This is enabled by default.
*/
static
void
n_setautocleanup( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  self->SetAutoCleanup( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getautocleanup

  @input
  v

  @output
  b(True/False) 

  @info
  Check if the lightweight C++ instance will be destroyed along with the node.
*/
static
void
n_getautocleanup( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  cmd->Out()->SetB( self->GetAutoCleanup() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  attachto

  @input
  s(OdeBodyNode0), s(OdeBodyNode1)

  @output
  v

  @info
  Attach the joint to the bodies. One of the OdeBodyNodes can be "none", this 
  means one end of the joint is attached to the world. The OdeBodyNodes must
  be distinct.
*/
static
void
n_attachto( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  const char* b1 = cmd->In()->GetS();
  const char* b2 = cmd->In()->GetS();
  self->AttachTo( b1, b2 );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getjointtype

  @input
  v

  @output
  s(JointType)

  @info
  JointType will match one of these values:
  - ball
  - hinge
  - slider
  - contact
  - universal
  - hinge2
  - fixed
  - amotor
*/
static
void
n_getjointtype( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  cmd->Out()->SetS( self->GetType() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getbody

  @input
  i(BodyIndex)

  @output
  s(OdeBodyNode)

  @info
  BodyIndex can be either 0 (first body) or 1 (second body).
  OdeBodyNode may be "none", this means that one end of the joint is
  attached to the world.
*/
static
void
n_getbody( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  cmd->Out()->SetS( self->GetBody( cmd->In()->GetI() ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  enablefeedback

  @input
  b(True/False)

  @output
  v

  @info
  Enable joint feedback.
  For more info lookup dJointSetFeedback in the ODE manual.
*/
static
void
n_enablefeedback( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  self->GetJoint()->EnableFeedback( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  isfeedbackenabled

  @input
  v

  @output
  b(True/False)

  @info
  Check if joint feedback is enabled.
  For more info lookup dJointSetFeedback in the ODE manual.
*/
static
void
n_isfeedbackenabled( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  cmd->Out()->SetB( self->GetJoint()->IsFeedbackEnabled() );
}


//------------------------------------------------------------------------------
/**
  @cmd
  getfeedback

  @input
  v

  @output
  f(f1.x), f(f1.y), f(f1.z), f(t1.x), f(t1.y), f(t1.z), 
  f(f2.x), f(f2.y), f(f2.z), f(t2.x), f(t2.y), f(t2.z)

  @info
  For more info lookup dJointGetFeedback in the ODE manual.
*/
static
void
n_getfeedback( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  vector3 f1, t1, f2, t2;
  self->GetJoint()->GetFeedback( &f1, &t1, &f2, &t2 );
  cmd->Out()->SetF( f1.x );
  cmd->Out()->SetF( f1.y );
  cmd->Out()->SetF( f1.z );
  cmd->Out()->SetF( t1.x );
  cmd->Out()->SetF( t1.y );
  cmd->Out()->SetF( t1.z );
  cmd->Out()->SetF( f2.x );
  cmd->Out()->SetF( f2.y );
  cmd->Out()->SetF( f2.z );
  cmd->Out()->SetF( t2.x );
  cmd->Out()->SetF( t2.y );
  cmd->Out()->SetF( t2.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setparam

  @input
  s(ParamName), i(AxisNum), f(ParamValue)

  @output
  v

  @info
  Set a joint parameter. 
  ParamName must be one of:
    - lostop
    - histop
    - vel
    - fmax
    - fudgefactor
    - bounce
    - cfm
    - stoperp
    - stopcfm
    - suspensionerp
    - suspensioncfm
  AxisNum must be 0, 1, or 2.
     
  For more info lookup dJointSet*Param in the ODE manual.
*/
static
void
n_setparam( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  const char* pName = cmd->In()->GetS();
  int pAxis = cmd->In()->GetI();
  float pVal = cmd->In()->GetF();
  self->SetParam( pName, pAxis, pVal );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getparam

  @input
  s(ParamName), i(AxisNum)

  @output
  f(ParamValue)

  @info
  Get the value of a joint parameter.
  Valid values for ParamName and AxisNum are the same as for setparam.
  For more info lookup dJointGet*Param in the ODE manual.
*/
static
void
n_getparam( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  const char* pName = cmd->In()->GetS();
  int pAxis = cmd->In()->GetI();
  cmd->Out()->SetF( self->GetParam( pName, pAxis ) );
}

#ifdef ODE_BREAKABLE_JOINTS

//------------------------------------------------------------------------------
/**
  @cmd
  setbreakable

  @input
  b(True/False)

  @output
  v

  @info
  Make the joint breakable, or unbreakable.
*/
static 
void 
n_setbreakable( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  self->GetJoint()->SetBreakable( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  isbreakable

  @input
  v

  @output
  b(True/False)

  @info
  Check if the joint is breakable.
*/
static 
void 
n_isbreakable( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  cmd->Out()->SetB( self->GetJoint()->IsBreakable() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setbreakmode

  @input
  v

  @output
  i(BreakMode)

  @info
  Lookup dJointSetBreakMode in the ODE docs for more info.
*/
static 
void 
n_setbreakmode( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  self->GetJoint()->SetBreakMode( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getbreakmode

  @input
  v

  @output
  i(BreakMode)

  @info
  Lookup dJointSetBreakMode in the ODE docs for more info.
*/
static 
void 
n_getbreakmode( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  cmd->Out()->SetI( self->GetJoint()->GetBreakMode() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setbreakmodeflag

  @input
  v

  @output
  s(Flag), b(On/Off)

  @info
  Toggle a break mode flag.
  Flag can be one of:
    - b1force  (If the force on body 1 is too high the joint will break)
    - b1torque (If the torque on body 1 is too high the joint will break)
    - b2force  (If the force on body 2 is too high the joint will break)
    - b2torque (If the torque on body 2 is too high the joint will break)
*/
static 
void 
n_setbreakmodeflag( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  const char* flag = cmd->In()->GetS();
  bool val = cmd->In()->GetB();
  self->SetBreakModeFlag( flag, val );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getbreakmodeflag

  @input
  s(Flag)

  @output
  b(On/Off)

  @info
  Check if a break mode flag is on/off.
  Flag can be one of:
    - b1force
    - b1torque
    - b2force
    - b2torque
*/
static 
void 
n_getbreakmodeflag( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  const char* flag = cmd->In()->GetS();
  cmd->Out()->SetB( self->GetBreakModeFlag( cmd->In()->GetS() ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setbreakforce

  @input
  i(BodyIndex), f(x), f(y), f(z)

  @output
  v

  @info
  BodyIndex must be either 0 or 1.
  Lookup dJointSetBreakForce in the ODE docs for more info.
*/
static 
void 
n_setbreakforce( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  int body = cmd->In()->GetI();
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->GetJoint()->SetBreakForce( body, v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getbreakforce

  @input
  i(BodyIndex)

  @output
  f(x), f(y), f(z)

  @info
  BodyIndex must be either 0 or 1.
  Lookup dJointGetBreakForce in the ODE docs for more info.
*/
static 
void 
n_getbreakforce( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  int body = cmd->In()->GetI();
  vector3 v;
  self->GetJoint()->GetBreakForce( body, &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setbreaktorque

  @input
  i(BodyIndex), f(x), f(y), f(z)

  @output
  v

  @info
  BodyIndex must be either 0 or 1.
  Lookup dJointSetBreakTorque in the ODE docs for more info.
*/
static 
void 
n_setbreaktorque( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  int body = cmd->In()->GetI();
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->GetJoint()->SetBreakTorque( body, v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getbreaktorque

  @input
  i(BodyIndex)

  @output
  f(x), f(y), f(z)

  @info
  BodyIndex must be either 0 or 1.
  Lookup dJointGetBreakTorque in the ODE docs for more info.
*/
static 
void 
n_getbreaktorque( void* slf, nCmd* cmd )
{
  nOdeJointNode* self = (nOdeJointNode*)slf;
  int body = cmd->In()->GetI();
  vector3 v;
  self->GetJoint()->GetBreakTorque( body, &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

#endif // ODE_BREAKABLE_JOINTS

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeJointNode::SaveCmds( nPersistServer* ps )
{
  if ( nRoot::SaveCmds( ps ) )
  {
    nCmd* cmd;
    nOdeJoint* j = this->GetJoint();
    
    // initjoint
    cmd = ps->GetCmd( this, 'IJ__' );
    cmd->In()->SetS( this->ref_PhysContext.getname() );
    ps->PutCmd( cmd );
    
    // attachto
    cmd = ps->GetCmd( this, 'AT__' );
    cmd->In()->SetS( this->GetBody( 0 ) );
    cmd->In()->SetS( this->GetBody( 1 ) );
    ps->PutCmd( cmd );
    
    // enablefeedback
    cmd = ps->GetCmd( this, 'EFB_' );
    cmd->In()->SetB( j->IsFeedbackEnabled() );
    ps->PutCmd( cmd );

#ifdef ODE_BREAKABLE_JOINTS
    vector3 v;
    
    // setbreakable
    cmd = ps->GetCmd( this, 'SBR_' );
    cmd->In()->SetB( j->IsBreakable() );
    ps->PutCmd( cmd );
    
    // setbreakmode
    cmd = ps->GetCmd( this, 'SBRM' );
    cmd->In()->SetI( j->GetBreakMode() );
    ps->PutCmd( cmd );
    
    // setbreakforce b1
    cmd = ps->GetCmd( this, 'SBF_' );
    j->GetBreakForce( 0, &v );
    cmd->In()->SetI( 0 );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
    // setbreaktorque b1
    cmd = ps->GetCmd( this, 'SBT_' );
    j->GetBreakTorque( 0, &v );
    cmd->In()->SetI( 0 );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
        
    // setbreakforce b2
    cmd = ps->GetCmd( this, 'SBF_' );
    j->GetBreakForce( 1, &v );
    cmd->In()->SetI( 1 );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
    // setbreaktorque b2
    cmd = ps->GetCmd( this, 'SBT_' );
    j->GetBreakTorque( 1, &v );
    cmd->In()->SetI( 1 );
    cmd->In()->SetF( v.x );
    cmd->In()->SetF( v.y );
    cmd->In()->SetF( v.z );
    ps->PutCmd( cmd );
    
#endif // ODE_BREAKABLE_JOINTS

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
