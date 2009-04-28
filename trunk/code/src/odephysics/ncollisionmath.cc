#define N_IMPLEMENTS nCollisionMath
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//
//  nCollisionMath is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/ncollisionmath.h"
#include <limits>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_DEBUG_H
#include "kernel/ndebug.h"
#endif

//------------------------------------------------------------------------------
/**
  @brief Finds the shortest distance squared between a line segment and 
         a triangle.
         
  @param pfSegP   t value for the line segment where the shortest distance between
                  the segment and the triangle occurs.
                  So the point along the segment that is the shortest distance
                  away from the triangle can be obtained by (seg.end - seg.start) * t.
  @param pfTriP0  Barycentric coordinate of triangle at point closest to seg (u)
  @param pfTriP1  Barycentric coordinate of triangle at point closest to seg (v)
  @return Shortest distance squared.
  
  The third Barycentric coordinate is implicit, ie. w = 1.0 - u - v
         
  Taken from:
  Magic Software, Inc.
  http://www.magic-software.com
*/
float nCollisionMath::SqrDistance( const line3& seg, const triangle& tri,
                                   float* pfSegP, float* pfTriP0, float* pfTriP1 )
{
  const float gs_fTolerance = 1e-06f;
  vector3 kDiff = tri.b - seg.b;
  float fA00 = seg.m.len_squared();
  float fA01 = -seg.m % tri.e0;
  float fA02 = -seg.m % tri.e1;
  float fA11 = tri.e0.len_squared();
  float fA12 = tri.e0 % tri.e1;
  float fA22 = tri.e1 % tri.e1;
  float fB0  = -kDiff % seg.m;
  float fB1  = kDiff % tri.e0;
  float fB2  = kDiff % tri.e1;

  line3 kTriSeg;
  vector3 kPt;
  float fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

  // Set up for a relative error test on the angle between ray direction
  // and triangle normal to determine parallel/nonparallel status.
  vector3 kN = tri.e0 * tri.e1;
  float fNSqrLen = kN.len_squared();
  float fDot = seg.m % kN;
  bool bNotParallel = (fDot*fDot >= gs_fTolerance*fA00*fNSqrLen);

    if ( bNotParallel )
    {
        float fCof00 = fA11*fA22-fA12*fA12;
        float fCof01 = fA02*fA12-fA01*fA22;
        float fCof02 = fA01*fA12-fA02*fA11;
        float fCof11 = fA00*fA22-fA02*fA02;
        float fCof12 = fA02*fA01-fA00*fA12;
        float fCof22 = fA00*fA11-fA01*fA01;
        float fInvDet = 1.0f/(fA00*fCof00+fA01*fCof01+fA02*fCof02);
        float fRhs0 = -fB0*fInvDet;
        float fRhs1 = -fB1*fInvDet;
        float fRhs2 = -fB2*fInvDet;

        fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
        fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
        fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

        if ( fR < 0.0f )
        {
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4m
                    {
                        // min on face s=0 or t=0 or r=0
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e1;
                        fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e0;
                        fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fS0);
                        fT0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                        fSqrDist0 = SqrDistance(seg.b,tri,&fS0,&fT0);
                        fR0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                    else  // region 3m
                    {
                        // min on face s=0 or r=0
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e1;
                        fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        fSqrDist0 = SqrDistance(seg.b,tri,&fS0,&fT0);
                        fR0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                }
                else if ( fT < 0.0f )  // region 5m
                {
                    // min on face t=0 or r=0
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    fSqrDist0 = SqrDistance(seg.b,tri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 0m
                {
                    // min on face r=0
                    fSqrDist = SqrDistance(seg.b,tri,&fS,&fT);
                    fR = 0.0f;
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2m
                {
                    // min on face s=0 or s+t=1 or r=0
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e1;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                    fS = 0.0f;
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(seg.b,tri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT < 0.0f )  // region 6m
                {
                    // min on face t=0 or s+t=1 or r=0
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(seg.b,tri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 1m
                {
                    // min on face s+t=1 or r=0
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                    fS = 1.0f-fT;
                    fSqrDist0 = SqrDistance(seg.b,tri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
        }
        else if ( fR <= 1.0f )
        {
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4
                    {
                        // min on face s=0 or t=0
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e1;
                        fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e0;
                        fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fS0);
                        fT0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                    else  // region 3
                    {
                        // min on face s=0
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e1;
                        fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                    }
                }
                else if ( fT < 0.0f )  // region 5
                {
                    // min on face t=0
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                }
                else  // region 0
                {
                    // global minimum is interior, done
                    fSqrDist = 0.0f;
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2
                {
                    // min on face s=0 or s+t=1
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e1;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                    fS = 0.0f;
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT < 0.0f )  // region 6
                {
                    // min on face t=0 or s+t=1
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 1
                {
                    // min on face s+t=1
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                    fS = 1.0f-fT;
                }
            }
        }
        else  // fR > 1
        {
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4p
                    {
                        // min on face s=0 or t=0 or r=1
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e1;
                        fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e0;
                        fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fS0);
                        fT0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                        kPt = seg.b + seg.m;
                        fSqrDist0 = SqrDistance(kPt,tri,&fS0,&fT0);
                        fR0 = 1.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                    else  // region 3p
                    {
                        // min on face s=0 or r=1
                        kTriSeg.b = tri.b;
                        kTriSeg.m = tri.e1;
                        fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kPt = seg.b + seg.m;
                        fSqrDist0 = SqrDistance(kPt,tri,&fS0,&fT0);
                        fR0 = 1.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                }
                else if ( fT < 0.0f )  // region 5p
                {
                    // min on face t=0 or r=1
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kPt = seg.b + seg.m;
                    fSqrDist0 = SqrDistance(kPt,tri,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 0p
                {
                    // min face on r=1
                    kPt = seg.b + seg.m;
                    fSqrDist = SqrDistance(kPt,tri,&fS,&fT);
                    fR = 1.0f;
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2p
                {
                    // min on face s=0 or s+t=1 or r=1
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e1;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                    fS = 0.0f;
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    kPt = seg.b + seg.m;
                    fSqrDist0 = SqrDistance(kPt,tri,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT < 0.0f )  // region 6p
                {
                    // min on face t=0 or s+t=1 or r=1
                    kTriSeg.b = tri.b;
                    kTriSeg.m = tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    kPt = seg.b + seg.m;
                    fSqrDist0 = SqrDistance(kPt,tri,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 1p
                {
                    // min on face s+t=1 or r=1
                    kTriSeg.b = tri.b + tri.e0;
                    kTriSeg.m = tri.e1 - tri.e0;
                    fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fT);
                    fS = 1.0f-fT;
                    kPt = seg.b + seg.m;
                    fSqrDist0 = SqrDistance(kPt,tri,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
        }
    }
    else
    {
        // segment and triangle are parallel
        kTriSeg.b = tri.b;
        kTriSeg.m = tri.e0;
        fSqrDist = SqrDistance(seg,kTriSeg,&fR,&fS);
        fT = 0.0f;

        kTriSeg.m = tri.e1;
        fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fT0);
        fS0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kTriSeg.b = tri.b + tri.e0;
        kTriSeg.m = tri.e1 - tri.e0;
        fSqrDist0 = SqrDistance(seg,kTriSeg,&fR0,&fT0);
        fS0 = 1.0f-fT0;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        fSqrDist0 = SqrDistance(seg.b,tri,&fS0,&fT0);
        fR0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kPt = seg.b + seg.m;
        fSqrDist0 = SqrDistance(kPt,tri,&fS0,&fT0);
        fR0 = 1.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }
    }

    if ( pfSegP )
        *pfSegP = fR;

    if ( pfTriP0 )
        *pfTriP0 = fS;

    if ( pfTriP1 )
        *pfTriP1 = fT;

    n_assert( fSqrDist >= 0 );
    return fSqrDist;
}

//------------------------------------------------------------------------------
/**
  @brief Finds the shortest distance squared between a point and a triangle.
  
  @param pfSParam  Barycentric coordinate of triangle at point closest to p (u)
  @param pfTParam  Barycentric coordinate of triangle at point closest to p (v)
  @return Shortest distance squared.
  
  The third Barycentric coordinate is implicit, ie. w = 1.0 - u - v
  
  Taken from:
  Magic Software, Inc.
  http://www.magic-software.com
*/
float nCollisionMath::SqrDistance( const vector3& p, const triangle& tri,
                                   float* pfSParam, float* pfTParam )
{
  vector3 kDiff = tri.b - p;
  float fA00 = tri.e0.len_squared();
  float fA01 = tri.e0 % tri.e1;
  float fA11 = tri.e1.len_squared();
  float fB0 = kDiff % tri.e0;
  float fB1 = kDiff % tri.e1;
  float fC = kDiff.len_squared();
  float fDet = (float)fabs(fA00*fA11-fA01*fA01);
  float fS = fA01*fB1-fA11*fB0;
  float fT = fA01*fB0-fA00*fB1;
  float fSqrDist;

  if ( fS + fT <= fDet )
  {
    if ( fS < 0.0f )
    {
      if ( fT < 0.0f )  // region 4
      {
        if ( fB0 < 0.0f )
        {
          fT = 0.0f;
          if ( -fB0 >= fA00 )
          {
            fS = 1.0f;
            fSqrDist = fA00+2.0f*fB0+fC;
          }
          else
          {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
        else
        {
          fS = 0.0f;
          if ( fB1 >= 0.0f )
          {
            fT = 0.0f;
            fSqrDist = fC;
          }
          else if ( -fB1 >= fA11 )
          {
            fT = 1.0f;
            fSqrDist = fA11+2.0f*fB1+fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
      else  // region 3
      {
        fS = 0.0f;
        if ( fB1 >= 0.0f )
        {
          fT = 0.0f;
          fSqrDist = fC;
        }
        else if ( -fB1 >= fA11 )
        {
          fT = 1.0f;
          fSqrDist = fA11+2.0f*fB1+fC;
        }
        else
        {
          fT = -fB1/fA11;
          fSqrDist = fB1*fT+fC;
        }
      }
    }
    else if ( fT < 0.0f )  // region 5
    {
      fT = 0.0f;
      if ( fB0 >= 0.0f )
      {
        fS = 0.0f;
        fSqrDist = fC;
      }
      else if ( -fB0 >= fA00 )
      {
        fS = 1.0f;
        fSqrDist = fA00+2.0f*fB0+fC;
      }
      else
      {
        fS = -fB0/fA00;
        fSqrDist = fB0*fS+fC;
      }
    }
    else  // region 0
    {
      // minimum at interior point
      if ( fDet == 0.0f )
      {
        fS = 0.0f;
        fT = 0.0f;
        fSqrDist = std::numeric_limits<float>::max();
      } 
      else
      {
        float fInvDet = 1.0f/fDet;
        fS *= fInvDet;
        fT *= fInvDet;
        fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                    fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
      }
    }
  }
  else
  {
    float fTmp0, fTmp1, fNumer, fDenom;

    if ( fS < 0.0f )  // region 2
    {
      fTmp0 = fA01 + fB0;
      fTmp1 = fA11 + fB1;
      if ( fTmp1 > fTmp0 )
      {
        fNumer = fTmp1 - fTmp0;
        fDenom = fA00-2.0f*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fS = 1.0f;
          fT = 0.0f;
          fSqrDist = fA00+2.0f*fB0+fC;
        }
        else
        {
          fS = fNumer/fDenom;
          fT = 1.0f - fS;
          fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
              fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
        }
      }
      else
      {
        fS = 0.0f;
        if ( fTmp1 <= 0.0f )
        {
          fT = 1.0f;
          fSqrDist = fA11+2.0f*fB1+fC;
        }
        else if ( fB1 >= 0.0f )
        {
          fT = 0.0f;
          fSqrDist = fC;
        }
        else
        {
          fT = -fB1/fA11;
          fSqrDist = fB1*fT+fC;
        }
      }
    }
    else if ( fT < 0.0f )  // region 6
    {
      fTmp0 = fA01 + fB1;
      fTmp1 = fA00 + fB0;
      if ( fTmp1 > fTmp0 )
      {
        fNumer = fTmp1 - fTmp0;
        fDenom = fA00-2.0f*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fT = 1.0f;
          fS = 0.0f;
          fSqrDist = fA11+2.0f*fB1+fC;
        }
        else
        {
          fT = fNumer/fDenom;
          fS = 1.0f - fT;
          fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
              fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
        }
      }
      else
      {
        fT = 0.0f;
        if ( fTmp1 <= 0.0f )
        {
          fS = 1.0f;
          fSqrDist = fA00+2.0f*fB0+fC;
        }
        else if ( fB0 >= 0.0f )
        {
          fS = 0.0f;
          fSqrDist = fC;
        }
        else
        {
          fS = -fB0/fA00;
          fSqrDist = fB0*fS+fC;
        }
      }
    }
    else  // region 1
    {
      fNumer = fA11 + fB1 - fA01 - fB0;
      if ( fNumer <= 0.0f )
      {
        fS = 0.0f;
        fT = 1.0f;
        fSqrDist = fA11+2.0f*fB1+fC;
      }
      else
      {
        fDenom = fA00-2.0f*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fS = 1.0f;
          fT = 0.0f;
          fSqrDist = fA00+2.0f*fB0+fC;
        }
        else
        {
          fS = fNumer/fDenom;
          fT = 1.0f - fS;
          fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
              fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
        }
      }
    }
  }

  if ( pfSParam )
      *pfSParam = (float)fS;

  if ( pfTParam )
      *pfTParam = (float)fT;

  //return (float)fabs(fSqrDist);
  n_assert( fSqrDist >= 0 );
  return fSqrDist;
}

//------------------------------------------------------------------------------
/**
  @brief Finds the shortest distance squared between a point and a line segment.
  @return Shortest distance squared.
*/
float nCollisionMath::SqrDistance( const vector3& p, const line3& seg )
{
  vector3 diff = p - seg.start();
  float t = diff % seg.m;
  
  if ( t > 0.0f )
  {
    float sqrLen = seg.m.len_squared();
    if ( t >= sqrLen )
    {
      // t = 1.0f;
      diff -= seg.m;
    }
    else
    {
      t /= sqrLen;
      diff -= seg.m * t;
    }
  }

  return diff.len_squared();
}

//------------------------------------------------------------------------------
/**
  @brief Finds the shortest distance squared between two line segments.
  @param pfSegP0  t value for seg1 where the shortest distance between
                  the segments exists.
  @param pfSegP0  t value for seg2 where the shortest distance between
                  the segments exists.
  @return Shortest distance squared.
  
  Taken from:
  Magic Software, Inc.
  http://www.magic-software.com
*/
float nCollisionMath::SqrDistance( const line3& seg1, const line3& seg2,
                                   float* pfSegP0, float* pfSegP1 )
{
  const float gs_fTolerance = 1e-05f;
  vector3 kDiff = seg1.start() - seg2.start();
  float fA00 = seg1.m.len_squared();
  float fA01 = -seg1.m % seg2.m;
  float fA11 = seg2.m.len_squared();
  float fB0 = kDiff % seg1.m;
  float fC = kDiff.len_squared();
  float fDet = (float)fabs(fA00*fA11-fA01*fA01);
  float fB1, fS, fT, fSqrDist, fTmp;

    if ( fDet >= gs_fTolerance )
    {
        // line segments are not parallel
        fB1 = -kDiff % seg2.m;
        fS = fA01*fB1-fA11*fB0;
        fT = fA01*fB0-fA00*fB1;
        
        if ( fS >= 0.0f )
        {
            if ( fS <= fDet )
            {
                if ( fT >= 0.0f )
                {
                    if ( fT <= fDet )  // region 0 (interior)
                    {
                        // minimum at two interior points of 3D lines
                        float fInvDet = 1.0f/fDet;
                        fS *= fInvDet;
                        fT *= fInvDet;
                        fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                            fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                    }
                    else  // region 3 (side)
                    {
                        fT = 1.0f;
                        fTmp = fA01+fB0;
                        if ( fTmp >= 0.0f )
                        {
                            fS = 0.0f;
                            fSqrDist = fA11+2.0f*fB1+fC;
                        }
                        else if ( -fTmp >= fA00 )
                        {
                            fS = 1.0f;
                            fSqrDist = fA00+fA11+fC+2.0f*(fB1+fTmp);
                        }
                        else
                        {
                            fS = -fTmp/fA00;
                            fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
                        }
                    }
                }
                else  // region 7 (side)
                {
                    fT = 0.0f;
                    if ( fB0 >= 0.0f )
                    {
                        fS = 0.0f;
                        fSqrDist = fC;
                    }
                    else if ( -fB0 >= fA00 )
                    {
                        fS = 1.0f;
                        fSqrDist = fA00+2.0f*fB0+fC;
                    }
                    else
                    {
                        fS = -fB0/fA00;
                        fSqrDist = fB0*fS+fC;
                    }
                }
            }
            else
            {
                if ( fT >= 0.0 )
                {
                    if ( fT <= fDet )  // region 1 (side)
                    {
                        fS = 1.0f;
                        fTmp = fA01+fB1;
                        if ( fTmp >= 0.0f )
                        {
                            fT = 0.0f;
                            fSqrDist = fA00+2.0f*fB0+fC;
                        }
                        else if ( -fTmp >= fA11 )
                        {
                            fT = 1.0f;
                            fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
                        }
                        else
                        {
                            fT = -fTmp/fA11;
                            fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
                        }
                    }
                    else  // region 2 (corner)
                    {
                        fTmp = fA01+fB0;
                        if ( -fTmp <= fA00 )
                        {
                            fT = 1.0f;
                            if ( fTmp >= 0.0f )
                            {
                                fS = 0.0f;
                                fSqrDist = fA11+2.0f*fB1+fC;
                            }
                            else
                            {
                                 fS = -fTmp/fA00;
                                 fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
                            }
                        }
                        else
                        {
                            fS = 1.0f;
                            fTmp = fA01+fB1;
                            if ( fTmp >= 0.0f )
                            {
                                fT = 0.0f;
                                fSqrDist = fA00+2.0f*fB0+fC;
                            }
                            else if ( -fTmp >= fA11 )
                            {
                                fT = 1.0f;
                                fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
                            }
                            else
                            {
                                fT = -fTmp/fA11;
                                fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
                            }
                        }
                    }
                }
                else  // region 8 (corner)
                {
                    if ( -fB0 < fA00 )
                    {
                        fT = 0.0f;
                        if ( fB0 >= 0.0f )
                        {
                            fS = 0.0f;
                            fSqrDist = fC;
                        }
                        else
                        {
                            fS = -fB0/fA00;
                            fSqrDist = fB0*fS+fC;
                        }
                    }
                    else
                    {
                        fS = 1.0f;
                        fTmp = fA01+fB1;
                        if ( fTmp >= 0.0f )
                        {
                            fT = 0.0f;
                            fSqrDist = fA00+2.0f*fB0+fC;
                        }
                        else if ( -fTmp >= fA11 )
                        {
                            fT = 1.0f;
                            fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
                        }
                        else
                        {
                            fT = -fTmp/fA11;
                            fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
                        }
                    }
                }
            }
        }
        else 
        {
            if ( fT >= 0.0f )
            {
                if ( fT <= fDet )  // region 5 (side)
                {
                    fS = 0.0f;
                    if ( fB1 >= 0.0f )
                    {
                        fT = 0.0f;
                        fSqrDist = fC;
                    }
                    else if ( -fB1 >= fA11 )
                    {
                        fT = 1.0f;
                        fSqrDist = fA11+2.0f*fB1+fC;
                    }
                    else
                    {
                        fT = -fB1/fA11;
                        fSqrDist = fB1*fT+fC;
                    }
                }
                else  // region 4 (corner)
                {
                    fTmp = fA01+fB0;
                    if ( fTmp < 0.0f )
                    {
                        fT = 1.0f;
                        if ( -fTmp >= fA00 )
                        {
                            fS = 1.0f;
                            fSqrDist = fA00+fA11+fC+2.0f*(fB1+fTmp);
                        }
                        else
                        {
                            fS = -fTmp/fA00;
                            fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
                        }
                    }
                    else
                    {
                        fS = 0.0f;
                        if ( fB1 >= 0.0f )
                        {
                            fT = 0.0f;
                            fSqrDist = fC;
                        }
                        else if ( -fB1 >= fA11 )
                        {
                            fT = 1.0f;
                            fSqrDist = fA11+2.0f*fB1+fC;
                        }
                        else
                        {
                            fT = -fB1/fA11;
                            fSqrDist = fB1*fT+fC;
                        }
                    }
                }
            }
            else   // region 6 (corner)
            {
                if ( fB0 < 0.0f )
                {
                    fT = 0.0f;
                    if ( -fB0 >= fA00 )
                    {
                        fS = 1.0f;
                        fSqrDist = fA00+2.0f*fB0+fC;
                    }
                    else
                    {
                        fS = -fB0/fA00;
                        fSqrDist = fB0*fS+fC;
                    }
                }
                else
                {
                    fS = 0.0f;
                    if ( fB1 >= 0.0f )
                    {
                        fT = 0.0f;
                        fSqrDist = fC;
                    }
                    else if ( -fB1 >= fA11 )
                    {
                        fT = 1.0f;
                        fSqrDist = fA11+2.0f*fB1+fC;
                    }
                    else
                    {
                        fT = -fB1/fA11;
                        fSqrDist = fB1*fT+fC;
                    }
                }
            }
        }
    }
    else
    {
        // line segments are parallel
        if ( fA01 > 0.0f )
        {
            // direction vectors form an obtuse angle
            if ( fB0 >= 0.0f )
            {
                fS = 0.0f;
                fT = 0.0f;
                fSqrDist = fC;
            }
            else if ( -fB0 <= fA00 )
            {
                fS = -fB0/fA00;
                fT = 0.0f;
                fSqrDist = fB0*fS+fC;
            }
            else
            {
                fB1 = -kDiff % seg2.m;
                fS = 1.0f;
                fTmp = fA00+fB0;
                if ( -fTmp >= fA01 )
                {
                    fT = 1.0f;
                    fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
                }
                else
                {
                    fT = -fTmp/fA01;
                    fSqrDist = fA00+2.0f*fB0+fC+fT*(fA11*fT+2.0f*(fA01+fB1));
                }
            }
        }
        else
        {
            // direction vectors form an acute angle
            if ( -fB0 >= fA00 )
            {
                fS = 1.0f;
                fT = 0.0f;
                fSqrDist = fA00+2.0f*fB0+fC;
            }
            else if ( fB0 <= 0.0f )
            {
                fS = -fB0/fA00;
                fT = 0.0f;
                fSqrDist = fB0*fS+fC;
            }
            else
            {
                fB1 = -kDiff % seg2.m;
                fS = 0.0f;
                if ( fB0 >= -fA01 )
                {
                    fT = 1.0f;
                    fSqrDist = fA11+2.0f*fB1+fC;
                }
                else
                {
                    fT = -fB0/fA01;
                    fSqrDist = fC+fT*(2.0f*fB1+fA11*fT);
                }
            }
        }
    }
    
    if ( pfSegP0 )
        *pfSegP0 = fS;

    if ( pfSegP1 )
        *pfSegP1 = fT;
    
    //return (float)fabs(fSqrDist);
  n_assert( fSqrDist >= 0 );
  return fSqrDist;
}
//------------------------------------------------------------------------------
