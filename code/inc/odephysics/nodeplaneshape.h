#ifndef N_ODEPLANESHAPE_H
#define N_ODEPLANESHAPE_H
//------------------------------------------------------------------------------
/**
  @class nOdePlaneShape
  @ingroup NODEPhysicsContribModule
  @brief Describes a plane shape for the ODE collision system.

  The plane is considered to be a non-placeable shape, this means it is
  not attached to any movable body and is assumed to be part of the static
  environment. You can't rotate/position the plane using the usual
  shape rotation/position methods, instead you must do so via setting
  the four plane components.
  
  The plane equation is given by ax + by + cz = d, the normal vector (a, b, c)
  must be unit length.

  (c) 2003 Vadim Macagon
  
  nOdePlaneShape is licensed under the terms of the Nebula License.
*/  
//------------------------------------------------------------------------------

#ifndef N_ODECOLLIDESHAPE_H
#include "odephysics/nodecollideshape.h"
#endif

//------------------------------------------------------------------------------
class N_PUBLIC nOdePlaneShape : public nOdeCollideShape
{
  public:
    nOdePlaneShape();
    virtual ~nOdePlaneShape();
    
    /// set plane parameters
    void SetParams( const vector3& normal, float d );
    /// get plane parameters
    void GetParams( vector3* normal, float* d );
    
    /// transform shape (scaling not supported)
    virtual void SetTransform( const matrix44& mat );
    /// get the transform for the shape
    virtual void GetTransform( matrix44* result );
    /// rotate the shape
    virtual void SetRotation( const matrix33& rot );
    virtual void SetRotation( const matrix44& rot );
    virtual void SetRotation( const quaternion& rot );
    /// get rotation of shape
    virtual void GetRotation( matrix33* rot );
    virtual void GetRotation( quaternion* rot );
    /// position the shape
    virtual void SetPosition( const vector3& pos );
    /// get position of shape
    virtual void GetPosition( vector3* pos );
};

//------------------------------------------------------------------------------
#endif // N_ODEPLANESHAPE_H
