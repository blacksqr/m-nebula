#define N_IMPLEMENTS nOdeQuadTreeSpace
//------------------------------------------------------------------------------
//  nodequadtreespace.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeQuadTreeSpace is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodequadtreespace.h"
nNebulaScriptClass(nOdeQuadTreeSpace, "nodecollidespace");

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#include "mathlib/vector.h"

//------------------------------------------------------------------------------
nOdeQuadTreeSpace::nOdeQuadTreeSpace()
  : center(0, 0, 0), extents(0, 0, 0), depth(0)
{
}

//------------------------------------------------------------------------------
/**
  @brief Initialize the space.
  @warning Must be called before you try using the space!
*/
void nOdeQuadTreeSpace::SetParams( const vector3& center, 
                                   const vector3& extents, int depth )
{
  this->center = center;
  this->extents = extents;
  this->depth = depth;
  dVector3 v1, v2;
  v1[0] = center.x;
  v1[1] = center.y;
  v1[2] = center.z;
  v2[0] = extents.x;
  v2[1] = extents.y;
  v2[2] = extents.z;
  this->spaceId = dQuadTreeSpaceCreate( 0, v1, v2, depth );
  dGeomSetData( (dGeomID)this->spaceId, this );
}

//------------------------------------------------------------------------------
/**
  @brief Get the initialization params.
*/
void nOdeQuadTreeSpace::GetParams( vector3* center, vector3* extents, 
                                   int* depth )
{
  n_assert( center && extents && depth );
  center->set( this->center );
  extents->set( this->extents );
  *depth = this->depth;
}

//------------------------------------------------------------------------------
nOdeQuadTreeSpace::~nOdeQuadTreeSpace()
{
  // empty
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
