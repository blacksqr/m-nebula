#define N_IMPLEMENTS nOdeBoxShape
//------------------------------------------------------------------------------
//  nodeboxshape.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeBoxShape is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeboxshape.h"

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
nOdeBoxShape::nOdeBoxShape()
{
  // create a 1x1x1 cube
  this->geomId = dCreateBox( 0, 1, 1, 1 );
  dGeomSetData( this->geomId, (void*)this );
  this->shapeType = nOdeCollideShape::OST_BOX;
}

//------------------------------------------------------------------------------
/**
*/
nOdeBoxShape::~nOdeBoxShape()
{
  dGeomDestroy( this->geomId );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeBoxShape::SetSize( float xsize, float ysize, float zsize )
{
  dGeomBoxSetLengths( this->geomId, xsize, ysize, zsize );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeBoxShape::GetSize( float* xsize, float* ysize, float* zsize )
{
  dVector3 res;
  dGeomBoxGetLengths( this->geomId, res );
  *xsize = res[0]; *ysize = res[1]; *zsize = res[2];
}

//------------------------------------------------------------------------------
/**
*/
void nOdeBoxShape::VisualizeLocal( nGfxServer* gs, nPrimitiveServer* prim )
{
  float lx, ly, lz;
  this->GetSize( &lx, &ly, &lz );
  
  //matrix44 ivm, m;
  // get the current inverse view matrix
  //gs->GetMatrix( N_MXM_INVVIEWER, ivm );
  //gs->PushMatrix( N_MXM_MODELVIEW );
  //this->GetTransform( &m );
  //m.mult_simple( ivm );
  //gs->SetMatrix( N_MXM_MODELVIEW, m );
  prim->WireBox( lx, ly, lz );
  //gs->PopMatrix( N_MXM_MODELVIEW );

  /*
  // sides
  gs->Begin( N_PTYPE_TRIANGLE_STRIP );
  gs->Norm( -1, 0, 0 );
  gs->Coord( -lx, -ly, -lz );
  gs->Coord( -lx, -ly, lz );
  gs->Coord( -lx, ly, -lz );
  gs->Coord( -lx, ly, lz );
  gs->Norm( 0, 1, 0 );
  gs->Coord( lx, ly, -lz );
  gs->Coord( lx, ly, lz );
  gs->Norm( 1, 0, 0 );
  gs->Coord( lx, -ly, -lz );
  gs->Coord( lx, -ly, lz );
  gs->Norm( 0, -1, 0 );
  gs->Coord( -lx, -ly, -lz );
  gs->Coord( -lx, -ly, lz );
  gs->End();

  // top face
  gs->Begin( N_PTYPE_TRIANGLE_FAN );
  gs->Norm( 0, 0, 1 );
  gs->Coord( -lx, -ly, lz );
  gs->Coord( lx, -ly, lz );
  gs->Coord( lx, ly, lz );
  gs->Coord( -lx, ly, lz );
  gs->End();

  // bottom face
  gs->Begin( N_PTYPE_TRIANGLE_FAN );
  gs->Norm( 0, 0, -1 );
  gs->Coord( -lx, -ly, -lz );
  gs->Coord( -lx, ly, -lz );
  gs->Coord( lx, ly, -lz );
  gs->Coord( lx, -ly, -lz );
  gs->End();
  */
}
