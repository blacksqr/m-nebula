#define N_IMPLEMENTS nOdeCapsuleShape
//------------------------------------------------------------------------------
//  nodecapsuleshape.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeCapsuleShape is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodecapsuleshape.h"

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

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
nOdeCapsuleShape::nOdeCapsuleShape()
{
  // create capsule with default radius = 1 and length = 1
  this->geomId = dCreateCCylinder( 0, 1, 1 );
  dGeomSetData( this->geomId, (void*)this );
  this->shapeType = nOdeCollideShape::OST_CAPSULE;
}

//------------------------------------------------------------------------------
/**
*/
nOdeCapsuleShape::~nOdeCapsuleShape()
{
  dGeomDestroy( this->geomId );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCapsuleShape::SetSize( float rad, float len )
{
  dGeomCCylinderSetParams( this->geomId, rad, len );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCapsuleShape::GetSize( float* rad, float* len )
{
  dReal radius = 0;
  dReal length = 0;
  dGeomCCylinderGetParams( this->geomId, &radius, &length );
  *rad = radius; 
  *len = length;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCapsuleShape::VisualizeLocal( nGfxServer* gs, 
                                       nPrimitiveServer* prim )
{
  float rad, len;
  this->GetSize( &rad, &len );
  matrix44 mv, m;
  // get the current inverse view matrix
  gs->GetMatrix( N_MXM_MODELVIEW, mv );
  gs->PushMatrix( N_MXM_MODELVIEW );
  m.ident();
  m.translate( 0, 0, -len * 0.5f );
  m.mult_simple( mv );
  gs->SetMatrix( N_MXM_MODELVIEW, m );
  prim->SolidCapsule( rad, len, 16, 8 );
  gs->PopMatrix( N_MXM_MODELVIEW );
}
