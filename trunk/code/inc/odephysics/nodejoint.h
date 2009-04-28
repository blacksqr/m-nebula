#ifndef N_ODEJOINT_H
#define N_ODEJOINT_H
//------------------------------------------------------------------------------
/**
    @class nOdeJoint
    @ingroup NODEPhysicsContribModule
    @brief Corresponds to an ODE joint.

    To enable breakable joints define ODE_BREAKABLE_JOINTS,
    but you must make the appropriate changes to ODE first.

    (C) 2003  Vadim Macagon
    
    nOdeJoint is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeHingeJoint
    @ingroup NODEPhysicsContribModule
    @brief Corresponds to an ODE hinge joint.

    (C) 2003  Vadim Macagon
    
    nOdeHingeJoint is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeSliderJoint
    @ingroup NODEPhysicsContribModule
    @brief Corresponds to an ODE slider joint.

    (C) 2003  Vadim Macagon
    
    nOdeSliderJoint is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeHinge2Joint
    @ingroup NODEPhysicsContribModule
    @brief Corresponds to an ODE hinge2 joint.

    (C) 2003  Vadim Macagon
    
    nOdeHinge2Joint is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeFixedJoint
    @ingroup NODEPhysicsContribModule
    @brief Corresponds to an ODE fixed joint.

    (C) 2003  Vadim Macagon
    
    nOdeFixedJoint is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeBallJoint
    @ingroup NODEPhysicsContribModule
    @brief Corresponds to an ODE ball joint.

    (C) 2003  Vadim Macagon
    
    nOdeBallJoint is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeAMotorJoint
    @ingroup NODEPhysicsContribModule
    @brief Corresponds to an ODE AMotor joint.

    (C) 2003  Vadim Macagon
    
    nOdeAMotorJoint is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeUniversalJoint
    @ingroup NODEPhysicsContribModule
    @brief Corresponds to an ODE universal joint.

    (C) 2003  Vadim Macagon
    
    nOdeUniversalJoint is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

//#ifndef N_ARRAY_H
//#include "util/narray.h"
//#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeJoint
#include "kernel/ndefdllclass.h"

class nOdeBody;

//------------------------------------------------------------------------------
#ifdef ODE_BREAKABLE_JOINTS
typedef void nOdeJointBreakCallback( nOdeJoint* );
#endif // ODE_BREAKABLE_JOINTS

class N_PUBLIC nOdeJoint
{
  public:
      
    virtual ~nOdeJoint();
    
    void* GetData() const;
    void SetData( void* data );
    
    virtual void AttachTo( nOdeBody* b1, nOdeBody* b2 );
    int GetType();
    nOdeBody* GetBody( int index );
    void EnableFeedback( bool flag );
    bool IsFeedbackEnabled();
    void GetFeedback( vector3* force1, vector3* torque1,
                      vector3* force2, vector3* torque2 );
    /// set a joint parameter
    virtual void SetParam( int param, float value );
    /// get a joint parameter
    virtual float GetParam( int param );
    
#ifdef ODE_BREAKABLE_JOINTS
    void SetBreakable( bool breakable );
    bool IsBreakable();
    void SetBreakMode( int mode );
    int GetBreakMode();
    void SetBreakModeFlag( int flag, bool enable );
    bool GetBreakModeFlag( int flag );
    void SetBreakCallback( nOdeJointBreakCallback* );
    nOdeJointBreakCallback* GetBreakCallback();
    void SetBreakForce( int body, const vector3& );
    void GetBreakForce( int body, vector3* );
    void SetBreakTorque( int body, const vector3& );
    void GetBreakTorque( int body, vector3* );
#endif // ODE_BREAKABLE_JOINTS
    
  protected:
    nOdeJoint();
    
    dJointID jointId;

  private:
    void* userdata;
    
#ifdef ODE_BREAKABLE_JOINTS   
    static void JointBreakCallback( dJointID );
    nOdeJointBreakCallback* JointBroken;
#endif // ODE_BREAKABLE_JOINTS
};

//------------------------------------------------------------------------------
class N_PUBLIC nOdeBallJoint : public nOdeJoint
{
  public:
    friend class nOdePhysicsContext;
    
    virtual ~nOdeBallJoint();
    
    void SetAnchor( const vector3& anchor );
    void GetAnchor( vector3* result );
    void GetAnchor2( vector3* result );
    
  protected:
    nOdeBallJoint( dWorldID world, dJointGroupID group );
    
  private:
    nOdeBallJoint() {};
};

//------------------------------------------------------------------------------
class N_PUBLIC nOdeHingeJoint : public nOdeJoint
{
  public:
    friend class nOdePhysicsContext;
  
    virtual ~nOdeHingeJoint();
    
    void SetAnchor( const vector3& anchor );
    void SetAxis( const vector3& axis );
    void GetAnchor( vector3* result );
    void GetAnchor2( vector3* result );
    void GetAxis( vector3* result);
    float GetAngle();
    float GetAngleRate();
    
    virtual void SetParam( int param, float value );
    virtual float GetParam( int param );
  
  protected:
    nOdeHingeJoint( dWorldID world, dJointGroupID group );
    
  private:
    nOdeHingeJoint() {};
};

//------------------------------------------------------------------------------
class N_PUBLIC nOdeSliderJoint : public nOdeJoint
{
  public:
    friend class nOdePhysicsContext;
    
    virtual ~nOdeSliderJoint();
    
    void SetAxis( const vector3& axis );
    void GetAxis( vector3* result );
    float GetPosition();
    float GetPositionRate();
    
    virtual void SetParam( int param, float value );
    virtual float GetParam( int param );
  
  protected:
    nOdeSliderJoint( dWorldID world, dJointGroupID group );
    
  private:
    nOdeSliderJoint() {};
};

//------------------------------------------------------------------------------
class N_PUBLIC nOdeUniversalJoint : public nOdeJoint
{
  public:
    friend class nOdePhysicsContext;
  
    virtual ~nOdeUniversalJoint();
    
    void SetAnchor( const vector3& anchor );
    void SetAxis1( const vector3& axis1 );
    void SetAxis2( const vector3& axis2 );
    void GetAnchor( vector3* result );
    void GetAnchor2( vector3* result );
    void GetAxis1( vector3* result );
    void GetAxis2( vector3* result );
  
    virtual void SetParam( int param, float value );
    virtual float GetParam( int param );
  
  protected:
    nOdeUniversalJoint( dWorldID world, dJointGroupID group );
    
  private:
    nOdeUniversalJoint() {};
};

//------------------------------------------------------------------------------
class N_PUBLIC nOdeHinge2Joint : public nOdeJoint
{
  public:
    friend class nOdePhysicsContext;
    
    virtual ~nOdeHinge2Joint();
    
    void SetAnchor( const vector3& anchor );
    void SetAxis1( const vector3& axis1 );
    void SetAxis2( const vector3& axis2 );
    void GetAnchor( vector3* result );
    void GetAnchor2( vector3* result );
    void GetAxis1( vector3* result );
    void GetAxis2( vector3* result );
    float GetAngle1();
    float GetAngle1Rate();
    float GetAngle2Rate();
    
    virtual void SetParam( int param, float value );
    virtual float GetParam( int param );
    
  protected:
    nOdeHinge2Joint( dWorldID world, dJointGroupID group );
    
  private:
    nOdeHinge2Joint() {};
};

//------------------------------------------------------------------------------
class N_PUBLIC nOdeFixedJoint : public nOdeJoint
{
  public:
    friend class nOdePhysicsContext;
  
    virtual ~nOdeFixedJoint();
    
    void Fix();
  
  protected:
    nOdeFixedJoint( dWorldID world, dJointGroupID group );
    
  private:
    nOdeFixedJoint() {};
};

//------------------------------------------------------------------------------
class N_PUBLIC nOdeAMotorJoint : public nOdeJoint
{
  public:
    friend class nOdePhysicsContext;
  
    virtual ~nOdeAMotorJoint();
    
    void SetMode( int mode );
    int GetMode();
    void SetNumAxes( int num );
    int GetNumAxes();
    void SetAxis( int anum, int rel, const vector3& axis );
    void GetAxis( int anum, vector3* result );
    int GetAxisRel( int anum );
    void SetAngle( int anum, float angle );
    float GetAngle( int anum );
    float GetAngleRate( int anum );
    
    virtual void SetParam( int param, float value );
    virtual float GetParam( int param );
  
  protected:
    nOdeAMotorJoint( dWorldID world, dJointGroupID group );
    
  private:
    nOdeAMotorJoint() {};
};

//------------------------------------------------------------------------------
/*
class nOdeJointGroup
{
  public:
    nOdeJointGroup( int initialSize = 5, int growSize = 0 );
    ~nOdeJointGroup();
    void Empty();
    
  private:
    dJointGroupID groupId;
    nArray<nOdeJoint*> joints;
};
*/
//------------------------------------------------------------------------------
/**
  @brief Constructor
  @param initialSize  Initial number of joint slots in group (ie. capacity).
  @param growSize     Number of new joint slots to grow group by when
                      space runs out.
  
  If growSize is set to zero (default value) the group capacity will be
  fixed.
*/
//inline
//nOdeJointGroup::nOdeJointGroup( int initialSize, int growSize )
//  : joints( initialSize, growSize )
//{ 
//  this->groupId = dJointGroupCreate( 0 );
//}

//------------------------------------------------------------------------------
//inline
//nOdeJointGroup::~nOdeJointGroup()
//{
//  dJointGroupDestroy( this->groupId );
//}

//------------------------------------------------------------------------------
//inline
//void nOdeJointGroup::Empty()
//{
//  delete this->joints[
//  dJointGroupEmpty( this->groupId );
//}

//------------------------------------------------------------------------------
#endif
