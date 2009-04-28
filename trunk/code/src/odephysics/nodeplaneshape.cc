#define N_IMPLEMENTS nOdePlaneShape
//------------------------------------------------------------------------------
//  nodeplaneshape.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdePlaneShape is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeplaneshape.h"

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

//------------------------------------------------------------------------------
/**
*/
nOdePlaneShape::nOdePlaneShape()
{
  this->geomId = dCreatePlane( 0, 0, 0, 0, 0);
  dGeomSetData( this->geomId, (void*)this );
  this->shapeType = nOdeCollideShape::OST_PLANE;
}

//------------------------------------------------------------------------------
/**
*/
nOdePlaneShape::~nOdePlaneShape()
{
  dGeomDestroy( this->geomId );
}

//------------------------------------------------------------------------------
/**
  @param normal Unit length vector specifying the plane normal.
  @param d The result of the dot product between the plane normal and a point 
           on the plane.
*/
void nOdePlaneShape::SetParams( const vector3& normal, float d )
{
  dGeomPlaneSetParams( this->geomId, normal.x, normal.y, normal.z, d );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::GetParams( vector3* normal, float* d )
{
  dVector4 res;
  dGeomPlaneGetParams( this->geomId, res );
  normal->set( res[0], res[1], res[2] );
  *d = res[3];
}

//------------------------------------------------------------------------------
void nOdePlaneShape::SetTransform( const matrix44& mat )
{
  n_error( "nOdePlaneShape::SetTransform() - operation not supported!" );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::GetTransform( matrix44* result )
{
  n_error( "nOdePlaneShape::GetTransform() - operation not supported!" );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::SetRotation( const matrix33& rot )
{
  n_error( "nOdePlaneShape::SetRotation() - operation not supported!" );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::SetRotation( const matrix44& rot )
{
  n_error( "nOdePlaneShape::GetRotation() - operation not supported!" );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::SetRotation( const quaternion& rot )
{
  n_error( "nOdePlaneShape::SetRotation() - operation not supported!" );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::GetRotation( matrix33* rot )
{
  n_error( "nOdePlaneShape::SetRotation() - operation not supported!" );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::GetRotation( quaternion* rot )
{
  n_error( "nOdePlaneShape::SetRotation() - operation not supported!" );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::SetPosition( const vector3& pos )
{
  n_error( "nOdePlaneShape::SetPosition() - operation not supported!" );
}

//------------------------------------------------------------------------------
void nOdePlaneShape::GetPosition( vector3* pos )
{
  n_error( "nOdePlaneShape::GetPosition() - operation not supported!" );
}
