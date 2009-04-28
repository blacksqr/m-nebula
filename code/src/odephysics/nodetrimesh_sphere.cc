#define N_IMPLEMENTS nOdeTriMesh
//--------------------------------------------------------------------
//  nodetrimesh_sphere.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeTriMesh is licensed under the terms of the Nebula License.
//--------------------------------------------------------------------
#include "odephysics/nodetrimesh.h"
#include "odephysics/odeutil.h"
#include "mathlib/vector.h"
#include "mathlib/triangle.h"
#include "mathlib/sphere.h"
#include "odephysics/ncollisionmath.h"
#include "odephysics/nodetrimeshshape.h"
//------------------------------------------------------------------------------
/**
  @brief Compute contact points between a sphere and a triangle mesh.
*/
int nOdeTriMesh::CollideSphereMesh( dxGeom* triList, dxGeom* sphereGeom, 
                                    int flags, dContactGeom* contacts, 
                                    int stride )
{
  nOdeTriMeshShape* shape = (nOdeTriMeshShape*)dGeomGetData( triList );
  nOdeTriMesh* mesh = shape->GetTriMesh();
      
  int retval = 0;
  
  // obtain a list of triangles the sphere overlaps
  // setup sphere collider
  SphereCollider& collider = nOdeTriMesh::odeServer->opcSphereCollider;
  //VolumeCollider* colliderPtr = (VolumeCollider*)&collider;
  SphereCache& cache = nOdeTriMesh::odeServer->opcSphereCache;
  collider.SetCallback( mesh->collCallback, (udword)mesh );
  collider.SetFirstContact( false );
  // convert matrix44 to Opcode Matrix4x4
  Matrix4x4 opcMeshMatrix;
  
  nOdeUtil::GetOpcodeMatrix( triList, &opcMeshMatrix );
  // convert opcode matrix to neb matrix
  matrix44 meshMatrix( &(opcMeshMatrix.m[0][0]) );
  // build an Opcode Sphere
  const dReal* pos = dGeomGetPosition( sphereGeom );
  dReal rad = dGeomSphereGetRadius ( sphereGeom );
  Point ptCenter( pos[0], pos[1], pos[2] );
  const Sphere opcSphere( ptCenter, rad );
  // perform collision
  collider.Collide( cache, opcSphere, &(mesh->opcModel),
                    NULL/* sphere already in world space */, &opcMeshMatrix );
  
  // if some triangles were found
  if ( collider.GetContactStatus() )
  {
    triangle tri;
    int triCount = collider.GetNbTouchedPrimitives();
    int outTriCount = 0;
    const udword* collFaces = collider.GetTouchedPrimitives();
    // create sphere in world space
    sphere theSphere( pos[0], pos[1], pos[2], rad );
    vector3 v0, v1, v2;
    vector3 tv0, tv1, tv2;
    int i;
    for ( i = 0; i < triCount; i++ )
    {
      // get touched triangle in model space
      mesh->GetTriCoords( collFaces[i], v0, v1, v2 );
            
      // transform triangle into world space
      tv0 = meshMatrix * v0;
      tv1 = meshMatrix * v1;
      tv2 = meshMatrix * v2;
      tri.set( tv0, tv1, tv2 );
      
      float u, v, w;
      if ( !nCollisionMath::IntersectTriSphere( theSphere, tri, 0, &u, &v ) )
      {
        continue; // Sphere doesn't overlap with triangle
      }
      w = 1.0f - u - v;
            
      // note: plane normal will be reverse of the corresponding triangle's normal
      // plane will also be normalized
      plane triPlane( tv0, tv1, tv2 );
      vector3 planeNormal = triPlane.normal();
      
      float contactDepth = (planeNormal % theSphere.p) + triPlane.d + theSphere.r;
      
      dContactGeom* contact = 
        nOdeUtil::FetchContact( flags, contacts, outTriCount, stride );
              
      // compute & store contact position
      contact->pos[0] = tv0.x * w + tv1.x * u + tv2.x * v;
      contact->pos[1] = tv0.y * w + tv1.y * u + tv2.y * v;
      contact->pos[2] = tv0.z * w + tv1.z * u + tv2.z * v;
      // using this version undesirable rolling can be avoided
      // but it introduces other problems
      //vector3 contactPos = theSphere.p + planeNormal * (theSphere.r - contactDepth);
      //contact->pos[0] = contactPos.x;
      //contact->pos[1] = contactPos.y;
      //contact->pos[2] = contactPos.z;
      contact->pos[3] = 0.0f;
      
      // store contact normal
      contact->normal[0] = planeNormal.x;
      contact->normal[1] = planeNormal.y;
      contact->normal[2] = planeNormal.z;
      contact->normal[3] = 0.0f;
      contact->depth = contactDepth;
            
      ++outTriCount;
    } // loop: for each triangle
        
#ifdef MERGECONTACTS
    if ( outTriCount != 0 )
    {
      /*
        mutilate everything into one contact point,
        err, I mean merge :)
      */
      dContactGeom* contact = nOdeUtil::FetchContact( flags, contacts, 0, stride );
              
      for ( i = 1; i < outTriCount; i++ )
      {
        dContactGeom* tempContact = 
          nOdeUtil::FetchContact( flags, contacts, i, stride );
        
        contact->pos[0] += tempContact->pos[0];
        contact->pos[1] += tempContact->pos[1];
        contact->pos[2] += tempContact->pos[2];
        
        contact->normal[0] += tempContact->normal[0] * contact->depth;
        contact->normal[1] += tempContact->normal[1] * contact->depth;
        contact->normal[2] += tempContact->normal[2] * contact->depth;
      }

      // average out the contact position
      contact->pos[0] /= outTriCount;
      contact->pos[1] /= outTriCount;
      contact->pos[2] /= outTriCount;
      
      // remember to divide in square space.
      vector3 outNormal( contact->normal[0], contact->normal[1], contact->normal[2] );
      contact->depth = n_sqrt( (outNormal % outNormal) / outTriCount );
      dNormalize3( contact->normal );
      
      contact->g1 = triList;
      contact->g2 = sphereGeom;
            
      retval = 1;

    } // if outTriCount != 0
#else
    for ( i = 0; i < outTriCount; i++ )
    {
      dContactGeom* contact = nOdeUtil::FetchContact( flags, contacts, i, stride );
      contact->g1 = triList;
      contact->g2 = sphereGeom;
    }

    retval = outTriCount;
#endif // MERGECONTACTS
  }
    
#ifdef MERGECONTACTS
#undef MERGECONTACTS
#endif
    
  return retval;
}

//------------------------------------------------------------------------------
