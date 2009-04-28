#define N_IMPLEMENTS nOdeHashSpace
//------------------------------------------------------------------------------
//  nodehashspace.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeHashSpace is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodehashspace.h"
nNebulaScriptClass(nOdeHashSpace, "nodecollidespace");

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

//------------------------------------------------------------------------------
nOdeHashSpace::nOdeHashSpace()
{
  this->spaceId = dHashSpaceCreate( 0 );
  dGeomSetData( (dGeomID)this->spaceId, this );
}

//------------------------------------------------------------------------------
nOdeHashSpace::~nOdeHashSpace()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Set the smallest and largest cell sizes to be used in the
         hash space.
*/
void nOdeHashSpace::SetLevels( int minLevel, int maxLevel )
{
  dHashSpaceSetLevels( this->spaceId, minLevel, maxLevel );
}

//------------------------------------------------------------------------------
/**
  @brief Get the smallest and largest cell sizes to be used in the
         hash space.
*/
void nOdeHashSpace::GetLevels( int* minLevel, int* maxLevel )
{
  dHashSpaceGetLevels( this->spaceId, minLevel, maxLevel );
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
