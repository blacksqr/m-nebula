#ifndef N_CSGPLANE_H
#define N_CSGPLANE_H
//-------------------------------------------------------------------
/**
@class csgplane
@brief extends a plane in 3d space for CSG purposes
*/
#ifndef N_PLANE_H
#include "mathlib/plane.h"
#endif

#include "mathlib/matrix.h"

//-------------------------------------------------------------------
class csgplane : public plane {
public:
    csgplane() : plane() {};

	csgplane(float _a, float _b, float _c, float _d) : plane(_a, _b, _c, _d) {};

	csgplane(const csgplane& p) {
        this->a = p.a;
        this->b = p.b;
        this->c = p.c;
        this->d = p.d;		
	};

	csgplane(const plane& p) {
        this->a = p.a;
        this->b = p.b;
        this->c = p.c;
        this->d = p.d;		
	};

	csgplane(const vector3& v0, const vector3& v1, const vector3& v2){
		this->set(v0,v1,v2);		
	};   
	
	void normalize() {
		float len = this->normal().len();
		if (n_neqz(len))
		{
			this->a /= len;
			this->b /= len;
			this->c /= len;
			this->d /= len;
		}		
	}
	

	// check if plane p is paralle to this
	bool isparallel(const plane& p)
	{
		return n_eq(n_abs(this->normal()%p.normal()), 1.0f);
	}

	// check if plane p is same like this
	bool isequal(const plane& p)
	{
		return n_eqz(this->distance(p.normal()*(-p.d))) && this->isparallel(p);
	}

    //--- get intersecting t of segment include ends with one-sided plane -----------
    //  return false if line is parallel to plane
    //
    bool intersect_seg_with_ends(const line3& l, float& t) const {        
        if (this->intersect(l, t)) 
		{        
			// [Gilad 23/8/07] to prevent detect it as non-intersected because of calculation accuracy 
			float len = l.len();
            if (n_inrange(t*len, 0.0f, len, (float)N_TOL))
			{
				n_saturate(t);
                return true;
			}
			else 
                return false;         
		} else 
            return false;        
    }

    //--- get intersecting t of segment with one-sided plane -----------
    //  return false if line is parallel to plane
    //
    bool intersect_seg(const line3& l, float& t) const {        
        if (this->intersect(l, t))
            if (t > 0.0f && t < 1.0f)
                return true;
            else 
                return false;
        else 
        	return false;        
    }
	
	 //--- compute distance of point to plane ------------------------
    //  distance is 0.0 if point is on plane
    //
    float abs_distance(const vector3& v) const {
        return n_abs(plane::distance(v));
    };
    
    ///--- which side the point in  -----------------------------------
    //   1 - right
	//	 0 - on the plane
    //	-1 - left
    long side(const vector3& v) const 
	{        
		//return n_isgn(a*v.x + b*v.y + c*v.z + d);// Gilad this is bad method that return 0 on any smaller then 1
		float f = a*v.x + b*v.y + c*v.z + d;
		return (f < -N_TINY ? -1 : (f > N_TINY ? 1 : 0));
    };

    //--- check if segment intersects the plane ------------------------
    //
    bool is_segment_intersects(const vector3& from, const vector3& to) const {
        float d1 = a*from.x + b*from.y + c*from.z + d;
        float d2 = a*to.x + b*to.y + c*to.z + d;
		return d1*d2 < N_TOL0;
    }; 

    //--- find point projection on plane ------------------------
    // find nearest point on plane
	//
    vector3 proj(const vector3& v) const {
        float l = a*v.x + b*v.y + c*v.z + d;
		vector3 p = v - vector3(a,b,c) * l;
		return p;
    };

	csgplane transform(matrix44 m) const;
	
	bool operator < (const csgplane& p) const
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
};

//-------------------------------------------------------------------
inline 
csgplane operator * (const matrix44& m, const csgplane& p)
{
	return csgplane(
		/*m.M11*p.a + m.M21*p.b + m.M31*p.c + m.M41*p.d,
		m.M12*p.a + m.M22*p.b + m.M32*p.c + m.M42*p.d,
		m.M13*p.a + m.M23*p.b + m.M33*p.c + m.M43*p.d,
		m.M14*p.a + m.M24*p.b + m.M34*p.c + m.M44*p.d);*/

		m.M11*p.a + m.M12*p.b + m.M13*p.c + m.M14*p.d,
		m.M21*p.a + m.M22*p.b + m.M23*p.c + m.M24*p.d,
		m.M31*p.a + m.M32*p.b + m.M33*p.c + m.M34*p.d,
		m.M41*p.a + m.M42*p.b + m.M43*p.c + m.M44*p.d);
};
//-------------------------------------------------------------------

inline
csgplane csgplane::transform(matrix44 m) const
{
	//m.transpose();

	csgplane lp(m * (*this));

	//lp.normalize();
	return lp;
}
#endif
