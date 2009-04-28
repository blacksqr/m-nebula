#define N_IMPLEMENTS nOdeTriMeshShape
//------------------------------------------------------------------------------
//  nodetrimeshshape.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeTriMeshShape is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodetrimeshshape.h"

#ifndef N_ODESERVER_H
#include "odephysics/nodeserver.h"
#endif

#ifndef N_ODETRIMESH_H
#include "odephysics/nodetrimesh.h"
#endif

#ifndef N_ODECOLLIDESHAPE_H
#include "odephysics/nodecollideshape.h"
#endif

//#ifndef N_ODETRILIST_H
//#define N_ODETRILIST_H
//#include <ode/dtrimesh.h>
//#endif

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

nOdeServer* nOdeTriMesh::odeServer = NULL;

//------------------------------------------------------------------------------
/**
  @brief Construct new shape using a mesh.
  
  The mesh will be released once the reference count reaches zero.
*/
nOdeTriMeshShape::nOdeTriMeshShape( nOdeTriMesh* mesh )
{
  n_assert( mesh );
  this->triMesh = mesh;
  this->triMesh->AddRef();
  //this->geomId = dCreateTriMesh( 0 );
  this->geomId = dCreateTriMesh( 0, mesh->meshDataId, 0, 0, 0 );
  dGeomSetData( this->geomId, this );
  this->shapeType = nOdeCollideShape::OST_MESH;
}

//------------------------------------------------------------------------------
/**
*/
nOdeTriMeshShape::~nOdeTriMeshShape()
{
  dGeomDestroy( this->geomId );
  this->triMesh->RemRef();
  if ( 0 == this->triMesh->GetRef() )
  {
    this->triMesh->Remove();
    delete this->triMesh;
  }
}

//------------------------------------------------------------------------------
/**
*/
nOdeTriMesh* nOdeTriMeshShape::GetTriMesh() const
{
  return this->triMesh;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeTriMeshShape::EnableTemporalCoherence( nOdeShapeType shapeType, 
                                                bool enable )
{
  switch ( shapeType )
  {
    case nOdeCollideShape::OST_SPHERE:
      dGeomTriMeshEnableTC( this->geomId, dSphereClass, enable );
      break;
    case nOdeCollideShape::OST_CAPSULE:
      dGeomTriMeshEnableTC( this->geomId, dCCylinderClass, enable );
      break;
    case nOdeCollideShape::OST_BOX:
      dGeomTriMeshEnableTC( this->geomId, dBoxClass, enable );
      break;
    default:
      n_warn( "nOdeTriMeshShape::EnableTemporalCoherence() - Unsupported shape!" );
  }
}

//------------------------------------------------------------------------------
/**
*/
bool nOdeTriMeshShape::IsTemporalCoherenceEnabled( int shapeType ) const
{
  switch ( shapeType )
  {
    case nOdeCollideShape::OST_SPHERE:
      return dGeomTriMeshIsTCEnabled( this->geomId, dSphereClass );
    case nOdeCollideShape::OST_CAPSULE:
      return dGeomTriMeshIsTCEnabled( this->geomId, dCCylinderClass );
    case nOdeCollideShape::OST_BOX:
      return dGeomTriMeshIsTCEnabled( this->geomId, dBoxClass );
    default:
      n_warn( "nOdeTriMeshShape::IsTemporalCoherenceEnabled() - Unsupported shape!" );
      return false;
  }
}

//------------------------------------------------------------------------------
/**
*/
void nOdeTriMeshShape::VisualizeLocal( nGfxServer* gfxServer, 
                                       nPrimitiveServer* prim )
{
  this->triMesh->Visualize( gfxServer );  
}

