#ifndef N_POLAR_H
#define N_POLAR_H
//------------------------------------------------------------------------------
/**
    @class t_polar2

    @brief A polar coordinate inline class, consisting of 2 angles theta (latitude)
    and rho (longitude).
    
    Also offers conversion between Cartesian and polar space.

    Allowed range for theta is 0..180 degree (in rad!) and for rho 0..360 degree
    (in rad).
*/
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "mathlib/vector.h"

//------------------------------------------------------------------------------
template<typename ntype>
class t_polar2
{
public:
	ntype theta;
    ntype rho;

	typedef t_vector3<ntype> t_vec3;
	typedef t_vector2<ntype> t_vec2;
    /// the default constructor    
	t_polar2() : theta(NT_ZERO), rho(NT_ZERO) {}
    /// constructor, theta and rho args
	t_polar2(const ntype t, const ntype r) : theta(t), rho(r) {} 
    /// constructor, normalized Cartesian t_vec as arg
    t_polar2(const t_vec3& v) { this->set(v); }
    /// the copy constructor
    t_polar2(const t_polar2& src) : theta(src.theta), rho(src.rho) {}
    /// the assignment operator
    t_polar2& operator=(const t_polar2& rhs) { this->set(rhs); return *this; }
    /// convert to normalized Cartesian coords 
    t_vec3 get_cartesian() const 
	{		
		ntype sin_theta = n_sin(this->theta);
		ntype cos_theta = n_cos(this->theta);
		ntype sin_rho   = n_sin(this->rho);
		ntype cos_rho   = n_cos(this->rho);
		ntype x = sin_theta * sin_rho;
		ntype y = cos_theta;
		ntype z = sin_theta * cos_rho;
		
		return t_vec3(x, y, z);
	}
    /// get theta and rho in a 2d vec
	t_vec2 get() const { return t_vec2(this->theta, this->rho); }
    /// set to polar object
    void set(const t_polar2& p) { this->theta = p.theta; this->rho = p.rho; }
    /// set to theta and rho
	void set(const ntype& t, const ntype& r) { this->theta = t; this->rho = r; }
    /// set to Cartesian 
    void set(const t_vec3& vec)
	{		
		this->theta = n_acos(vec.y);

		// build a normalized 2d t_vec of the xz component
		t_vec2 v2(vec.x, vec.z);
		v2.norm();

		// adjust dRho based on the quadrant we are in		
		if ((v2.x >= NT_ZERO) && (v2.y >= NT_ZERO))
		{
			// quadrant 1
			this->rho = n_acos(v2.x);
		}
		else if ((v2.x < NT_ZERO) && (v2.y >= NT_ZERO))
		{
			// quadrant 2 
			this->rho = n_acos(v2.y) + N_HALF_PI; // +90
		}
		else if ((v2.x < NT_ZERO) && (v2.y < NT_ZERO))
		{
			// quadrant 3
			this->rho = n_acos(-v2.x) + N_PI; // +180
		}
		else
		{
			// quadrant 4
			this->rho = n_acos(-v2.y) + N_PI + N_HALF_PI; // +270
		}
	}

    /// fuzzy equality check 
    bool isequal(const t_polar2& rhs, ntype tol)
	{
		return n_eq(this->theta, rhs.theta, tol) && n_eq(this->rho, rhs.rho, tol);
	}

private:
    /// the equal operator is not allowed, use isequal() with tolerance!
    bool operator==(const t_polar2& /*rhs*/) { return false; }
};


//------------------------------------------------------------------------------
#endif
