#ifndef N_PLANE_H
#define N_PLANE_H
//-------------------------------------------------------------------
/**
@class t_plane
@brief a t_plane in 3d space
*/
//-------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#pragma warning( push )
#pragma warning( disable : 4201 )

//-------------------------------------------------------------------
template<class ntype> 
class t_plane 
{
public:
	typedef t_vector3<ntype> t_vector;
	typedef t_line_n<t_vector3, ntype> t_line;
	typedef t_ray_n<t_vector3, ntype>	t_ray;
	typedef t_segment_n<t_vector3, ntype>  t_segment;
	typedef t_minsegment_n<t_vector3, ntype>  t_minsegment;

	union 
	{		
		struct {ntype a, b, c, d;}; ///< coordinates					
		struct {ntype p[4]; };
	};
    

    t_plane() : a(NT_ZERO), b(NT_ZERO), c(NT_ZERO), d(NT_ONE) {}
    t_plane(const ntype& _a, const ntype& _b, const ntype& _c, const ntype& _d)	{ 
		this->set(_a, _b, _c, _d);
	}
	t_plane(const t_vector& v0, const t_vector& v1, const t_vector& v2) {
        this->set(v0, v1, v2);
    }
    t_plane(const t_plane& p){ this->set(p.a, p.b, p.c, p.d); }

    void set(const ntype& _a, const ntype& _b, const ntype& _c, const ntype& _d) {
        a = _a; b = _b; c = _c; d = _d;
	}

    //--- construct t_plane from 3 vectors ----------------------------
    void set(const t_vector& v0, const t_vector& v1, const t_vector& v2) {
        t_vector cross((v2-v0) * (v1-v0), true);        
        a = cross.x;
        b = cross.y;
        c = cross.z;
        d = -(a*v0.x + b*v0.y + c*v0.z);
    }


	//--- by index access operator  -----------------------------------
	ntype& operator[](size_t i)			{ return p[i]; }
	ntype  operator[](size_t i) const	{ return p[i]; }

	//-------------------------------------------------------------------
	t_plane operator * (const t_matrix44<ntype>& m) const
	{
		return t_plane(
			m.M11*a + m.M12*b + m.M13*c + m.M14*d,
			m.M21*a + m.M22*b + m.M23*c + m.M24*d,
			m.M31*a + m.M32*b + m.M33*c + m.M34*d,
			m.M41*a + m.M42*b + m.M43*c + m.M44*d);
	}

	bool operator < (const t_plane& p) const
	{		
		if (n_eq(this->d, p.d, 0.001f))
		{
			if (n_eq(this->a, p.a, 0.001f))
			{
				if (n_eq(this->b, p.b, 0.001f))
				{
					if (n_eq(this->c, p.c, 0.001f))
					{
						return false;
					}
					else
					{
						return this->c < p.c;
					}
				}
				else 
					return this->b < p.b;
			}
			else 
				return this->a < p.a;
		}
		else 
			return this->d < p.d;
	}

    //--- compute distance of point to t_plane ------------------------
    //  distance is 0.0 if point is on t_plane
    //
    ntype distance(const t_vector& v) const { return a*v.x + b*v.y + c*v.z + d; }	

    //--- get t_plane normal ------------------------------------------
    t_vector normal(bool norm = true) const { return t_vector(a, b, c, norm); }
				   
    //--- get intersecting t of line with one-sided t_plane -----------
    //  return false if line is parallel to t_plane
    //
    bool intersect(const t_line& l, ntype& t) const 
	{
        ntype f0 =  a * l.va().x + b * l.va().y + c * l.va().z + d;
        ntype f1 = -a * l.dir().x - b * l.dir().y - c * l.dir().z;

		if (n_neqz(f1))
        {
			t = (ntype)(f0 / f1);
            return true;
        }
        else
        {
            return false;
        }
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

	//--- check if segment intersects the plane ------------------------
    //
    bool intersect(const t_segment& l) const 
	{
        ntype d1 = a*l.a.x + b*l.a.y + c*l.a.z + d;
        ntype d2 = a*l.b.x + b*l.b.y + c*l.b.z + d;
		return d1*d2 < N_TOL0;
    }

	//--- get intersection t_plane/t_plane intersection -----------------
	//  return false is planes are parallel
	bool intersect(t_plane& p, t_line& l) const 
	{
		t_vector n0 = normal();
		t_vector n1 = p.normal();
		ntype n00 = n0 % n0;
		ntype n01 = n0 % n1;
		ntype n11 = n1 % n1;
		ntype det = n00*n11 - n01*n01;
		if (n_eqz(det, NT_TINY))
			return false;
		else 
		{
			ntype inv_det = NT_ONE/det;
			ntype c0 = (n11*d   - n01*p.d) * inv_det;
			ntype c1 = (n00*p.d - n01*d)   * inv_det;
			l.dir() = n0 * n1;
			l.a = n0*c0 + n1*c1;
			l.b = l.a + l.dir();
			return true;
		}
	}

	 //  distance is 0.0 if point is on plane
    //
    float abs_distance(const t_vector& v) const {
        return n_abs(t_plane::distance(v));
    }
    
    ///--- which side the point in  -----------------------------------
    //   1 - right
	//	 0 - on the plane
    //	-1 - left
    long side(const t_vector& v) const 
	{        
		//return n_isgn(a*v.x + b*v.y + c*v.z + d);// Gilad this is bad method that return 0 on any smaller then 1
		float f = a*v.x + b*v.y + c*v.z + d;
		return (f < -N_TINY ? -1 : (f > N_TINY ? 1 : 0));
    }


	// check if plane p is same like this
	bool isequal(const t_plane& p)
	{
		return n_eqz(this->distance(p.normal()*(-p.d))) && this->isparallel(p);
	}

	// check if plane p is paralle to this
	bool isparallel(const t_plane& p)
	{
		return n_eq(n_abs(this->normal()%p.normal()), 1.0f);
	}

	void normalize() 
	{
		float len = this->normal().len();
		if (n_neqz(len))
		{
			this->a /= len;
			this->b /= len;
			this->c /= len;
			this->d /= len;
		}		
	}	

	//--- find point projection on plane ------------------------
    // find nearest point on plane
	//
    t_vector proj(const t_vector& v) const 
	{
        ntype l = a*v.x + b*v.y + c*v.z + d;
		return (v - this->normal(false) * l);		
    }

	
	//-------------------------------------------------------------------

	inline
	t_plane transform(t_matrix44<ntype> m) const
	{
		t_plane lp((*this)*m);
		//lp.normalize();
		return lp;
	}	
};

typedef t_plane<nReal> plane;

#pragma warning( pop )
//-------------------------------------------------------------------
#endif
