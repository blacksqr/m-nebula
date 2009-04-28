#ifndef N_ODEPHYSICSCONTEXT_H
#define N_ODEPHYSICSCONTEXT_H
//------------------------------------------------------------------------------
/**
    @class nOdePhysicsContext
    @ingroup NODEPhysicsContribModule
    @brief A physics context corresponds to an ODE World.

    Typically you'll probably only need one of these.

    Various global world parameters can be altered to tweak how things are
    simulated within the context. The context also acts as a factory for
    rigid bodies and joints.
    
    You can't create ODE contact joints explicitly, instead use CreateContact,
    the contact joints are destroyed after each simulation Step().

    (C) 2003  Vadim Macagon
    
    nOdePhysicsContext is licensed under the terms of the Nebula License.
*/
#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_ODESURFACE_H
#include "odephysics/nodesurface.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdePhysicsContext
#include "kernel/ndefdllclass.h"

class nOdeBody;
class nOdeCollideContact;
class nOdeJoint;
class nOdeBallJoint;
class nOdeHingeJoint;
class nOdeHinge2Joint;
class nOdeSliderJoint;
class nOdeUniversalJoint;
class nOdeFixedJoint;
class nOdeAMotorJoint;

//------------------------------------------------------------------------------
class N_PUBLIC nOdePhysicsContext : public nRoot
{
  public:
    /// constructor
    nOdePhysicsContext();
    /// destructor
    virtual ~nOdePhysicsContext();
    /// persistency
    virtual bool SaveCmds( nPersistServer* persistServer );

    /// set gravity
    void SetGravity( const vector3& gravity );
    void GetGravity( vector3* gravity );
    /// set global error reduction parameter value
    void SetERP( float value );
    float GetERP();
    /// set global constraint force mixing value
    void SetCFM( float value );
    float GetCFM();

    // set and get the AutoEnableDepth parameter used by the StepFast1 algorithm
    void SetAutoEnableDepth( int depth );
    int GetAutoEnableDepth();
     
    /// take a simulation step of 'stepSize' seconds
    void Step( float stepSize );
    
    // step the world by stepsize seconds using the StepFast1 algorithm
    void StepFast( float stepSize, int maxIterations );

    /// convert an impulse to a force
    void ImpulseToForce( float stepSize, const vector3& impulse, vector3* force );
    
    dWorldID GetWorldId();
    
    /// create rigid body
    nOdeBody* NewOdeBody();
    /// destroy rigid body
    void ReleaseOdeBody( nOdeBody* );

    // add a contact joint to the context
    void CreateContact( nOdeBody* b1, nOdeBody* b2,
                        const nOdeCollideContact& contact, 
                        const nOdeSurface& surface,
                        const vector3& friction = vector3( 0.0f, 0.0f, 0.0f ) );
                        
    // destroy all contact joints
    void ClearContacts();

    nOdeBallJoint* NewBallJoint( /*nOdeJointGroup* group = NULL*/ );
    nOdeHingeJoint* NewHingeJoint( /*nOdeJointGroup* group = NULL*/ );
    nOdeSliderJoint* NewSliderJoint( /*nOdeJointGroup* group = NULL*/ );
    nOdeUniversalJoint* NewUniversalJoint( /*nOdeJointGroup* group = NULL*/ );
    nOdeHinge2Joint* NewHinge2Joint( /*nOdeJointGroup* group = NULL*/ );
    nOdeFixedJoint* NewFixedJoint( /*nOdeJointGroup* group = NULL*/ );
    nOdeAMotorJoint* NewAMotorJoint( /*nOdeJointGroup* group = NULL*/ );
    void ReleaseOdeJoint( nOdeJoint* joint );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
    
  protected:
    dWorldID worldId;
    dJointGroupID contactJointGroup;
};

//------------------------------------------------------------------------------
inline
dWorldID nOdePhysicsContext::GetWorldId()
{
  return this->worldId;
}

//------------------------------------------------------------------------------
inline
void nOdePhysicsContext::SetGravity( const vector3& gravity )
{
  dWorldSetGravity( this->worldId, gravity.x, gravity.y, gravity.z );
}

//------------------------------------------------------------------------------
inline
void nOdePhysicsContext::GetGravity( vector3* gravity )
{
  dVector3 grav;
  dWorldGetGravity( this->worldId, grav );
  gravity->set( grav[0], grav[1], grav[2] );
}

//------------------------------------------------------------------------------
inline
void nOdePhysicsContext::SetERP( float value )
{
  dWorldSetERP( this->worldId, value );
}

//------------------------------------------------------------------------------
inline
float nOdePhysicsContext::GetERP()
{
  return dWorldGetERP( this->worldId );
}

//------------------------------------------------------------------------------
inline
void nOdePhysicsContext::SetCFM( float value )
{
  dWorldSetCFM( this->worldId, value );
}

//------------------------------------------------------------------------------
inline
float nOdePhysicsContext::GetCFM()
{
  return dWorldGetCFM( this->worldId );
}

//------------------------------------------------------------------------------
inline
void nOdePhysicsContext::Step( float stepSize )
{
  dWorldStep( this->worldId, stepSize );
  this->ClearContacts();
}

//------------------------------------------------------------------------------
inline
void nOdePhysicsContext::StepFast( float stepSize, int maxIterations )
{
  dWorldStepFast1( this->worldId, stepSize, maxIterations );
  this->ClearContacts();
}

//------------------------------------------------------------------------------
inline
void nOdePhysicsContext::ImpulseToForce( float stepSize, const vector3& impulse, 
                                         vector3* force )
{
  dVector3 res;
  dWorldImpulseToForce( this->worldId, stepSize, impulse.x, impulse.y, impulse.z,
                        res );
  force->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
inline                        
void nOdePhysicsContext::ClearContacts()
{
  dJointGroupEmpty( this->contactJointGroup );
}

//------------------------------------------------------------------------------
#endif // N_ODEPHYSICSCONTEXT_H

