#define N_IMPLEMENTS nOdeSphereShape
//------------------------------------------------------------------------------
//  nodesphereshape.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeSphereShape is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodesphereshape.h"

#ifndef N_PRIMITIVESERVER_H
#include "gfx/nprimitiveserver.h"
#endif

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

//------------------------------------------------------------------------------
/**
*/
nOdeSphereShape::nOdeSphereShape()
{
  // create sphere with default radius = 1
  this->geomId = dCreateSphere( 0, 1 );
  dGeomSetData( this->geomId, (void*)this );
  this->shapeType = nOdeCollideShape::OST_SPHERE;
}

//------------------------------------------------------------------------------
/**
*/
nOdeSphereShape::~nOdeSphereShape()
{
  dGeomDestroy( this->geomId );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeSphereShape::SetRadius( float rad )
{
  dGeomSphereSetRadius( this->geomId, rad );
}

//------------------------------------------------------------------------------
/**
*/
float nOdeSphereShape::GetRadius()
{
  return dGeomSphereGetRadius( this->geomId );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeSphereShape::VisualizeLocal( nGfxServer* gs, nPrimitiveServer* prim )
{
  prim->SolidSphere( this->GetRadius(), 16, 16 );
}
