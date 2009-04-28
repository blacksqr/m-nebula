#ifndef N_COLLISION_MATH_H
#define N_COLLISION_MATH_H
//------------------------------------------------------------------------------
/**
  @class nCollisionMath
  @ingroup NODEPhysicsContribModule
  @brief Encapsulates some functions used by collision code in this package.

  (c) 2003  Vadim Macagon
  
  nCollisionMath is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_TRIANGLE_H
#include "mathlib/triangle.h"
#endif

#ifndef N_SPHERE_H
#include "mathlib/sphere.h"
#endif

#ifndef N_CAPSULE_H
#include "mathlib/capsule.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCollisionMath
#include "kernel/ndefdllclass.h"
//------------------------------------------------------------------------------
class N_PUBLIC nCollisionMath
{
  public:
  
    static bool IntersectTriSphere( const sphere& ball, const triangle& tri, 
                                    float* dist = 0, float* u = 0, float* v = 0 );
    static bool IntersectTriCapsule( const capsule& cap, const triangle& tri,
                                   float* dist = 0, float* t = 0, 
                                   float* u = 0, float* v = 0 );
    static float SqrDistance( const line3& seg, const triangle& tri,
                              float* pfSegP = 0, float* pfTriP0 = 0, 
                              float* pfTriP1 = 0 );
    static float SqrDistance( const vector3& p, const triangle& tri,
                              float* pfSParam = 0, float* pfTParam = 0 );
    static float SqrDistance( const vector3& p, const line3& seg );
    static float SqrDistance( const line3& seg1, const line3& seg2,
                              float* pfSegP0 = 0, float* pfSegP1 = 0 );
};

//------------------------------------------------------------------------------
/**
  @brief Check for intersection between triangle and sphere.
  
  @param ball [in]
  @param tri  [in]
  @param dist [out] Shortest distance squared between the triangle and the 
                    sphere center.
  @param u    [out] Barycentric coord on triangle.
  @param v    [out] Barycentric coord on triangle.
  @return True if intersection exists.
  
  The third Barycentric coord is implicit, ie. w = 1.0 - u - v
  The Barycentric coords give the location of the point on the triangle
  closest to the sphere (where the distance between the two shapes
  is the shortest).
*/
inline
bool nCollisionMath::IntersectTriSphere( const sphere& ball, const triangle& tri,
                                         float* dist, float* u, float* v )
{
  float sqrDist = nCollisionMath::SqrDistance( ball.p, tri, u, v );

  if ( dist )
    *dist = sqrDist;
  
  if ( sqrDist <= ball.r * ball.r )
    return true;
    
  return false;
}

//------------------------------------------------------------------------------
/**
  @brief Check for intersection between triangle and capsule.
  
  @param cap
  @param tri
  @param dist [out] Shortest distance squared between the triangle and 
                    the capsule segment (central axis).
  @param t    [out] t value of point on segment that's the shortest distance 
                    away from the triangle, the coordinates of this point 
                    can be found by (cap.seg.end - cap.seg.start) * t,
                    or cap.seg.ipol(t).
  @param u    [out] Barycentric coord on triangle.
  @param v    [out] Barycentric coord on triangle.
  @return True if intersection exists.
  
  The third Barycentric coord is implicit, ie. w = 1.0 - u - v
  The Barycentric coords give the location of the point on the triangle
  closest to the capsule (where the distance between the two shapes
  is the shortest).
*/
inline
bool nCollisionMath::IntersectTriCapsule( const capsule& cap, const triangle& tri,
                                          float* dist, float* t, float* u, float* v )
{
  float sqrDist = nCollisionMath::SqrDistance( cap.seg, tri, t, u, v );
  
  if ( dist )
    *dist = sqrDist;
    
  if ( sqrDist <= cap.r * cap.r )
    return true;
     
  return false;
}

//------------------------------------------------------------------------------
#endif // N_COLLISION_MATH_H
