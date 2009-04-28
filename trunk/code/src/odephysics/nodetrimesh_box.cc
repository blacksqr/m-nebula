#define N_IMPLEMENTS nOdeTriMesh
//--------------------------------------------------------------------
//  nodetrimesh_box.cc
//
//  (c) 2003  Vadim Macagon
//
//  Most of this box collision stuff was ripped out of the ODE
//  tri-collider (gl's version I believe).
//
//  nOdeTriMesh is licensed under the terms of the Nebula License.
//--------------------------------------------------------------------
#include "odephysics/nodetrimesh.h"
#include "mathlib/matrix.h"
#include "odephysics/nodetrimeshshape.h"
#include "odephysics/odeutil.h"
#include "odephysics/nodeserver.h"
//------------------------------------------------------------------------------
struct LineContactSet
{
  dVector3 Points[8];
  int Count;
};

const dReal Epsilon = REAL(1e-08);

namespace nOdeUtil
{
  void ComputeVertices( const dVector3 Center, const dVector3 Extents, 
                        const dMatrix3 Matrix, dVector3 Vertices[8] )
  {
    dVector3 Axis[3];
    nOdeUtil::DecomposeMatrix(Matrix, Axis);

    dVector3 TransExtents[3];
    for (int i = 0; i < 3; i++)
    {
      TransExtents[i][0] = Axis[i][0] * Extents[i];
      TransExtents[i][1] = Axis[i][1] * Extents[i];
      TransExtents[i][2] = Axis[i][2] * Extents[i];
      TransExtents[i][3] = Axis[i][3] * Extents[i];
    }

    #define COMPUTEVERTEX(a, op1, b, op2, c, op3, d, op4, e)  \
      a[0] op1 b[0] op2 c[0] op3 d[0] op4 e[0];       \
      a[1] op1 b[1] op2 c[1] op3 d[1] op4 e[1];       \
      a[2] op1 b[2] op2 c[2] op3 d[2] op4 e[2];       \
      a[3] op1 REAL(0.0);

    COMPUTEVERTEX(Vertices[0], =, Center, -, TransExtents[0], +, TransExtents[1], +, TransExtents[2]);
    COMPUTEVERTEX(Vertices[1], =, Center, +, TransExtents[0], +, TransExtents[1], +, TransExtents[2]);
    COMPUTEVERTEX(Vertices[2], =, Center, +, TransExtents[0], -, TransExtents[1], +, TransExtents[2]);
    COMPUTEVERTEX(Vertices[3], =, Center, -, TransExtents[0], -, TransExtents[1], +, TransExtents[2]);
    COMPUTEVERTEX(Vertices[4], =, Center, -, TransExtents[0], +, TransExtents[1], -, TransExtents[2]);
    COMPUTEVERTEX(Vertices[5], =, Center, +, TransExtents[0], +, TransExtents[1], -, TransExtents[2]);
    COMPUTEVERTEX(Vertices[6], =, Center, +, TransExtents[0], -, TransExtents[1], -, TransExtents[2]);
    COMPUTEVERTEX(Vertices[7], =, Center, -, TransExtents[0], -, TransExtents[1], -, TransExtents[2]);
    #undef COMPUTEVERTEX
  }

  //------------------------------------------------------------------------------
  /**
  */
  void ClipConvexPolygonAgainstPlane( const dVector3 N, dReal C, 
                                      LineContactSet& Contacts )
  {
      // The input vertices are assumed to be in counterclockwise order.  The
      // ordering is an invariant of this function.
    
      // test on which side of line are the vertices
      int Positive = 0, Negative = 0, PIndex = -1;
      int Quantity = Contacts.Count;
    
      dReal Test[8];
      for ( int i = 0; i < Contacts.Count; i++ )
      {
          // An epsilon is used here because it is possible for the dot product
          // and fC to be exactly equal to each other (in theory), but differ
          // slightly because of floating point problems.  Thus, add a little
          // to the test number to push actually equal numbers over the edge
          // towards the positive.
      
          // NOLAN: This should probably be somehow a relative tolerance, and
          // I don't think multiplying by the constant is somehow the best
          // way to do this.
          Test[i] = dDOT(N, Contacts.Points[i]) - C + dFabs(C) * Epsilon;
      
          if (Test[i] >= REAL(0.0))
          {
              Positive++;
              if (PIndex < 0)
              {
                  PIndex = i;
              }
          }
          else Negative++;
      }
    
    if (Positive > 0)
    {
      if (Negative > 0)
      {
        // plane transversely intersects polygon
        dVector3 CV[8];
        int CQuantity = 0, Cur, Prv;
        dReal T;
        
        if (PIndex > 0)
        {
          // first clip vertex on line
          Cur = PIndex;
          Prv = Cur - 1;
          T = Test[Cur] / (Test[Cur] - Test[Prv]);
          CV[CQuantity][0] = Contacts.Points[Cur][0] 
                            + T * (Contacts.Points[Prv][0] - Contacts.Points[Cur][0]);
          CV[CQuantity][1] = Contacts.Points[Cur][1] 
                            + T * (Contacts.Points[Prv][1] - Contacts.Points[Cur][1]);
          CV[CQuantity][2] = Contacts.Points[Cur][2] 
                            + T * (Contacts.Points[Prv][2] - Contacts.Points[Cur][2]);
          CV[CQuantity][3] = Contacts.Points[Cur][3] 
                            + T * (Contacts.Points[Prv][3] - Contacts.Points[Cur][3]);
          CQuantity++;
          
          // vertices on positive side of line
          while (Cur < Quantity && Test[Cur] >= REAL(0.0))
          {
            CV[CQuantity][0] = Contacts.Points[Cur][0];
            CV[CQuantity][1] = Contacts.Points[Cur][1];
            CV[CQuantity][2] = Contacts.Points[Cur][2];
            CV[CQuantity][3] = Contacts.Points[Cur][3];
            CQuantity++;
            Cur++;
          }
          
          // last clip vertex on line
          if (Cur < Quantity)
          {
            Prv = Cur - 1;
          }
          else
          {
            Cur = 0;
            Prv = Quantity - 1;
          }
          T = Test[Cur] / (Test[Cur] - Test[Prv]);
          CV[CQuantity][0] = Contacts.Points[Cur][0] 
                            + T * (Contacts.Points[Prv][0] - Contacts.Points[Cur][0]);
          CV[CQuantity][1] = Contacts.Points[Cur][1] 
                            + T * (Contacts.Points[Prv][1] - Contacts.Points[Cur][1]);
          CV[CQuantity][2] = Contacts.Points[Cur][2] 
                            + T * (Contacts.Points[Prv][2] - Contacts.Points[Cur][2]);
          CV[CQuantity][3] = Contacts.Points[Cur][3] 
                            + T * (Contacts.Points[Prv][3] - Contacts.Points[Cur][3]);
          CQuantity++;
        }
        else
        { // iPIndex is 0
          // vertices on positive side of line
          Cur = 0;
          while (Cur < Quantity && Test[Cur] >= REAL(0.0))
          {
            CV[CQuantity][0] = Contacts.Points[Cur][0];
            CV[CQuantity][1] = Contacts.Points[Cur][1];
            CV[CQuantity][2] = Contacts.Points[Cur][2];
            CV[CQuantity][3] = Contacts.Points[Cur][3];
            CQuantity++;
            Cur++;
          }
          
          // last clip vertex on line
          Prv = Cur - 1;
          T = Test[Cur] / (Test[Cur] - Test[Prv]);
          CV[CQuantity][0] = Contacts.Points[Cur][0] 
                            + T * (Contacts.Points[Prv][0] - Contacts.Points[Cur][0]);
          CV[CQuantity][1] = Contacts.Points[Cur][1] 
                            + T * (Contacts.Points[Prv][1] - Contacts.Points[Cur][1]);
          CV[CQuantity][2] = Contacts.Points[Cur][2] 
                            + T * (Contacts.Points[Prv][2] - Contacts.Points[Cur][2]);
          CV[CQuantity][3] = Contacts.Points[Cur][3] 
                            + T * (Contacts.Points[Prv][3] - Contacts.Points[Cur][3]);
          CQuantity++;
          
          // skip vertices on negative side
          while (Cur < Quantity && Test[Cur] < REAL(0.0))
          {
            Cur++;
          }
          
          // first clip vertex on line
          if (Cur < Quantity)
          {
            Prv = Cur - 1;
            T = Test[Cur] / (Test[Cur] - Test[Prv]);
            CV[CQuantity][0] = Contacts.Points[Cur][0] 
                              + T * (Contacts.Points[Prv][0] - Contacts.Points[Cur][0]);
            CV[CQuantity][1] = Contacts.Points[Cur][1] 
                              + T * (Contacts.Points[Prv][1] - Contacts.Points[Cur][1]);
            CV[CQuantity][2] = Contacts.Points[Cur][2] 
                              + T * (Contacts.Points[Prv][2] - Contacts.Points[Cur][2]);
            CV[CQuantity][3] = Contacts.Points[Cur][3] 
                              + T * (Contacts.Points[Prv][3] - Contacts.Points[Cur][3]);
            CQuantity++;
            
            // vertices on positive side of line
            while (Cur < Quantity && Test[Cur] >= REAL(0.0))
            {
              CV[CQuantity][0] = Contacts.Points[Cur][0];
              CV[CQuantity][1] = Contacts.Points[Cur][1];
              CV[CQuantity][2] = Contacts.Points[Cur][2];
              CV[CQuantity][3] = Contacts.Points[Cur][3];
              CQuantity++;
              Cur++;
            }
          }
          else
          { // iCur = 0
            Prv = Quantity - 1;
            T = Test[0] / (Test[0] - Test[Prv]);
            CV[CQuantity][0] = Contacts.Points[0][0] 
                              + T * (Contacts.Points[Prv][0] - Contacts.Points[0][0]);
            CV[CQuantity][1] = Contacts.Points[0][1] 
                              + T * (Contacts.Points[Prv][1] - Contacts.Points[0][1]);
            CV[CQuantity][2] = Contacts.Points[0][2] 
                              + T * (Contacts.Points[Prv][2] - Contacts.Points[0][2]);
            CV[CQuantity][3] = Contacts.Points[0][3] 
                              + T * (Contacts.Points[Prv][3] - Contacts.Points[0][3]);
            CQuantity++;
          }
        }
        Quantity = CQuantity;
        memcpy( Contacts.Points, CV, CQuantity * sizeof(dVector3) );
      }
      // else polygon fully on positive side of plane, nothing to do    
      Contacts.Count = Quantity;
    }
    else 
      Contacts.Count = 0; // This should not happen, but for safety
  }

  //------------------------------------------------------------------------------
  /**
  */
  dReal PointLineDist( const dVector3 Point, const dVector3 Origin, 
                       const dVector3 Direction, dReal& T )
  {
    dVector3 Diff;
    Diff[0] = Point[0] - Origin[0];
    Diff[1] = Point[1] - Origin[1];
    Diff[2] = Point[2] - Origin[2];
    Diff[3] = Point[3] - Origin[3];

    T = dDOT(Diff, Direction);

    if (T <= REAL(0.0))
    {
      T = REAL(0.0);
    }
    else
    {
      dReal MagSq = dDOT(Direction, Direction);
      if ( T >= MagSq )
      {
        T = REAL(1.0);
        Diff[0] -= Direction[0];
        Diff[1] -= Direction[1];
        Diff[2] -= Direction[2];
        Diff[3] -= Direction[3];
      }
      else
      {
        T /= MagSq;

        Diff[0] -= T * Direction[0];
        Diff[1] -= T * Direction[1];
        Diff[2] -= T * Direction[2];
        Diff[3] -= T * Direction[3];
      }
    }
    return dSqrt(dDOT(Diff, Diff));
  }

  //------------------------------------------------------------------------------
  /**
  */
  bool FindTriBoxIntersection( const dVector3 Tri[3], const dVector4 Planes[6], 
                               LineContactSet& Contacts )
  { 
    Contacts.Count = 3;
    memcpy( Contacts.Points, Tri, 3 * sizeof(dVector3) );
  
    for ( int i = 0; i < 6; i++ )
      ClipConvexPolygonAgainstPlane( Planes[i], Planes[i][3], Contacts );
  
    return Contacts.Count > 0;
  }

} // namespace nOdeUtil

//------------------------------------------------------------------------------
/**
  @brief Collide box with triangle mesh.
*/
int nOdeTriMesh::CollideBoxMesh( dxGeom* triList, dxGeom* boxGeom, int flags,
                                 dContactGeom* contacts, int stride )
{
  nOdeTriMeshShape* shape = (nOdeTriMeshShape*)dGeomGetData( triList );
  nOdeTriMesh* mesh = shape->GetTriMesh();
      
  int retval = 0;
    
  // compute the box
  const dVector3& boxCenter = *(const dVector3*)dGeomGetPosition( boxGeom );
  dVector3 boxExtents;
  dGeomBoxGetLengths( boxGeom, boxExtents );
  boxExtents[0] /= 2;
  boxExtents[1] /= 2;
  boxExtents[2] /= 2;
  
  const dMatrix3& boxRotation = *(const dMatrix3*)dGeomGetRotation( boxGeom );

  // make OBB
  OBB box;
  box.mCenter.x = boxCenter[0];
  box.mCenter.y = boxCenter[1];
  box.mCenter.z = boxCenter[2];

  box.mExtents.x = boxExtents[0];
  box.mExtents.y = boxExtents[1];
  box.mExtents.z = boxExtents[2];

  box.mRot2.m[0][0] = boxRotation[0];
  box.mRot2.m[1][0] = boxRotation[1];
  box.mRot2.m[2][0] = boxRotation[2];

  box.mRot2.m[0][1] = boxRotation[4];
  box.mRot2.m[1][1] = boxRotation[5];
  box.mRot2.m[2][1] = boxRotation[6];

  box.mRot2.m[0][2] = boxRotation[8];
  box.mRot2.m[1][2] = boxRotation[9];
  box.mRot2.m[2][2] = boxRotation[10];
  
  // setup OBB collider
  OBBCollider& collider = nOdeTriMesh::odeServer->opcOBBCollider;
  //VolumeCollider* colliderPtr = (VolumeCollider*)&collider;
  OBBCache& cache = nOdeTriMesh::odeServer->opcOBBCache;
  collider.SetCallback( mesh->collCallback, (udword)mesh );
    
  // convert ODE geom rotation/position to Opcode Matrix4x4
  Matrix4x4 opcMeshMatrix;
  nOdeUtil::GetOpcodeMatrix( triList, &opcMeshMatrix );
  // convert opcode matrix to neb matrix
  matrix44 meshMatrix( &(opcMeshMatrix.m[0][0]) );
  
  // Intersect
  collider.Collide( cache, box, &(mesh->opcModel), 
                    NULL/* box already in world space */, &opcMeshMatrix );
  
  if ( collider.GetContactStatus() )
  {
    // Retrieve data
    int triCount = collider.GetNbTouchedPrimitives();
      
    const int* triangles = (const int*)collider.GetTouchedPrimitives();

    // Decompose transformation
    dVector3 axis[3];
    nOdeUtil::DecomposeMatrix( boxRotation, axis );

    // Compute box planes
    dVector4 planes[6];
    int planeCounter = 0;
    
    for ( dReal dir = REAL(-1.0); dir <= REAL(1.0); dir += REAL(2.0) )
    {
      for ( int side = 0; side < 3; side++ )
      {
        planes[planeCounter][0] = dir * axis[side][0];
        planes[planeCounter][1] = dir * axis[side][1];
        planes[planeCounter][2] = dir * axis[side][2];
        
        dVector3 temp;
        temp[0] = boxCenter[0] - dir * boxExtents[side] * axis[side][0];
        temp[1] = boxCenter[1] - dir * boxExtents[side] * axis[side][1];
        temp[2] = boxCenter[2] - dir * boxExtents[side] * axis[side][2];
        temp[3] = boxCenter[3] - dir * boxExtents[side] * axis[side][3];
        
        planes[planeCounter][3] = dir * dDOT(axis[side], temp);
        
        planeCounter++;
      }
    }

    int outTriCount = 0;
    vector3 v0, v1, v2;
    vector3 tv0, tv1, tv2;
    dVector3 dv[3];
    int i;
    for ( i = 0; i < triCount; i++ )
    {
      // get touched triangle in model space
      mesh->GetTriCoords( triangles[i], v0, v1, v2 );
            
      // transform triangle into world space
      tv0 = meshMatrix * v0;
      tv1 = meshMatrix * v1;
      tv2 = meshMatrix * v2;
      
      dv[0][0] = tv0.x; dv[1][0] = tv1.x; dv[2][0] = tv2.x;
      dv[0][1] = tv0.y; dv[1][1] = tv1.y; dv[2][1] = tv2.y;
      dv[0][2] = tv0.z; dv[1][2] = tv1.z; dv[2][2] = tv2.z;
      dv[0][3] = REAL(0.0); dv[1][3] = REAL(0.0); dv[2][3] = REAL(0.0);
      
      // Apply some magic to find collision points
      LineContactSet InContacts;
      if ( nOdeUtil::FindTriBoxIntersection( dv, planes, InContacts ) )
      {
        // Compute triangle plane
        dVector3 vu;
        vu[0] = dv[1][0] - dv[0][0];
        vu[1] = dv[1][1] - dv[0][1];
        vu[2] = dv[1][2] - dv[0][2];
        vu[3] = REAL(0.0);
        
        dVector3 vv;
        vv[0] = dv[2][0] - dv[0][0];
        vv[1] = dv[2][1] - dv[0][1];
        vv[2] = dv[2][2] - dv[0][2];
        vv[3] = REAL(0.0);

        dVector4 Plane;
        dCROSS(Plane, =, vu, vv);
        Plane[3] = dDOT(Plane, dv[0]);

        // Normalize the plane
        dReal Area = dSqrt(dDOT(Plane, Plane)); // Use this later for epsilons
        Plane[0] /= Area;
        Plane[1] /= Area;
        Plane[2] /= Area;
        Plane[3] /= Area;

        // Compute box vertices
        dVector3 Vertices[8];
        nOdeUtil::ComputeVertices( boxCenter, boxExtents, boxRotation, Vertices );

        // Find the smallest penetration depth of the box and a 
        // plane going through the triangle.
        // This needs to be optimized.
        dReal Depth = dInfinity;
        int j;
        for ( j = 0; j < 8; j++ )
        {
          dReal Behind = dDOT(Plane, Vertices[j]) - Plane[3];

          if ( Behind < REAL(0.00001) )
          { // Should be relative. How?
            Depth = nOdeUtil::dcMIN(Depth, -Behind);
          }
        }
        Depth = nOdeUtil::dcMAX(Depth, REAL(0.0));  // For small inaccuracies

        // Shouldn't we remove some irrelevant points? We do not care for >3 contacts.
        for ( j = 0; j < InContacts.Count; j++ )
        {
          dContactGeom* contact = 
            nOdeUtil::FetchContact( flags, contacts, outTriCount, stride );
          
          contact->pos[0] = InContacts.Points[j][0];
          contact->pos[1] = InContacts.Points[j][1];
          contact->pos[2] = InContacts.Points[j][2];
          contact->pos[3] = InContacts.Points[j][3];

          // Always use triangle's normal. For edge contacts this is not preferable.
          // How do we determine a correct edge-contact normal?
          contact->normal[0] = -Plane[0];
          contact->normal[1] = -Plane[1];
          contact->normal[2] = -Plane[2];
          contact->normal[3] = REAL(0.0);

          // Test all 3 triangle edges to see if we can find a smaller penetration.
          // This is good for sharp edges. Doesnt this interfere with sliding objects?
          // Hopefully not!
          dReal MinDepth = Depth;
          for ( int k = 0; k < 3; k++ )
          {
            const dVector3& Origin = dv[k];
            dVector3 Direction;
            Direction[0] = dv[(k + 1) % 3][0] - Origin[0];
            Direction[1] = dv[(k + 1) % 3][1] - Origin[1];
            Direction[2] = dv[(k + 1) % 3][2] - Origin[2];
            Direction[3] = dv[(k + 1) % 3][3] - Origin[3];
            
            dReal T;  // Maybe use this to adjust contact position? Sounds logical.
            dReal Dist = nOdeUtil::PointLineDist( contact->pos, Origin, Direction, T );

            if (Dist <= MinDepth)
            {
              MinDepth = Dist;

              /*if (MinDepth > REAL(0.0001)){ // Is this any help?
                contact->normal[0] = contact->pos[0] - (Origin[0] + Direction[0] * T);
                contact->normal[1] = contact->pos[1] - (Origin[1] + Direction[1] * T);
                contact->normal[2] = contact->pos[2] - (Origin[2] + Direction[2] * T);
                contact->normal[3] = REAL(0.0);
                dNormalize3(contact->normal);
              }*/
            }
          }
          
          contact->depth = MinDepth;
          contact->g1 = (dxGeom*)1; // WARNING: Large hack. Using g1 as a counter.

          int index;
          for ( index = 0; index < outTriCount; index++ )
          {
            dContactGeom* tempContact = 
              nOdeUtil::FetchContact( flags, contacts, index, stride );
            
            dVector3 Diff;
            Diff[0] = tempContact->pos[0] - contact->pos[0];
            Diff[1] = tempContact->pos[1] - contact->pos[1];
            Diff[2] = tempContact->pos[2] - contact->pos[2];
            Diff[3] = tempContact->pos[3] - contact->pos[3];
            
            dReal DistSq = dDOT(Diff, Diff);
            
            if ( DistSq < 0.001 )
            {
              //BoxRadius * REAL(0.1)){ // Tweak?
              break;
            }
          }
          
          if ( index != outTriCount )
          {
            dContactGeom* tempContact = 
              nOdeUtil::FetchContact( flags, contacts, index, stride );
            
            tempContact->normal[0] += contact->normal[0];
            tempContact->normal[1] += contact->normal[1];
            tempContact->normal[2] += contact->normal[2];
            tempContact->normal[3] += contact->normal[3];

            tempContact->depth += contact->depth;
            contact->g1 = (dxGeom*)(((char*)contact->g1) + 1);
          }
          else 
            outTriCount++;
        }
      }
    }
    
    for ( i = 0; i < outTriCount; i++ )
    { 
      // Now normalize normals
      dContactGeom* contact = nOdeUtil::FetchContact( flags, contacts, i, stride );
      dNormalize3( contact->normal );

      contact->depth /= (int&)contact->g1;  // Hacking again.

      contact->g1 = triList;
      contact->g2 = boxGeom;
    }
    
    retval = outTriCount;
  }
    
  // number of contacts
  return retval;
}
//------------------------------------------------------------------------------
