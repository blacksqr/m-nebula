#define N_IMPLEMENTS nOdeJoint
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//
//  nOdeJoint and friends are licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodejoint.h"

#ifndef N_ODEBODY_H
#include "odephysics/nodebody.h"
#endif

#ifndef N_ODEPHYSICSCONTEXT_H
#include "odephysics/nodephysicscontext.h"
#endif


//------------------------------------------------------------------------------
nOdeJoint::nOdeJoint()
  : jointId(0), userdata(0)
{
  
#ifdef ODE_BREAKABLE_JOINTS
  this->JointBroken = 0;
#endif // ODE_BREAKABLE_JOINTS
}

//------------------------------------------------------------------------------
nOdeJoint::~nOdeJoint()
{
  dJointFeedback* fb = dJointGetFeedback( this->jointId );
  if ( fb )
    delete fb;
  dJointDestroy( this->jointId );
  this->jointId = 0;
}

//------------------------------------------------------------------------------
void* nOdeJoint::GetData() const
{
  return this->userdata;
}

//------------------------------------------------------------------------------
void nOdeJoint::SetData( void* data )
{
  this->userdata = data;
}

//------------------------------------------------------------------------------
void nOdeJoint::AttachTo( nOdeBody* b1, nOdeBody* b2 )
{
  dJointAttach( this->jointId, b1->bodyId, b2->bodyId );
}

//------------------------------------------------------------------------------
int nOdeJoint::GetType()
{
  return dJointGetType( this->jointId );
}

//------------------------------------------------------------------------------
nOdeBody* nOdeJoint::GetBody( int index )
{
  return (nOdeBody*)dBodyGetData( dJointGetBody( this->jointId, index ) );
}

//------------------------------------------------------------------------------
void nOdeJoint::EnableFeedback( bool enable )
{
  if ( enable )
  {
    if ( 0 == dJointGetFeedback( this->jointId ) )
      dJointSetFeedback( this->jointId, new dJointFeedback );
  }
  else
  {
    dJointFeedback* fb = dJointGetFeedback( this->jointId );
    dJointSetFeedback( this->jointId, 0 );
    if ( fb )
      delete fb;
  }
}

//------------------------------------------------------------------------------
bool nOdeJoint::IsFeedbackEnabled()
{
  return (0 != dJointGetFeedback( this->jointId ));
}

//------------------------------------------------------------------------------
void nOdeJoint::GetFeedback(  vector3* force1, vector3* torque1,
                              vector3* force2, vector3* torque2 )
{
  dJointFeedback* fb = dJointGetFeedback( this->jointId );
  force1->set( fb->f1[0], fb->f1[1], fb->f1[2] );
  torque1->set( fb->t1[0], fb->t1[1], fb->t1[2] );
  force2->set( fb->f2[0], fb->f2[1], fb->f2[2] );
  torque2->set( fb->t2[0], fb->t2[1], fb->t2[2] );
}

//------------------------------------------------------------------------------
void nOdeJoint::SetParam( int param, float value )
{
  n_warn( "nOdeJoint::SetParam(): Pure virtual method called!" );
}

//------------------------------------------------------------------------------
float nOdeJoint::GetParam( int param )
{
  n_warn( "nOdeJoint::GetParam(): Pure virtual method called!" );
  return 0.0f;
}

#ifdef ODE_BREAKABLE_JOINTS

//------------------------------------------------------------------------------
void nOdeJoint::SetBreakable( bool breakable )
{
  if ( breakable )
  {
    dJointSetBreakable( this->jointId, 1 );
  }
  else
    dJointSetBreakable( this->jointId, 0 );
}

//------------------------------------------------------------------------------
bool nOdeJoint::IsBreakable()
{
  return (0 != dJointIsBreakable( this->jointId ));
}

//------------------------------------------------------------------------------
void nOdeJoint::SetBreakMode( int mode )
{
  n_assert( this->IsBreakable() );
  
  dJointSetBreakMode( this->jointId, mode );
}

//------------------------------------------------------------------------------
void nOdeJoint::SetBreakModeFlag( int flag, bool enable )
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  int mode = dJointGetBreakMode( this->jointId );
  
  if ( enable )
    mode |= flag;
  else
    mode &= ~flag;
    
  dJointSetBreakMode( this->jointId, mode );
}

//------------------------------------------------------------------------------
bool nOdeJoint::GetBreakModeFlag( int flag )
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  int mode = dJointGetBreakMode( this->jointId );
  return (0 != (mode & flag));
}

//------------------------------------------------------------------------------
int nOdeJoint::GetBreakMode()
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  return dJointGetBreakMode( this->jointId );
}

//------------------------------------------------------------------------------
void nOdeJoint::JointBreakCallback( dJointID joint )
{
  nOdeJoint* self = ((nOdeJoint*)dJointGetData( joint ));
  self->JointBroken( self );
}

//------------------------------------------------------------------------------
void nOdeJoint::SetBreakCallback( nOdeJointBreakCallback* callback )
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  if ( callback )
    dJointSetBreakCallback( this->jointId, nOdeJoint::SetBreakCallback );
  else
    dJointSetBreakCallback( this->jointId, 0 );
  this->JointBroken = callback;
}

//------------------------------------------------------------------------------
nOdeJointBreakCallback* nOdeJoint::GetBreakCallback()
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  return this->JointBroken;
}

//------------------------------------------------------------------------------
void nOdeJoint::SetBreakForce( int body, const vector3& v )
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  dJointSetBreakForce( this->jointId, body, v.x, v.y, v.z );
}

//------------------------------------------------------------------------------
void nOdeJoint::GetBreakForce( int body, vector3* result )
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  dVector3 res;
  dJointGetBreakForce( this->jointId, body, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeJoint::SetBreakTorque( int body, const vector3& v )
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  dJointSetBreakTorque( this->jointId, body, v.x, v.y, v.z );
}

//------------------------------------------------------------------------------
void nOdeJoint::GetBreakTorque( int body, vector3* result )
{
  n_assert( this->IsBreakable() && "Joint must be breakable!" );
  
  dVector3 res;
  dJointGetBreakTorque( this->jointId, body, res );
  result->set( res[0], res[1], res[2] );
}

#endif // ODE_BREAKABLE_JOINTS

//------------------------------------------------------------------------------
nOdeBallJoint::nOdeBallJoint( dWorldID world, dJointGroupID group )
{
  this->jointId = dJointCreateBall( world, group );
  dJointSetData( this->jointId, this );
}

//------------------------------------------------------------------------------
nOdeBallJoint::~nOdeBallJoint()
{
}

//------------------------------------------------------------------------------
void nOdeBallJoint::SetAnchor( const vector3& anchor )
{
  dJointSetBallAnchor( this->jointId, anchor.x, anchor.y, anchor.z );
}

//------------------------------------------------------------------------------
void nOdeBallJoint::GetAnchor( vector3* result )
{
  dVector3 res;
  dJointGetBallAnchor( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeBallJoint::GetAnchor2( vector3* result )
{
  dVector3 res;
  dJointGetBallAnchor2( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
nOdeHingeJoint::nOdeHingeJoint( dWorldID world, dJointGroupID group )
{
  this->jointId = dJointCreateHinge( world, group );
  dJointSetData( this->jointId, this );
}

//------------------------------------------------------------------------------
nOdeHingeJoint::~nOdeHingeJoint()
{
}

//------------------------------------------------------------------------------
void nOdeHingeJoint::SetAnchor( const vector3& anchor )
{
  dJointSetHingeAnchor( this->jointId, anchor.x, anchor.y, anchor.z );
}

//------------------------------------------------------------------------------
void nOdeHingeJoint::SetAxis( const vector3& axis )
{
  dJointSetHingeAxis( this->jointId, axis.x, axis.y, axis.z );
}

//------------------------------------------------------------------------------
void nOdeHingeJoint::GetAnchor( vector3* result )
{
  dVector3 res;
  dJointGetHingeAnchor( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeHingeJoint::GetAnchor2( vector3* result )
{
  dVector3 res;
  dJointGetHingeAnchor2( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeHingeJoint::GetAxis( vector3* result )
{
  dVector3 res;
  dJointGetHingeAxis( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
float nOdeHingeJoint::GetAngle()
{
  return dJointGetHingeAngle( this->jointId );
}

//------------------------------------------------------------------------------
float nOdeHingeJoint::GetAngleRate()
{
  return dJointGetHingeAngleRate( this->jointId );
}

//------------------------------------------------------------------------------
void nOdeHingeJoint::SetParam( int param, float value )
{
  dJointSetHingeParam( this->jointId, param, value );
}

//------------------------------------------------------------------------------
float nOdeHingeJoint::GetParam( int param )
{
  return dJointGetHingeParam( this->jointId, param );
}

//------------------------------------------------------------------------------
nOdeSliderJoint::nOdeSliderJoint( dWorldID world, dJointGroupID group )
{
  this->jointId = dJointCreateSlider( world, group );
  dJointSetData( this->jointId, this );
}

//------------------------------------------------------------------------------
nOdeSliderJoint::~nOdeSliderJoint()
{
}

//------------------------------------------------------------------------------
void nOdeSliderJoint::SetAxis( const vector3& axis )
{
  dJointSetSliderAxis( this->jointId, axis.x, axis.y, axis.z );
}

//------------------------------------------------------------------------------
void nOdeSliderJoint::GetAxis( vector3* result )
{
  dVector3 res;
  dJointGetSliderAxis( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
float nOdeSliderJoint::GetPosition()
{
  return dJointGetSliderPosition( this->jointId );
}

//------------------------------------------------------------------------------
float nOdeSliderJoint::GetPositionRate()
{
  return dJointGetSliderPositionRate( this->jointId );
}

//------------------------------------------------------------------------------
void nOdeSliderJoint::SetParam( int param, float value )
{
  dJointSetSliderParam( this->jointId, param, value );
}

//------------------------------------------------------------------------------
float nOdeSliderJoint::GetParam( int param )
{
  return dJointGetSliderParam( this->jointId, param );
}

//------------------------------------------------------------------------------
nOdeUniversalJoint::nOdeUniversalJoint( dWorldID world, dJointGroupID group )
{
  this->jointId = dJointCreateUniversal( world, group );
  dJointSetData( this->jointId, this );
}

//------------------------------------------------------------------------------
nOdeUniversalJoint::~nOdeUniversalJoint()
{
}

//------------------------------------------------------------------------------
void nOdeUniversalJoint::SetAnchor( const vector3& anchor )
{
  dJointSetUniversalAnchor( this->jointId, anchor.x, anchor.y, anchor.z );
}

//------------------------------------------------------------------------------
void nOdeUniversalJoint::SetAxis1( const vector3& axis1 )
{
  dJointSetUniversalAxis1( this->jointId, axis1.x, axis1.y, axis1.z );
}

//------------------------------------------------------------------------------
void nOdeUniversalJoint::SetAxis2( const vector3& axis2 )
{
  dJointSetUniversalAxis2( this->jointId, axis2.x, axis2.y, axis2.z );
}

//------------------------------------------------------------------------------
void nOdeUniversalJoint::GetAnchor( vector3* result )
{
  dVector3 res;
  dJointGetUniversalAnchor( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeUniversalJoint::GetAnchor2( vector3* result )
{
  dVector3 res;
  dJointGetUniversalAnchor2( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeUniversalJoint::GetAxis1( vector3* result )
{
  dVector3 res;
  dJointGetUniversalAxis1( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeUniversalJoint::GetAxis2( vector3* result )
{
  dVector3 res;
  dJointGetUniversalAxis2( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeUniversalJoint::SetParam( int param, float value )
{
  dJointSetUniversalParam( this->jointId, param, value );
}

//------------------------------------------------------------------------------
float nOdeUniversalJoint::GetParam( int param )
{
  return dJointGetUniversalParam( this->jointId, param );
}

//------------------------------------------------------------------------------
nOdeHinge2Joint::nOdeHinge2Joint( dWorldID world, dJointGroupID group )
{
  this->jointId = dJointCreateHinge2( world, group );
  dJointSetData( this->jointId, this );
}

//------------------------------------------------------------------------------
nOdeHinge2Joint::~nOdeHinge2Joint()
{
}

//------------------------------------------------------------------------------
void nOdeHinge2Joint::SetAnchor( const vector3& anchor )
{
  dJointSetHinge2Anchor( this->jointId, anchor.x, anchor.y, anchor.z );
}

//------------------------------------------------------------------------------
void nOdeHinge2Joint::SetAxis1( const vector3& axis1 )
{
  dJointSetHinge2Axis1( this->jointId, axis1.x, axis1.y, axis1.z );
}

//------------------------------------------------------------------------------
void nOdeHinge2Joint::SetAxis2( const vector3& axis2 )
{
  dJointSetHinge2Axis2( this->jointId, axis2.x, axis2.y, axis2.z );
}

//------------------------------------------------------------------------------
void nOdeHinge2Joint::GetAnchor( vector3* result )
{
  dVector3 res;
  dJointGetHinge2Anchor( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeHinge2Joint::GetAnchor2( vector3* result )
{
  dVector3 res;
  dJointGetHinge2Anchor2( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeHinge2Joint::GetAxis1( vector3* result )
{
  dVector3 res;
  dJointGetHinge2Axis1( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
void nOdeHinge2Joint::GetAxis2( vector3* result )
{
  dVector3 res;
  dJointGetHinge2Axis2( this->jointId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
float nOdeHinge2Joint::GetAngle1()
{
  return dJointGetHinge2Angle1( this->jointId );
}

//------------------------------------------------------------------------------
float nOdeHinge2Joint::GetAngle1Rate()
{
  return dJointGetHinge2Angle1Rate( this->jointId );
}

//------------------------------------------------------------------------------
float nOdeHinge2Joint::GetAngle2Rate()
{
  return dJointGetHinge2Angle2Rate( this->jointId );
}

//------------------------------------------------------------------------------
void nOdeHinge2Joint::SetParam( int param, float value )
{
  dJointSetHinge2Param( this->jointId, param, value );
}

//------------------------------------------------------------------------------
float nOdeHinge2Joint::GetParam( int param )
{
  return dJointGetHinge2Param( this->jointId, param );
}

//------------------------------------------------------------------------------
nOdeFixedJoint::nOdeFixedJoint( dWorldID world, dJointGroupID group )
{
  this->jointId = dJointCreateFixed( world, group );
  dJointSetData( this->jointId, this );
}

//------------------------------------------------------------------------------
nOdeFixedJoint::~nOdeFixedJoint()
{
}

//------------------------------------------------------------------------------
void nOdeFixedJoint::Fix() 
{ 
  dJointSetFixed( this->jointId ); 
}

//------------------------------------------------------------------------------
nOdeAMotorJoint::nOdeAMotorJoint( dWorldID world, dJointGroupID group )
{
  this->jointId = dJointCreateAMotor( world, group );
  dJointSetData( this->jointId, this );
}

//------------------------------------------------------------------------------
nOdeAMotorJoint::~nOdeAMotorJoint()
{
}

//------------------------------------------------------------------------------
void nOdeAMotorJoint::SetMode( int mode )
{
  dJointSetAMotorMode( this->jointId, mode );
}

//------------------------------------------------------------------------------
int nOdeAMotorJoint::GetMode()
{
  return dJointGetAMotorMode( this->jointId );
}

//------------------------------------------------------------------------------
void nOdeAMotorJoint::SetNumAxes( int num )
{
  dJointSetAMotorNumAxes( this->jointId, num );
}

//------------------------------------------------------------------------------
int nOdeAMotorJoint::GetNumAxes()
{
  return dJointGetAMotorNumAxes( this->jointId );
}

//------------------------------------------------------------------------------
void nOdeAMotorJoint::SetAxis( int anum, int rel, const vector3& axis )
{
  dJointSetAMotorAxis( this->jointId, anum, rel, axis.x, axis.y, axis.z );
}

//------------------------------------------------------------------------------
void nOdeAMotorJoint::GetAxis( int anum, vector3* result )
{
  dVector3 res;
  dJointGetAMotorAxis( this->jointId, anum, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
int nOdeAMotorJoint::GetAxisRel( int anum )
{
  return dJointGetAMotorAxisRel( this->jointId, anum );
}

//------------------------------------------------------------------------------
void nOdeAMotorJoint::SetAngle( int anum, float angle )
{
  dJointSetAMotorAngle( this->jointId, anum, angle );
}

//------------------------------------------------------------------------------
float nOdeAMotorJoint::GetAngle( int anum )
{
  return dJointGetAMotorAngle( this->jointId, anum );
}

//------------------------------------------------------------------------------
float nOdeAMotorJoint::GetAngleRate( int anum )
{
  return dJointGetAMotorAngleRate( this->jointId, anum );
}

//------------------------------------------------------------------------------
void nOdeAMotorJoint::SetParam( int param, float value )
{
  dJointSetAMotorParam( this->jointId, param, value );
}

//------------------------------------------------------------------------------
float nOdeAMotorJoint::GetParam( int param )
{
  return dJointGetAMotorParam( this->jointId, param );
}

