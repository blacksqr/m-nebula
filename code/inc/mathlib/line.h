#ifndef N_LINE_H
#define N_LINE_H
//-------------------------------------------------------------------
/**
    @class t_line2<ntype>
    @brief a line in 2d space
*/

/**
    @class line3
    @brief a line in 3d space

    - 04-Dec-01   floh    bugfix: line3::len() was broken
*/
//-------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif
#pragma warning( push )
#pragma warning( disable : 4201 )

//-------------------------------------------------------------------
template<template<class> class vtype, typename ntype> 
class t_line_n 
{
public:
	typedef vtype<ntype> t_vector;

public:
	t_vector a;
    t_vector b;
    t_vector m;

	//-- constructors -----------------------------------------------
    t_line_n(){}
	t_line_n(const t_vector& v0, const t_vector& v1) : a(v0), b(v1), m(v1-v0) {}
    t_line_n(const t_line_n& l) : a(l.a), b(l.b), m(l.m) {}

	//-- set elements -------------------------------------------
    void set(const t_vector& v0, const t_vector& v1) 
	{ 
		this->a = v0; 
		this->b = v1; 
		this->m = v1 - v0;
	}

    const t_vector& va()			const { return a; }	
    const t_vector& vb()			const { return b; }
	const t_vector& dir()			const { return m; }
    ntype			len()			const { return m.len(); }    
	ntype			len_squared()	const { return m.len_squared();	}

    //--- minimal distance of point to line -------------------------
    ntype distance(const t_vector& p) const
	{
        t_vector diff(p - a);
        ntype l = m % m;

        if (n_neqz(l)) 
		{
            ntype t = (m % diff) / l;
            diff = diff - m*t;
            return diff.len();
		}
        else // line is really a point...
            return (p-a).len();
    }

	/**
		Calculate the segment dl that is the shortest route between
		two lines this and l. 
		Return: 
			-1 if no solution exists.
			0 if no segment intersection
			1 if segments intersects
	*/
	int distance(const t_line_n& l, t_line_n& dl) const 
	{
		//vector3 p1, vector3 p2, vector3 p3, vector3 p4,vector3& pa, vector3& pb) {   
		t_vector v1(this->dir());
		t_vector v2(l.dir());
		t_vector vd(this->va() - l.va());   
		
		ntype dvdv2, dv2v1, dvdv1, dv2v2, dv1v1;
		ntype numer, denom;

		if (v2.len_squared() < NT_TINY)
			return -1;
		
		if (v1.len_squared() < NT_TINY)
			return -1;   

		dvdv2 = vd%v2; //p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
		dv2v1 = v2%v1;	//p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
		dvdv1 = vd%v1;	//p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
		dv2v2 = v2%v2;	//p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
		dv1v1 = v1%v1;	//p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;
		
		denom = dv1v1 * dv2v2 - dv2v1 * dv2v1;
		if (n_abs(denom) < NT_TINY)
			return -1;
		
		numer = dvdv2 * dv2v1 - dvdv1 * dv2v2;
		
		ntype t1 = numer / denom;
		ntype t2 = (dvdv2 + dv2v1 * (t1)) / dv2v2;
		
		dl.set(this->ipol(t1), l.ipol(t2));   
		
		if (n_in1(t1) && n_in1(t2) && n_eqz(dl.len()))
			return 1;
		else 
			return 0;
	} 
	
	ntype project(const t_vector& p) const
	{	
		ntype sqrMag = m.len_squared();
		if (sqrMag < NT_TINY)
			return NT_ZERO;
		ntype t = (m % (p-this->va())) / sqrMag;
		return t;
	}

    //--- get 3d point on line given t ------------------------------
    t_vector ipol(const ntype t) const 
	{ 
		return t_vector(a + m*t); 
	}
};

template<template<class> class vtype, typename ntype> 
class t_ray_n : public t_line_n<vtype, ntype>
{	
public:
	t_ray_n() : t_line_n() {}
	t_ray_n(const t_vector& v0, const t_vector& v1) : t_line_n(v0, v1) {}
	t_ray_n(const t_ray_n& s) : t_line_n(s.a, s.b) {}
};

template<template<class> class vtype, typename ntype> 
class t_segment_n : public t_line_n<vtype, ntype>
{
public:
	t_segment_n() : t_line_n() {}
	t_segment_n(const t_vector& v0, const t_vector& v1) : t_line_n(v0, v1) {}
	t_segment_n(const t_segment_n& s) : t_line_n(s.a, s.b) {}
};

template<template<class> class vtype, typename ntype> 
class t_minsegment_n : public t_line_n<vtype, ntype>
{
public:
	t_minsegment_n() : t_line_n() {}

	t_minsegment_n(const t_vector& v0, const t_vector& v1) { this->set(v0, v1); }

    t_minsegment_n(const t_line_n& l) {this->set(l.a, l.b);}

    void set(const t_vector& v0, const t_vector& v1) 
	{ 
		if (v0 < v1) 
		{
			this->a = v0;
			this->b = v1;
		}
		else
		{
			this->a = v1;
			this->b = v0;
		}
		this->m = this->b - this->a;
	}
};

typedef t_line_n<t_vector2, nReal> line2;
typedef t_line_n<t_vector3, nReal> line3;
typedef t_ray_n<t_vector3, nReal>		 ray3;
typedef t_segment_n<t_vector3, nReal>	 segment3;
typedef t_minsegment_n<t_vector3, nReal> minsegment3;
//-------------------------------------------------------------------
#pragma warning( pop ) 
#endif
