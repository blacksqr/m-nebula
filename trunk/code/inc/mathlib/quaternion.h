#ifndef N_QUATERNION_H
#define N_QUATERNION_H
//-------------------------------------------------------------------
/**
    @class t_quaternion
    @brief t_quaternion class with some basic operators.

    - 08-Dec-00   floh    extended, ATTENTION: argument ordering in
                          constructor has changed!
*/
//-------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>

#pragma warning( push )
#pragma warning( disable : 4201 )

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

//-------------------------------------------------------------------
//  t_quaternion
//-------------------------------------------------------------------
template<class ntype>
class t_quaternion 
{
public:
	typedef t_vector3<ntype> t_vector;	

	union {		
		struct {ntype x, y, z, w;}; ///< quaternion
		struct {ntype v[4];};
	};

	N_MATH_PUBLIC static const t_quaternion zero;

    //-- constructors -----------------------------------------------
    t_quaternion() : x(NT_ZERO), y(NT_ZERO), z(NT_ZERO), w(NT_ONE) {}
    t_quaternion(const ntype& _x, const ntype& _y, const ntype& _z, const ntype& _w) : x(_x), y(_y), z(_z), w(_w) {}
    t_quaternion(const t_quaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}

    //-- setting elements -------------------------------------------
    void set(const ntype& nx, const ntype& ny, 
		const ntype& nz, const ntype& nw)	{ this->x = nx;  this->y = ny; this->z = nz; this->w = nw; }
	void set(const t_quaternion& q)			{ memcpy(v, q.v, sizeof(ntype) * 4);		}
	
	//-- compare -------------------------------------------
    int cmp(const  t_quaternion& q) const	{ return memcmp(v, q.v, sizeof(ntype) * 4);	}
	
	//-- self actions -------------------------------------------
	void self_add(const t_quaternion& q)	{ x += q.x; y += q.y; z += q.z; w += q.w; }
	void self_sub(const t_quaternion& q)	{ x -= q.x; y -= q.y; z -= q.z; w -= q.w; }
	void self_mul(const ntype& s)			{ x *= s; y *= s; z *= s; w *= s;		  }
	void self_div(const ntype& s)			{ x /= s; y /= s; z /= s; w /= s;		  }	

	//-- comparison operators ----------------------------------------------
	bool operator&& (const t_quaternion& q) const { return this->fuz_eq(q);}
	bool operator== (const t_quaternion& q) const { return this->cmp(q) == 0;}
	bool operator!= (const t_quaternion& q) const { return this->cmp(q) != 0;}	

	//-- unary minus -------------------------------------------
	t_quaternion operator-() const { return t_quaternion(-x, -y, -z, -w); }
	
	//-- normalized quaternion -------------------------------------
	t_quaternion operator~() const { ntype l = magnitude(); return (l > NT_ZERO ? (*this)*(NT_ONE/l) : *this); }

	//--- by index access operator  -----------------------------------
	ntype& operator[](size_t i)			{ return v[i]; }
	ntype  operator[](size_t i) const	{ return v[i]; }

	//-- binary update operators ----------------------------------------------
	t_quaternion& operator+=(const t_quaternion& q) { this->self_add(q); return *this; }
    t_quaternion& operator-=(const t_quaternion& q) { this->self_sub(q); return *this; }
	t_quaternion& operator*=(const ntype& s)  { this->self_mul(s); return *this; }
	t_quaternion& operator/=(const ntype& s)  { this->self_div(s); return *this; }

    t_quaternion& operator*=(const t_quaternion& q) 
	{
        ntype qx = w*q.x + x*q.w + y*q.z - z*q.y;
        ntype qy = w*q.y + y*q.w + z*q.x - x*q.z;
        ntype qz = w*q.z + z*q.w + x*q.y - y*q.x;
        ntype qw = w*q.w - x*q.x - y*q.y - z*q.z;
        x = qx;
        y = qy;
        z = qz;
        w = qw;
        return *this;
    }

	//--- global operators ----------------------------------------------
	t_quaternion operator+(const t_quaternion& q) const { return t_quaternion(x+q.x, y+q.y, z+q.z, w+q.w); }
	t_quaternion operator-(const t_quaternion& q) const { return t_quaternion(x-q.x, y-q.y, z-q.z, w-q.w); }
	t_quaternion operator*(const ntype& s) const { return t_quaternion(x * s, y * s, z * s, w * s); }
    t_quaternion operator/(const ntype& s) const { return t_quaternion(x / s, y / s, z / s, w / s); }
	t_quaternion operator*(const t_quaternion& q) const {
		return t_quaternion(w*q.x + x*q.w + y*q.z - z*q.y,
                      w*q.y + y*q.w + z*q.x - x*q.z,
                      w*q.z + z*q.w + x*q.y - y*q.x,
                      w*q.w - x*q.x - y*q.y - z*q.z);
	}


    //-- misc operations --------------------------------------------
    void ident(void) 
	{
        x = NT_ZERO;
        y = NT_ZERO;
        z = NT_ZERO;
        w = NT_ONE;
    }

    void conjugate(void) { x = -x; y = -y; z = -z; }

	void scale(ntype s)	 { this->self_mul(s); }

    ntype norm() const {return n_sqr(x) + n_sqr(y) + n_sqr(z) + n_sqr(w);}

    ntype magnitude() const { return n_sqrt(this->norm());}

    void invert(void) 
	{
        ntype n = this->norm();
        if (n > NT_TINY) this->self_div(n);
        this->conjugate();
    }

    void normalize(void) 
	{
        ntype l = this->magnitude();
        if (l > NT_TINY) this->self_div(l);
        else this->ident();
    }

    //-- operators --------------------------------------------------
    bool fuz_eq(const t_quaternion& q) const 
	{
		return n_eqz(((*this) - q).norm(), n_sqr(NT_TOL));
    }  	
	
	ntype inner_product(const t_quaternion& q) const {
		return x * q.x + y * q.y + z * q.z + w * q.w;
	}

    /// rotate vector by t_quaternion
    vector3 rotate(const t_vector& v) const 
	{
        t_quaternion q(v.x * w + v.z * y - v.y * z,
                     v.y * w + v.x * z - v.z * x,
                     v.z * w + v.y * x - v.x * y,
                     v.x * x + v.y * y + v.z * z);

        return t_vector(w * q.x + x * q.w + y * q.z - z * q.y,
                       w * q.y + y * q.w + z * q.x - x * q.z,
                       w * q.z + z * q.w + x * q.y - y * q.x);
    }

    /// convert from two directions, must be unit vectors
    void set_from_axes(const t_vector& from, const t_vector& to) 
    {
        ntype cosa = from % to;
        if (cosa < (ntype)-1 + NT_TINY )  // angle close to N_PI
        {
           t_vector vcross( NT_ZERO, from.x, -from.y );
           if (n_sqr(from.z) > n_sqr(from.y))
                  vcross.set(-from.z, NT_ZERO, from.x);
           vcross.norm();
           x = vcross.x; y = vcross.y; z = vcross.z;
           w = NT_ZERO;
        }
        else
        {
           t_vector bisect(from + to);
           bisect.norm();
           t_vector bcross(from*bisect);
           x = bcross.x; y = bcross.y; z = bcross.z;
           w = from % bisect;
        }
    }

    //-- convert from euler angles ----------------------------------
    void set_rotate_axis_angle(const t_vector& v, ntype a) {
        ntype sin_a = n_sin(n_half(a));
        ntype cos_a = n_cos(n_half(a));
        x = v.x * sin_a;
        y = v.y * sin_a;
        z = v.z * sin_a;
        w = cos_a;
    }

    void set_rotate_x(ntype a) {
        ntype sin_a = n_sin(n_half(a));
        ntype cos_a = n_cos(n_half(a));
        x = sin_a;
        y = NT_ZERO;
        z = NT_ZERO;
        w = cos_a;
    }

    void set_rotate_y(ntype a) {
        ntype sin_a = n_sin(n_half(a));
        ntype cos_a = n_cos(n_half(a));
        x = NT_ZERO;
        y = sin_a;
        z = NT_ZERO;
        w = cos_a;
    }

    void set_rotate_z(ntype a) {
        ntype sin_a = n_sin(n_half(a));
        ntype cos_a = n_cos(n_half(a));
        x = NT_ZERO;
        y = NT_ZERO;
        z = sin_a;
        w = cos_a;
    }

    void set_rotate_xyz(ntype ax, ntype ay, ntype az) {
        t_quaternion qx, qy, qz;
        qx.set_rotate_x(ax);
        qy.set_rotate_y(ay);
        qz.set_rotate_z(az);
        *this = qx;
        *this *= qy;
        *this *= qz;
    }

    bool isident() {return (n_eqz(x) && n_eqz(y) && n_eqz(z) && n_eq(w, (ntype)1));}

    //-- rotation interpolation, set this matrix to the -------------
    //-- interpolated result of q0->q1 with l as interpolator -------
    void slerp(const t_quaternion& q0, const t_quaternion& q1, ntype l) 
    {
        ntype fScale1;
        ntype fScale2;
        t_quaternion A(q0);
        t_quaternion B(q1);

        // compute dot product, aka cos(theta):
        ntype fCosTheta = A.x*B.x + A.y*B.y + A.z*B.z + A.w*B.w;

        if (fCosTheta < NT_ZERO) 
        {
            // flip start t_quaternion
           A.x = -A.x; A.y = -A.y; A.z = -A.z; A.w = -A.w;
           fCosTheta = -fCosTheta;
        }

        if ((fCosTheta + NT_ONE) > (ntype)0.05) 
        {
            // If the quaternions are close, use linear interploation
            if ((NT_ONE - fCosTheta) < (ntype)0.05) 
            {
                fScale1 = NT_ONE - l;
                fScale2 = l;
            }
            else 
            { 
                // Otherwise, do spherical interpolation
                ntype fTheta    = n_acos(fCosTheta);
                ntype fSinTheta = n_sin(fTheta);
                fScale1 = n_sin( fTheta * (NT_ONE - l) ) / fSinTheta;
                fScale2 = n_sin( fTheta * l ) / fSinTheta;
            }
        }
        else 
        {
            B.x = -A.y;
            B.y =  A.x;
            B.z = -A.w;
            B.w =  A.z;
            fScale1 = n_sin( NT_PI * ((ntype)0.5 - l) );
            fScale2 = n_sin( NT_PI * l );
        }

        x = fScale1 * A.x + fScale2 * B.x;
        y = fScale1 * A.y + fScale2 * B.y;
        z = fScale1 * A.z + fScale2 * B.z;
        w = fScale1 * A.w + fScale2 * B.w;
    }
};


typedef t_quaternion<nReal> quaternion;
//-------------------------------------------------------------------
#pragma warning( pop ) 
#endif
