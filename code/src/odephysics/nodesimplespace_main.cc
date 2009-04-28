#define N_IMPLEMENTS nOdeSimpleSpace
//------------------------------------------------------------------------------
//  nodesimplespace.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeSimpleSpace is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodesimplespace.h"
nNebulaClass(nOdeSimpleSpace, "nodecollidespace");

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

//------------------------------------------------------------------------------
nOdeSimpleSpace::nOdeSimpleSpace()
{
  this->spaceId = dSimpleSpaceCreate( 0 );
  dGeomSetData( (dGeomID)this->spaceId, this );
}

//------------------------------------------------------------------------------
nOdeSimpleSpace::~nOdeSimpleSpace()
{
  // empty
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
