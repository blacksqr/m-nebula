#ifndef N_t_triangle_n_H
#define N_t_triangle_n_H
//-------------------------------------------------------------------
/**
@class t_triangle_n
@brief defines a t_triangle_n in 3d space
*/
//-------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

#ifndef N_PLANE_H
#include "mathlib/plane.h"
#endif

#include <math.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
/**
	t_triangle_n points are tri(s,t)=b + s*v[e0] + t*v[e1] where
	0<=s<=1, 0<=t<=1 and 0<=s+t<=1

	created 8-Jan-2009 Insightec, Inc -by Ilya Kliot
*/
template<template<class> class vtype, typename ntype> 
class t_triangle_n 
{
public:
	typedef vtype<ntype>			t_vector;
	typedef t_line_n<vtype, ntype>  t_line;
	typedef t_ray_n<vtype, ntype>	t_ray;
	typedef t_segment_n<vtype, ntype>  t_segment;
	typedef t_minsegment_n<vtype, ntype>  t_minsegment;
	typedef t_plane<ntype>			   _t_plane;	
public:
	enum {
		a = 0,
		b = 1,
		c = 2,
		e0 = 3,
		e1 = 4
	};
	t_vector v[5]; ///< coordinates			

    t_triangle_n() {}
	t_triangle_n(const t_vector& v0, const t_vector& v1, const t_vector& v2){ this->set(v0, v1, v2);}
    t_triangle_n(const t_triangle_n& t) {memcpy(v, t.v, sizeof(t_vector)*5);}

    void set(const t_vector& v0, const t_vector& v1, const t_vector& v2) 
	{
		v[a] = v0;
		v[b] = v1;
		v[c] = v2;
		v[e0] = v1 - v0;
		v[e1] = v2 - v0;
    }

	//--- get one the sides ---------------------------------------------
	t_minsegment side(int i) const 
	{	
		switch (i)
        {
        case 0: 
			return t_minsegment(this->v[a], this->v[b]);	//line3(va(), vb());			
        case 1: 
			return t_minsegment(this->v[c], this->v[b]); //line3(vb(), vc());
        case 2: 
			return t_minsegment(this->v[a], this->v[c]); //line3(vc(), va());
        default: 
			return t_minsegment();
        }		
	}

    ///--- get the face normal of the t_triangle_n ---------------------------------
    t_vector normal(bool normalize = true) const 
	{
        return t_vector(v[e0]*v[e1], normalize);
    }

    ///--- get the midpoint (center of gravity) of the t_triangle_n ----------------
    t_vector midpoint(void) const 
	{
        const ntype oneThird = NT_ONE / (ntype)3;
        return v[a] + ((v[e0] + v[e1]) * oneThird);
    }

    ///--- get the plane of the t_triangle_n ---------------------------------------
    plane getplane(void) const { return plane(v[a], v[b], v[c]); }

    ///--- get one the edge points ---------------------------------------------
    const t_vector& point(int i) const { return v[i]; }

	//------------------------------------------------------------------------------
	/**
		check if line has intersection with triangle (bfc - back face culled version)
		returns true and time of intersection if success and false if triangle is 
		a backface or line is parallel to triangel plane
	*/	
	bool intersect_bfc(const t_line_n<vtype, ntype>& l, ntype& t) const 
	{		
		// find vectors for two edges sharing vert0 
		// edge1 - v[e0]
		// edge2 - v[e1]
		
		// begin calculating determinant - also used to calculate U parameter 
		t_vector pvec(l.dir() * v[e1]);		

		// if determinant is near zero, ray lies in plane of t_triangle_n 
		ntype det = v[e0] % pvec;		

		if (det < N_TINY)
			return false;
		
		// calculate distance from vert0 to ray origin 
		t_vector tvec(l.va() - v[a]);
		
		// calculate U parameter and test bounds 
		ntype u = tvec % pvec;
		if (u < NT_ZERO || u > det)
			return false;
		
		// prepare to test V parameter 
		t_vector qvec(tvec * this->v[e0]);
		
		// calculate V parameter and test bounds 
		ntype v = l.dir() % qvec;
		if (v < NT_ZERO || u + v > det)
			return false;

		// calculate t, scale parameters, ray intersects t_triangle_n 
		t = this->v[e1] % qvec;		
		t /= det;
		return true;
	}

	//------------------------------------------------------------------------------
	/**
		bfc - (back face culled version)
		check if ray has intersection with triangle (back face culled version)
		returns true and time of intersection if success and false if triangle is 
		a back face or ray is parallel to triangle plane
		created 8-Mar-2009 Insightec, Inc -by Ilya Kliot
	*/
    bool intersect_bfc(const t_ray& l, ntype& t) const 
	{
        bool isec = this->intersect_bfc((t_line&)l, t);
        return (isec && t >= NT_ZERO);
    }

	//------------------------------------------------------------------------------
	/**
		bfc - (back face culled version)
		get intersecting t of segment with triangle
		return false if segment is parallel to triangle plane
		or has no intersection
		created 8-Mar-2009 Insightec, Inc -by Ilya Kliot
	*/
    bool intersect_bfc(const t_segment& l, ntype& t) const 
	{
        bool isec = this->intersect_bfc((t_line&)l, t);
        return (isec && n_in1(t));
    }


	bool intersect(const t_line& l, ntype& t) const 
	{
		// find vectors for two edges sharing vert0 
		// edge1 - v[e0]
		// edge2 - v[e1]		
		// begin calculating determinant - also used to calculate U parameter 
		t_vector pvec(l.dir() * v[e1]);		

		// if determinant is near zero, ray lies in plane of t_triangle_n 
		ntype det = v[e0] % pvec;
		// if determinant < 0 or ~0 back face or parallel case
		if (n_eqz(det))
			return false;

		ntype inv_det = NT_ONE / det; 
		
		// calculate distance from vert0 to ray origin 
		t_vector tvec(l.va() - v[a]);
		
		// calculate U parameter and test bounds 
		ntype u = tvec % pvec * inv_det;
		if (u < NT_ZERO || u > NT_ONE)
		//if (u < -N_TINY || u > NT_ONE + N_TINY)  // [Gilad 23/8/07] Don't miss intersections because of calculation accuracy
			return false;

		// prepare to test W parameter 
		t_vector qvec(tvec * v[e0]);

		/// calculate V parameter and test bounds 
		ntype w = l.dir()%qvec * inv_det;
		if (w < NT_ZERO || u + w > NT_ONE)
		//if (v < -N_TINY || u + v > NT_ONE+N_TINY) // [Gilad 23/8/07] Don't miss intersections because of calculation accuracy
			return false;

		/// calculate t, ray intersects t_triangle_n 
		t = v[e1] % qvec * inv_det;
		return true;
	}

	//------------------------------------------------------------------------------
	/**		
		check if ray has intersection with triangle 
		returns true and time of intersection if success and false if triangle is 
		a back face or ray is parallel to triangle plane
		created 8-Mar-2009 Insightec, Inc -by Ilya Kliot
	*/
    bool intersect(const t_ray& l, ntype& t) const 
	{
        bool isec = this->intersect((t_line&)l, t);
        return (isec && t >= NT_ZERO);
    }

	//------------------------------------------------------------------------------
	/**		
		get intersecting t of segment with triangle
		return false if segment is parallel to triangle plane
		or has no intersection
		created 8-Mar-2009 Insightec, Inc -by Ilya Kliot
	*/
    bool intersect(const t_segment& l, ntype& t) const 
	{
        bool isec = this->intersect((t_line&)l, t);
        return (isec && n_in1(t));
    }    

	/// returns 0 if no cut, -1 if all 3 points are on the plane, 1 if cut exists and saves cut in l
	int intersect(const _t_plane& p, t_segment& l) const 
	{	
		int a_side = p.side(this->v[a]);

		if ( a_side &&			// not on plane
			(a_side == p.side(this->v[b])) &&  // same side as va
			(a_side == p.side(this->v[c]))) // same side as va					
			return 0;	

		t_minsegment lab(this->side(0));
		t_minsegment lbc(this->side(1));
		t_minsegment lca(this->side(2));/*
		float tol = (float)N_TOL;
		bool aonp = p.abs_distance(this->v[a]) < tol;
		bool bonp = p.abs_distance(this->v[b]) < tol;
		bool conp = p.abs_distance(this->v[c]) < tol;
		if (aonp && bonp)
		{
			l = lab;
			return 1;
		} 
		else if (conp && bonp)
		{
			l = lbc;
			return 1;
		}
		else if (aonp && conp)
		{
			l = lca;
			return 1;
		} else if (aonp || bonp || conp)
			return 0;
*/
		
		
		float it = 0.0f;
		long isec = 0;

		t_vector vab;
		t_vector vbc;
		t_vector vca;

		bool bab = p.intersect(lab, it);
		if (bab)  
		{
			vab = lab.ipol(it);
			isec++;
		}

		bool bbc = p.intersect(lbc, it);
		if (bbc) 
		{
			vbc = lbc.ipol(it);
			isec++;
		}
		
		bool bca = p.intersect(lca, it);
		if (bca) 
		{
			vca = lca.ipol(it);
			isec++;
		}
		
		if (isec == 3) {
			if (vab == vbc) {
				bbc = false;
				isec--;
			} else if (vab == vca) {
				bca = false;
				isec--;
			} else if (vbc == vca) {
				bca = false;
				isec--;
			}
		}
		
		if (isec > 1) {		
			if (isec == 3) {
				return -1;
			} else if (bab && bbc) {			
				l.set(vab, vbc);
				return 1;
			} else if (bab && bca) {			
				l.set(vab, vca);
				return 1;
			} else if (bca && bbc) {			
				l.set(vca, vbc);
				return 1;
			}		
		}	
		return 0;
	}
	/**
		returns 0 if no cut, 
		-1 if triangle on a paralell plane, 
		-2 if all 3 points are on the plane, 
		1 if cut exists and saves cut in l
	*/
    int intersect(const t_triangle_n& t, t_segment& l) const {
        t_vector n1 = normal();
        t_vector n2 = t.normal();
		
		_t_plane p(t.getplane());

		int a_side = p.side(this->v[a]);

		if (a_side && 
			(a_side == p.side(this->v[b])) && 
			(a_side == p.side(this->v[c])))
			return 0;
       
        if (n_eq(n_abs(n1%n2), 1.0f)) { // coplanar
            ntype dist = getplane().distance(t.v[b]);
            if (dist < N_TINY) { // same plane
				return -2;
            }
            return -1;
        } else {
            int i, num = 0;
            ntype tm;
            t_segment l1, l2;
            t_triangle_n v[6];

            for (i = 0; i < 3; i++) {
                t_segment ls(t.side(i));
                if (intersect_both_sides_seg_with_ends(ls, tm)) {
                    v[num] = ls.ipol(tm);                    
                    num++;
                }
            }
            
            for (i = 0; i < 3; i++) {
                t_segment ls(side(i));
                if (t.intersect_both_sides_seg_with_ends(ls, tm)) {
                    v[num] = ls.ipol(tm);
                    num++;                    
                }
            }
            
			if (num == 2) {
                l.set(v[0], v[1]);
                return 1;
			} else if (num > 2) {				
				int maxi = 1;
				ntype maxd = NT_ZERO;
				t_vector& p0 = v[0];

				for (i = 1; i < num; i++) {
					ntype d = (p0-v[i]).len();
					if (d > maxd) {
						maxd = d;
						maxi = i;
					}
				} 

				l.set(p0, v[maxi]);
				return 1;
            }
        }
		return 0;		
	}
};

typedef t_triangle_n<t_vector3, nReal> triangle;
//-------------------------------------------------------------------
#endif

