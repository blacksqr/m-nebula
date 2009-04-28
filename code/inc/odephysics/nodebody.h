#ifndef N_ODEBODY_H
#define N_ODEBODY_H
//------------------------------------------------------------------------------
/**
  @class nOdeBody
  @ingroup NODEPhysicsContribModule
  @brief Defines an ODE rigid body.

  To enable auto-disabling functionallity define ODE_STEPFAST_AUTO_DISABLE,
  but you must make the appropriate changes to ODE first.

  (c) 2003 Vadim Macagon
  
  nOdeBody is licensed under the terms of the Nebula License.
*/  
//------------------------------------------------------------------------------
#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#ifndef N_ODEUTIL_H
#include "odephysics/odeutil.h"
#endif

#ifndef N_ODEMASS_H
#include "odephysics/nodemass.h"
#endif

#ifndef N_ODEPHYSICSCONTEXT_H
#include "odephysics/nodephysicscontext.h"
#endif

//------------------------------------------------------------------------------
class nOdeBody
{
  public:
    friend class nOdePhysicsContext;
    friend class nOdeCollideShape;
    friend class nOdeJoint;
  
    nOdeBody( nOdePhysicsContext* context );
    ~nOdeBody();
    
    void* GetData() const;
    void SetData( void* data );
    
    void SetMass( const nOdeMass& mass );
    void GetMass( nOdeMass* mass );
    
    /// set position & orientation
    void SetTransform( const matrix44& mat );
    /// get the transform for the body
    void GetTransform( matrix44* result );
    /// rotate the body
    void SetRotation( const matrix33& rot );
    void SetRotation( const matrix44& rot );
    void SetRotation( const quaternion& rot );
    /// get rotation of body
    void GetRotation( matrix33* rot );
    void GetRotation( quaternion* rot );
    /// position the body
    void SetPosition( const vector3& pos );
    /// get position of body
    void GetPosition( vector3* pos );
    
    /// set linear velocity of body
    void SetLinearVel( const vector3& vel );
    void GetLinearVel( vector3* vel );
    /// set angular velocity of body
    void SetAngularVel( const vector3& vel );
    void GetAngularVel( vector3* vel );
    
    void AddForce( const vector3& force );
    void AddTorque( const vector3& torque );
    void AddRelForce( const vector3& force );
    void AddRelTorque( const vector3& torque );
    void AddForceAtPos( const vector3& force, const vector3& point );
    void AddForceAtRelPos( const vector3& force, const vector3& point );
    void AddRelForceAtPos( const vector3& force, const vector3& point );
    void AddRelForceAtRelPos( const vector3& force, const vector3& point );
    
    /// get accumulated force vector
    void GetForce( vector3* force );
    /// get accumulated torque vector
    void GetTorque( vector3* torque );
    
    /// set the accumulated force vector
    void SetForce( const vector3& force );
    /// set the accumulated torque vector
    void SetTorque( const vector3& torque );
    
    // utility
    
    void GetRelPointPos( const vector3& point, vector3* result );
    void GetRelPointVel( const vector3& point, vector3* result );
    void GetPointVel( const vector3& point, vector3* result );
    void GetPosRelPoint( const vector3& point, vector3* result );
    void VectorToWorld( const vector3& vec, vector3* result );
    void VectorFromWorld( const vector3& vec, vector3* result );
    
    void Enable();
    void Disable();
    bool IsEnabled();
    
# ifdef ODE_STEPFAST_AUTO_DISABLE
    void SetAutoDisableThreshold( float threshold );
    float GetAutoDisableThreshold();
    void SetAutoDisableSteps( int steps );
    int GetAutoDisableSteps();
    void SetAutoDisable( bool doAutoDisable );
    bool GetAutoDisable();
# endif // ODE_STEPFAST_AUTO_DISABLE

    void SetFiniteRotationMode( int mode );
    int GetFiniteRotationMode();
    void SetFiniteRotationAxis( const vector3& axis );
    void GetFiniteRotationAxis( vector3* result );
    
    int GetNumJoints();
    void EnableGravity( bool mode );
    bool IsGravityEnabled();
      
    /// return true if two bodies are connected together by a joint
    bool ConnectedTo( nOdeBody* other );
    /// return true if two bodies are connected together by a joint 
    /// that does not have type jointType
    bool ConnectedToEx( nOdeBody* other, int jointType );
      
  private:
    nOdeBody(); // prevent default construction
    
    dBodyID bodyId;
    void* userdata;
};

//------------------------------------------------------------------------------
inline
nOdeBody::nOdeBody()
{
  // private
}

//------------------------------------------------------------------------------
inline
nOdeBody::nOdeBody( nOdePhysicsContext* context )
  : userdata(0)
{
  this->bodyId = dBodyCreate( context->GetWorldId() );
  dBodySetData( this->bodyId, (void*)this );
}

//------------------------------------------------------------------------------
inline
nOdeBody::~nOdeBody()
{
  dBodyDestroy( this->bodyId );
}

//------------------------------------------------------------------------------
inline
void* nOdeBody::GetData() const
{
  return this->userdata;
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetData( void* data )
{
  this->userdata = data;
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetMass( const nOdeMass& mass )
{
  dBodySetMass( this->bodyId, &mass.mass );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetMass( nOdeMass* mass )
{
  dMass m;
  dBodyGetMass( this->bodyId, &m );
  mass->mass = m;
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetTransform( const matrix44& mat )
{
  dMatrix3 rotation;
  nOdeUtil::NebMatrixToODE( mat, rotation );
  dBodySetPosition( this->bodyId, mat.M41, mat.M42, mat.M43 );
  dBodySetRotation( this->bodyId, rotation );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetTransform( matrix44* result )
{
  const dVector3& position = *(const dVector3*)dBodyGetPosition( this->bodyId );
  const dMatrix3& rotation = *(const dMatrix3*)dBodyGetRotation( this->bodyId );
  nOdeUtil::OdeMatrixToNeb( position, rotation, *result );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::SetRotation( const matrix33& rot )
{
  dMatrix3 rotation;
  nOdeUtil::NebMatrixToODE( rot, rotation );
  dBodySetRotation( this->bodyId, rotation );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetRotation( const matrix44& rot )
{
  dMatrix3 rotation;
  nOdeUtil::NebMatrixToODE( rot, rotation );
  dBodySetRotation( this->bodyId, rotation );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetRotation( const quaternion& rot )
{
  dQuaternion quat;
  nOdeUtil::NebQuatToOde( rot, quat );
  dBodySetQuaternion( this->bodyId, quat );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetRotation( matrix33* rot )
{
  const dMatrix3& rotation = *(const dMatrix3*)dBodyGetRotation( this->bodyId );
  nOdeUtil::OdeMatrixToNeb( rotation, *rot );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetRotation( quaternion* rot )
{
  const dQuaternion& quat = *(const dQuaternion*)dBodyGetQuaternion( this->bodyId );
  nOdeUtil::OdeQuatToNeb( quat, rot );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetPosition( const vector3& pos )
{
  dBodySetPosition( this->bodyId, pos.x, pos.y, pos.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetPosition( vector3* pos )
{
  const dReal* res = dBodyGetPosition( this->bodyId );
  pos->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetLinearVel( const vector3& vel )
{
  dBodySetLinearVel( this->bodyId, vel.x, vel.y, vel.z );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::GetLinearVel( vector3* vel )
{
  const dReal* res = dBodyGetLinearVel( this->bodyId );
  vel->set( res[0], res[1], res[2] );
}
    
//------------------------------------------------------------------------------
inline
void nOdeBody::SetAngularVel( const vector3& vel )
{
  dBodySetAngularVel( this->bodyId, vel.x, vel.y, vel.z );
}

//------------------------------------------------------------------------------
inline 
void nOdeBody::GetAngularVel( vector3* vel )
{
  const dReal* res = dBodyGetAngularVel( this->bodyId );
  vel->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::AddForce( const vector3& force )
{
  dBodyAddForce( this->bodyId, force.x, force.y, force.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::AddTorque( const vector3& torque )
{
  dBodyAddTorque( this->bodyId, torque.x, torque.y, torque.z );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::AddRelForce( const vector3& force )
{
  dBodyAddRelForce( this->bodyId, force.x, force.y, force.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::AddRelTorque( const vector3& torque )
{
  dBodyAddRelTorque( this->bodyId, torque.x, torque.y, torque.z );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::AddForceAtPos( const vector3& force, const vector3& point )
{
  dBodyAddForceAtPos( this->bodyId, force.x, force.y, force.z,
                      point.x, point.y, point.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::AddForceAtRelPos( const vector3& force, const vector3& point )
{
  dBodyAddForceAtRelPos( this->bodyId, force.x, force.y, force.z,
                         point.x, point.y, point.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::AddRelForceAtPos( const vector3& force, const vector3& point )
{
  dBodyAddRelForceAtPos( this->bodyId, force.x, force.y, force.z,
                         point.x, point.y, point.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::AddRelForceAtRelPos( const vector3& force, const vector3& point )
{
  dBodyAddRelForceAtRelPos( this->bodyId, force.x, force.y, force.z,
                            point.x, point.y, point.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetForce( vector3* force )
{
  const dReal* res = dBodyGetForce( this->bodyId );
  force->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetTorque( vector3* torque )
{
  const dReal* res = dBodyGetTorque( this->bodyId );
  torque->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetForce( const vector3& force )
{
  dBodySetForce( this->bodyId, force.x, force.y, force.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetTorque( const vector3& torque )
{
  dBodySetTorque( this->bodyId, torque.x, torque.y, torque.z );
}
    
//------------------------------------------------------------------------------
inline
void nOdeBody::GetRelPointPos( const vector3& point, vector3* result )
{
  dVector3 res;
  dBodyGetRelPointPos( this->bodyId, point.x, point.y, point.z, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetRelPointVel( const vector3& point, vector3* result )
{
  dVector3 res;
  dBodyGetRelPointVel( this->bodyId, point.x, point.y, point.z, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::GetPointVel( const vector3& point, vector3* result )
{
  dVector3 res;
  dBodyGetPointVel( this->bodyId, point.x, point.y, point.z, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::GetPosRelPoint( const vector3& point, vector3* result )
{
  dVector3 res;
  dBodyGetPosRelPoint( this->bodyId, point.x, point.y, point.z, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::VectorToWorld( const vector3& vec, vector3* result )
{
  dVector3 res;
  dBodyVectorToWorld( this->bodyId, vec.x, vec.y, vec.z, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::VectorFromWorld( const vector3& vec, vector3* result )
{
  dVector3 res;
  dBodyVectorFromWorld( this->bodyId, vec.x, vec.y, vec.z, res );
  result->set( res[0], res[1], res[2] );
}
    
//------------------------------------------------------------------------------
inline
void nOdeBody::Enable()
{
  dBodyEnable( this->bodyId );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::Disable()
{
  dBodyDisable( this->bodyId );
}

//------------------------------------------------------------------------------
inline
bool nOdeBody::IsEnabled()
{
  return (1 == dBodyIsEnabled( this->bodyId ));
}

#ifdef ODE_STEPFAST_AUTO_DISABLE

//------------------------------------------------------------------------------
inline
void nOdeBody::SetAutoDisableThreshold( float threshold )
{
  dBodySetAutoDisableThresholdSF1( this->bodyId, threshold );
}

//------------------------------------------------------------------------------
inline
float nOdeBody::GetAutoDisableThreshold()
{
  return dBodyGetAutoDisableThresholdSF1( this->bodyId );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetAutoDisableSteps( int steps )
{
  dBodySetAutoDisableStepsSF1( this->bodyId, steps );
}

//------------------------------------------------------------------------------
inline
int nOdeBody::GetAutoDisableSteps()
{
  return dBodyGetAutoDisableStepsSF1( this->bodyId );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::SetAutoDisable( bool doAutoDisable )
{
  if ( doAutoDisable )
    dBodySetAutoDisableSF1( this->bodyId, 1 );
  else
    dBodySetAutoDisableSF1( this->bodyId, 0 );
}

//------------------------------------------------------------------------------
inline
bool nOdeBody::GetAutoDisable()
{
  return (dBodyGetAutoDisableSF1( this->bodyId ) != 0);
}

#endif // ODE_STEPFAST_AUTO_DISABLE

//------------------------------------------------------------------------------
inline
void nOdeBody::SetFiniteRotationMode( int mode )
{
  dBodySetFiniteRotationMode( this->bodyId, mode );
}

//------------------------------------------------------------------------------
inline
int nOdeBody::GetFiniteRotationMode()
{
  return dBodyGetFiniteRotationMode( this->bodyId );
}

//------------------------------------------------------------------------------
inline    
void nOdeBody::SetFiniteRotationAxis( const vector3& axis )
{
  dBodySetFiniteRotationAxis( this->bodyId, axis.x, axis.y, axis.z );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::GetFiniteRotationAxis( vector3* result )
{
  dVector3 res;
  dBodyGetFiniteRotationAxis( this->bodyId, res );
  result->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline    
int nOdeBody::GetNumJoints()
{
  return dBodyGetNumJoints( this->bodyId );
}

//------------------------------------------------------------------------------
inline
void nOdeBody::EnableGravity( bool mode )
{
  if ( true == mode )
    dBodySetGravityMode( this->bodyId, 1 );
  else
    dBodySetGravityMode( this->bodyId, 0 );
}

//------------------------------------------------------------------------------
inline
bool nOdeBody::IsGravityEnabled()
{
  return (0 != dBodyGetGravityMode( this->bodyId ));
}

//------------------------------------------------------------------------------
inline
bool nOdeBody::ConnectedTo( nOdeBody* other )
{
  return (1 == dAreConnected( this->bodyId, other->bodyId ));
}

//------------------------------------------------------------------------------
inline
bool nOdeBody::ConnectedToEx( nOdeBody* other, int jointType )
{
  return (1 == dAreConnectedExcluding( this->bodyId, other->bodyId, jointType ) );
}

//------------------------------------------------------------------------------
#endif  // N_ODEBODY_H
