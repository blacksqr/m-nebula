#define N_IMPLEMENTS nOdeTriMesh
//--------------------------------------------------------------------
//  nodetrimesh_capsule.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeTriMesh is licensed under the terms of the Nebula License.
//--------------------------------------------------------------------
#include "odephysics/nodetrimesh.h"
#include "odephysics/nodetrimeshshape.h"
#include "mathlib/matrix.h"
#include "mathlib/capsule.h"
#include "mathlib/triangle.h"
#include "odephysics/odeutil.h"
#include "odephysics/ncollisionmath.h"
//--------------------------------------------------------------------
/**
  @brief Compute contact points between a capsule and a triangle mesh.
  
  Overview of the algorithm:
  Create AABB enclosing the capsule and query OPCODE for all triangles
  that overlap with the box.
  For each triangle, check for overlap of capsule with triangle, if one 
  is detected generate at most 2 contact points and merge them with any 
  previously found contacts.
  Average out the accumulated contact points and normals to obtain at
  most 2 contact points that will get returned.
  
  FIXME:
  Identical contacts should probably be eliminated, currently this method 
  will always produce 2 contacts, even when there is only 1 unique 
  contact possible (eg. capsule standing on one of it's ends).
*/
int nOdeTriMesh::CollideCapsuleMesh( dxGeom* triMesh, dxGeom* capGeom, 
                                     int flags, dContactGeom* contacts, 
                                     int stride )
{
  nOdeTriMeshShape* shape = (nOdeTriMeshShape*)dGeomGetData( triMesh );
  nOdeTriMesh* mesh = shape->GetTriMesh();
  
  int retval = 0;
  
  // obtain a list of triangles the capsule may overlap
  
  /*
    Since there is no Capsule/Mesh test in OPCODE we do an AABB/Mesh
    test instead, alternatively could do OBB/Mesh test,
    an OBB would fit the capsule better but I opted for AABB since
    it's less work for me :)
  */
  
  // setup AABB collider
  AABBCollider& collider = nOdeTriMesh::odeServer->opcAABBCollider;
  AABBCache& cache = nOdeTriMesh::odeServer->opcAABBCache;
  collider.SetCallback( mesh->collCallback, (udword)mesh );
  collider.SetFirstContact( false );
  
  dReal capRad, capLength;
  dGeomCCylinderGetParams( capGeom, &capRad, &capLength );
  matrix44 nebMeshMat, nebCapMat;
  nOdeUtil::GetNebMatrix( triMesh, &nebMeshMat );
  nOdeUtil::GetNebMatrix( capGeom, &nebCapMat );
  matrix44 nebInverseMeshMat( nebMeshMat );
  nebInverseMeshMat.invert_simple();
  // compute transform from capsule model space -> mesh model space
  matrix44 localCapMat = nebCapMat * nebInverseMeshMat;
  
  /* 
    hmm, not sure about this, I expect these to be the endpoints of the line
    segment representing the capsule, I'm assuming the center of the capsule
    is at (0,0,0) - but I'm not sure if that's what ODE does.
  */
  vector3 p1( 0.0f, 0.0f, capLength * 0.5f );
  vector3 p2( 0.0f, 0.0f, -p1.z );
  
  // transform capsule to mesh model space
  p1 = localCapMat * p1;
  p2 = localCapMat * p2;
  
  // build an AABB from capsule (in mesh model space)
  AABB box;
  Point minP( n_min( p1.x, p2.x ) - capRad,
              n_min( p1.y, p2.y ) - capRad,
              n_min( p1.z, p2.z ) - capRad );
  Point maxP( n_max( p1.x, p2.x ) + capRad,
              n_max( p1.y, p2.y ) + capRad,
              n_max( p1.z, p2.z ) + capRad );
  box.SetMinMax( minP, maxP );
  
  // perform collision
  collider.Collide( cache, CollisionAABB( box ), &(mesh->opcModel) );
  
  // if some triangles were found
  if ( collider.GetContactStatus() )
  {
    triangle tri;
    int triCount = collider.GetNbTouchedPrimitives();
    int outTriCount = 0;
    const udword* collFaces = collider.GetTouchedPrimitives();
    vector3 v0, v1, v2;
    vector3 tv0, tv1, tv2;
    // accumulate contact info for merging later
    dContactGeom accumContacts[3];
    memset( accumContacts, 0, sizeof(dContactGeom) * 3 );
    int accum[3] = { 0, 0, 0 };
    
    capsule theCapsule( p1, p2, capRad );
    
    for ( int i = 0; i < triCount; i++ )
    {
      // get touched triangle in model space
      mesh->GetTriCoords( collFaces[i], v0, v1, v2 );
            
      // transform triangle into world space
      tv0 = nebMeshMat * v0;
      tv1 = nebMeshMat * v1;
      tv2 = nebMeshMat * v2;
      tri.set( tv0, tv1, tv2 );
            
      float t;  // t value of a point along the capsule's axis
      if ( !nCollisionMath::IntersectTriCapsule( theCapsule, tri, 0, &t, 0, 0 ) )
      {
        continue; // capsule doesn't overlap with triangle
      }
      
      // note: plane normal will be reverse of the corresponding triangle's normal
      // plane will also be normalized
      plane triPlane( tv0, tv1, tv2 );
      vector3 planeNormal = triPlane.normal();
      float depth;
      vector3 contactPos;
      dContactGeom* contact;
      
      /*
        If both end spheres overlap with the triangle then generate
        2 contacts from them.
        If only one of the end spheres overlap with the triangle then 
        generate 2 contacts, one from one of the end spheres and the 
        other from from a sphere centered at 't' along the capsule's
        central axis.
      */
            
      depth = triPlane.distance( theCapsule.origin() ) + theCapsule.r;
      if ( depth >= 0 )
      //if ( (depth >= 0) && (accum[0] < 1) )
      {
        // store the contact between sphere 1 & tri plane
        contact = &(accumContacts[0]);
        contactPos = theCapsule.origin() + planeNormal * (theCapsule.r - depth);
        contact->pos[0] += contactPos.x;
        contact->pos[1] += contactPos.y;
        contact->pos[2] += contactPos.z;
        contact->normal[0] += planeNormal.x * depth;
        contact->normal[1] += planeNormal.y * depth;
        contact->normal[2] += planeNormal.z * depth;
        ++accum[0];
      }
      
      depth = triPlane.distance( theCapsule.seg.end() ) + theCapsule.r;
      if ( depth >= 0 )
      //if ( (depth >= 0) && (accum[1] < 1) )
      {
        // store the contact between sphere 2 & tri plane
        contact = &(accumContacts[1]);
        contactPos = theCapsule.seg.end() + planeNormal * (theCapsule.r - depth);
        contact->pos[0] += contactPos.x;
        contact->pos[1] += contactPos.y;
        contact->pos[2] += contactPos.z;
        contact->normal[0] += planeNormal.x * depth;
        contact->normal[1] += planeNormal.y * depth;
        contact->normal[2] += planeNormal.z * depth;
        ++accum[1];
      }
      
      if ( !(accum[0] & accum[1]) )
      //if ( (!(accum[0] & accum[1])) && (accum[2] < 1) )
      {
        vector3 p = theCapsule.seg.ipol( t );
        depth = triPlane.distance( p ) + theCapsule.r;
        if ( depth >= 0 )
        {
          // store the contact
          contact = &(accumContacts[2]);
          contactPos = p + planeNormal * (theCapsule.r - depth);
          contact->pos[0] += contactPos.x;
          contact->pos[1] += contactPos.y;
          contact->pos[2] += contactPos.z;
          contact->normal[0] += planeNormal.x * depth;
          contact->normal[1] += planeNormal.y * depth;
          contact->normal[2] += planeNormal.z * depth;
          ++accum[2];
        }
      }
                  
      ++outTriCount;
    } // loop: for each triangle
 
    if ( outTriCount != 0 )
    {
      dContactGeom* contact;
      dContactGeom* accumContact;
      vector3 outNorm;
      retval = 0;
      
      // average out the contacts, normals and determine depths
      if ( accum[0] > 0 )
      {
        contact = nOdeUtil::FetchContact( flags, contacts, retval, stride );
        accumContact = &(accumContacts[0]);
        float rec = 1.0f / (float)accum[0];
        contact->pos[0] = accumContact->pos[0] * rec;
        contact->pos[1] = accumContact->pos[1] * rec;
        contact->pos[2] = accumContact->pos[2] * rec;
        contact->pos[3] = 0;
        outNorm.set( accumContact->normal[0], 
                     accumContact->normal[1], 
                     accumContact->normal[2] );
        //contact->depth = n_sqrt( (outNorm % outNorm) * rec );
        contact->depth = n_sqrt( outNorm % outNorm ) * rec;
        contact->normal[0] = outNorm.x;
        contact->normal[1] = outNorm.y;
        contact->normal[2] = outNorm.z;
        contact->normal[3] = 0;
        dNormalize3( contact->normal );
        contact->g1 = triMesh;
        contact->g2 = capGeom;
        ++retval;
      }
      if ( accum[1] > 0 )
      {
        contact = nOdeUtil::FetchContact( flags, contacts, retval, stride );
        accumContact = &(accumContacts[1]);
        float rec = 1.0f / (float)accum[1];
        contact->pos[0] = accumContact->pos[0] * rec;
        contact->pos[1] = accumContact->pos[1] * rec;
        contact->pos[2] = accumContact->pos[2] * rec;
        contact->pos[3] = 0;
        outNorm.set( accumContact->normal[0], 
                     accumContact->normal[1], 
                     accumContact->normal[2] );
        //contact->depth = n_sqrt( (outNorm % outNorm) * rec );
        contact->depth = n_sqrt( outNorm % outNorm ) * rec;
        contact->normal[0] = outNorm.x;
        contact->normal[1] = outNorm.y;
        contact->normal[2] = outNorm.z;
        contact->normal[3] = 0;
        dNormalize3( contact->normal );
        contact->g1 = triMesh;
        contact->g2 = capGeom;
        ++retval;
      }
      // use 3rd contact only if we don't yet have 2 contacts or got multiple tris
      if ( (accum[2] > 0) && ((!(accum[0] & accum[1])) || (outTriCount > 1)) )
      {
        contact = nOdeUtil::FetchContact( flags, contacts, retval, stride );
        accumContact = &(accumContacts[2]);
        float rec = 1.0f / (float)accum[2];
        contact->pos[0] = accumContact->pos[0] * rec;
        contact->pos[1] = accumContact->pos[1] * rec;
        contact->pos[2] = accumContact->pos[2] * rec;
        contact->pos[3] = 0;
        outNorm.set( accumContact->normal[0], 
                     accumContact->normal[1], 
                     accumContact->normal[2] );
        //contact->depth = n_sqrt( (outNorm % outNorm) * rec );
        contact->depth = n_sqrt( outNorm % outNorm ) * rec;
        contact->normal[0] = outNorm.x;
        contact->normal[1] = outNorm.y;
        contact->normal[2] = outNorm.z;
        contact->normal[3] = 0;
        dNormalize3( contact->normal );
        contact->g1 = triMesh;
        contact->g2 = capGeom;
        ++retval;
      }
    } // if outTriCount != 0
  }  
  
  return retval;
}