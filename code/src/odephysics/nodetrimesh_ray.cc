#define N_IMPLEMENTS nOdeTriMesh
//--------------------------------------------------------------------
//  nodetrimesh_ray.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeTriMesh is licensed under the terms of the Nebula License.
//--------------------------------------------------------------------
#include "odephysics/nodetrimesh.h"
#include "odephysics/nodetrimeshshape.h"
#include "mathlib/matrix.h"
#include "mathlib/triangle.h"
#include "odephysics/odeutil.h"
#include "odephysics/ncollisionmath.h"

//--------------------------------------------------------------------
/**
  @brief Compute contact point between a line/ray and a triangle mesh.
*/
int nOdeTriMesh::CollideRayMesh( dxGeom* triMesh, dxGeom* rayGeom, 
                                 int flags, dContactGeom* contacts, 
                                 int stride )
{
  nOdeTriMeshShape* shape = (nOdeTriMeshShape*)dGeomGetData( triMesh );
  nOdeTriMesh* mesh = shape->GetTriMesh();
  
  int retval = 0;
  
  // setup ray collider
  RayCollider& collider = nOdeTriMesh::odeServer->opcRayCollider;
  collider.SetCallback( mesh->collCallback, (udword)mesh );
  collider.SetMaxDist( dGeomRayGetLength( rayGeom ) );
  collider.SetClosestHit( false );
  
  switch ( nOdeCollideShape::rayCollType )
  {
    case ODE_COLLTYPE_QUICK:
    case ODE_COLLTYPE_EXACT:
      collider.SetFirstContact( true );
      break;
    case ODE_COLLTYPE_CONTACT:
      collider.SetFirstContact( false );
      break;
    default:
      break;
  }
  
  Matrix4x4 opcMeshMatrix;
  nOdeUtil::GetOpcodeMatrix( triMesh, &opcMeshMatrix );
  // convert opcode matrix to neb matrix
  matrix44 meshMatrix( &(opcMeshMatrix.m[0][0]) );

  // build Opcode ray
  dVector3 start, dir;
  dGeomRayGet( rayGeom, start, dir );
  Ray ray;
  ray.mOrig.Set( start[0], start[1], start[2] );
  ray.mDir.Set( dir[0], dir[1], dir[2] );

  // perform collision
  collider.Collide( ray, &(mesh->opcModel), &opcMeshMatrix );

  // get collision result
  if ( collider.GetContactStatus() )
  {
    // fill out contact point and collision normal of closest contact
    const CollisionFace* collFaces = nOdeTriMesh::odeServer->opcFaceCache.GetFaces();
    
    int numFaces = nOdeTriMesh::odeServer->opcFaceCache.GetNbFaces();
    if ( numFaces > 0 )
    {
      // if in closest hit mode, find the contact with the smallest distance
      int collFaceIndex = 0;
      if ( ODE_COLLTYPE_CONTACT == nOdeCollideShape::rayCollType )
      {
        for ( int i = 0; i < numFaces; i++ )
        {
          if ( collFaces[i].mDistance < collFaces[collFaceIndex].mDistance )
            collFaceIndex = i;
        }
      }
      
      int triangleIndex = collFaces[collFaceIndex].mFaceID;
      float dist        = collFaces[collFaceIndex].mDistance;

      // build triangle from from faceIndex
      vector3 v0,v1,v2;
      mesh->GetTriCoords( triangleIndex, v0, v1, v2 );
      triangle tri( v0, v1, v2 );
      
      // get 3x3 matrix to transform normal into global space
      matrix33 m33( meshMatrix.M11, meshMatrix.M12, meshMatrix.M13, 
                    meshMatrix.M21, meshMatrix.M22, meshMatrix.M23, 
                    meshMatrix.M31, meshMatrix.M32, meshMatrix.M33 ); 

      // tranform normal to global space
      vector3 triNormal( m33 * tri.normal() );

      // record contact
      dContactGeom* contact = nOdeUtil::FetchContact( flags, contacts, 
                                                      0, stride );
      contact->pos[0] = start[0] + (dir[0] * dist);
      contact->pos[1] = start[1] + (dir[1] * dist);
      contact->pos[2] = start[2] + (dir[2] * dist);
      contact->pos[3] = 0;
      // surface normal of triangle at contact point
      contact->normal[0] = triNormal.x;
      contact->normal[1] = triNormal.y;
      contact->normal[2] = triNormal.z;
      contact->normal[3] = 0;
      contact->depth = dist;
      contact->g1 = triMesh;
      contact->g2 = rayGeom;
        
      retval = 1;
    }
    else
      n_printf( "nOdeTriMesh::CollideRayMesh(): Contact but no faces!\n" );
  }
  return retval;
}
