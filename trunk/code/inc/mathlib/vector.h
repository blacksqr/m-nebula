#ifndef N_VECTOR_H
#define N_VECTOR_H
//-------------------------------------------------------------------
/**
    @template<typename ntype> class t_vector2
    @brief 2-dimensional ntype vector template<typename ntype> class t_with operators.
*/
/**
    @template<typename ntype> class t_vector3
    @brief 3-dimensional ntype vector template<typename ntype> class t_with operators.
*/
/**
    @template<typename ntype> class t_vector4
    @brief 4-dimensional ntype vector template<typename ntype> class t_with operators.
*/
//-------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "mathlib/nmath.h"
#pragma warning( push )
#pragma warning( disable : 4201 )

//------------------------------------------------------------------------------
/**
	class t_vector3
*/
template<typename ntype>
class t_vector2 
{
public:
	union {
		union {
			struct {ntype x, y;}; ///< coordinates
			struct {ntype l, a;}; ///< luminance, alpha
		};		
		struct { ntype v[2]; };
	};    

	
    //-- constructors -----------------------------------------------
	t_vector2()									: x(NT_ZERO),	y(NT_ZERO) {}
    t_vector2(const ntype& nx, const ntype& ny) : x(nx),		y(ny) {}
    t_vector2(const t_vector2& o)				: x(o.x),		y(o.y) {}
	t_vector2(const ntype* p)					: x(p[0]),		y(p[1]) {}

    //-- setting elements -------------------------------------------
    void set(const ntype& nx, const ntype& ny) 	{ this->x = nx;  this->y = ny; }
	void set(const t_vector2& o) 				{ this->set(o.x, o.y); }

	int cmp(const t_vector2& o) const			{return memcmp(v, o.v, sizeof(ntype) * 2);}

	void self_add(const t_vector2& o)	{ x += o.x; y += o.y;}
	void self_sub(const t_vector2& o)	{ x -= o.x; y -= o.y;}
	void self_mul(const ntype& s)		{ x *= s; y *= s;}
	void self_div(const ntype& s)		{ x /= s; y /= s;}	

	//-- comparison operators ----------------------------------------------
	bool operator&& (const t_vector2& o) const { return this->fuz_eq(o);}
	bool operator== (const t_vector2& o) const { return this->cmp(o) == 0;}
    bool operator!= (const t_vector2& o) const { return this->cmp(o) != 0;}	
    bool operator<  (const t_vector2& o) const { return this->cmp(o)  < 0;}
    bool operator<= (const t_vector2& o) const { return this->cmp(o) <= 0;}
    bool operator>  (const t_vector2& o) const { return this->cmp(o)  > 0;}
    bool operator>= (const t_vector2& o) const { return this->cmp(o) >= 0;}

	//-- unary minus -------------------------------------------
	t_vector2 operator-() const { return t_vector2(-x, -y); }
	//-- normalized vector -------------------------------------
	t_vector2 operator~() const { ntype l = len(); return (l > NT_ZERO ? (*this)*(NT_ONE/l) : *this); }

	//-- binary update operators ----------------------------------------------
	t_vector2& operator= (const t_vector2& o) { this->set(o); return *this; }
	t_vector2& operator+=(const t_vector2& o) { this->self_add(o); return *this; }
    t_vector2& operator-=(const t_vector2& o) { this->self_sub(o); return *this; }
    t_vector2& operator*=(const ntype& s)	  { this->self_mul(o); return *this; }
    t_vector2& operator/=(const ntype& s)	  { this->self_div(o); return *this; }

	//-- binary operators -----------------------------------------------------
	t_vector2 operator+(const t_vector2& o) const { return t_vector2(x + o.x, y + o.y); }
    t_vector2 operator-(const t_vector2& o) const { return t_vector2(x - o.x, y - o.y); }
    t_vector2 operator*(const ntype& s)	const	  { return t_vector2(x * s, y * s); }
    t_vector2 operator/(const ntype& s)	const	  { return t_vector2(x / s, y / s); }

	//--- dot product ---------------------------------------------------------
	ntype operator%(const t_vector2& o) const { return x * o.x + y * o.y; }
	//--- cross product value (z component)------------------------------
	ntype operator*(const t_vector2& o) const {  return x * o.y - y * o.x; }		

	//--- by index access operator  -----------------------------------
	ntype& operator[](size_t i)			{ return v[i]; }
	ntype  operator[](size_t i) const	{ return v[i]; }

    //-- misc -------------------------------------------------------    
    ntype len_squared(void) const	{ return n_sqr(x) + n_sqr(y); }
	ntype len(void) const			{ return n_sqrt(this->len_squared()); }
	
	void norm(void) 
	{        
		ntype l = this->len();
        if (l > NT_ZERO) 		
			this->self_div(l);        
    }

	ntype distance(const t_vector2& o)
	{
		return t_vector2(*this - o).len();
	}

	ntype theta(void) const 
	{
		if (n_eqz(x) && n_eqz(y))
			return (ntype)-1;

		if (n_eqz(x))
			return (y > NT_ZERO ? (ntype)N_HALF_PI : (ntype)3*(ntype)N_HALF_PI);

		ntype add = (x <= NT_ZERO ? 
			(ntype)N_PI : (y < NT_ZERO ? (ntype)N_DOUBLE_PI : NT_ZERO));		

		return n_atan(y/x) + add; // radians			
	}

    //--- fuzzy comparison -----------------------------------
	bool fuz_eq(const t_vector2& o, ntype tol = NT_TOL) const { return t_vector2(*this - o).len_squared() < tol; }

	bool isident() {return n_eqz(x) && n_eqz(y);}
    bool isone()   {return n_eq(x, NT_ONE) && n_eq(y, NT_ONE);}		

	// projection of vector o on this
	t_vector2 proj(const t_vector2& o) const
	{
		t_vector2<ntype> u(~(*this));		
		return u*(o%u);		
	}

	// special vectors
    N_MATH_PUBLIC static const t_vector2 zero;
    N_MATH_PUBLIC static const t_vector2 unit_x;  // (1,0)
    N_MATH_PUBLIC static const t_vector2 unit_y;  // (0,1)    
    N_MATH_PUBLIC static const t_vector2 one;     // (1,1)
};

//------------------------------------------------------------------------------
/**
	class t_vector3	
*/
template<typename ntype>
class t_vector3 
{
public:
	union {
		union {
			struct {ntype x, y, z;}; ///< coordinates
			struct {ntype r, g, b;}; ///< color
		};		
		struct {ntype v[3];};
	};
	
    //-- constructors -----------------------------------------------
	t_vector3()													: x(NT_ZERO),	y(NT_ZERO),	z(NT_ZERO) {}
    t_vector3(const ntype& nx, const ntype& ny, const ntype& nz, bool normalize = false):
		x(nx),		y(ny),		z(nz) 
		{ if (normalize) this->norm(); }
    t_vector3(const t_vector3& o, bool normalize = false)		: x(o.x),		y(o.y),		z(o.z) 
		{ if (normalize) this->norm(); }
	t_vector3(const ntype* p)									: x(p[0]),		y(p[1]),	z(p[2]) {}

    //-- set elements -------------------------------------------
    void set(const ntype& nx, const ntype& ny, const ntype& nz) { this->x = nx;  this->y = ny; this->z = nz; }
	void set(const t_vector3& o) 								{ memcpy(v, o.v, sizeof(ntype) * 3);		 }

	//-- compare -------------------------------------------
	int cmp(const t_vector3& o) const							{ return memcmp(v, o.v, sizeof(ntype) * 3);	 }

	//-- self actions -------------------------------------------
	void self_add(const t_vector3& o)	{ x += o.x; y += o.y; z += o.z;}
	void self_sub(const t_vector3& o)	{ x -= o.x; y -= o.y; z -= o.z;}
	void self_mul(const ntype& s)		{ x *= s; y *= s; z *= s;}
	void self_div(const ntype& s)		{ x /= s; y /= s; z /= s;}	

	//-- comparison operators ----------------------------------------------
	bool operator&& (const t_vector3& o) const { return this->fuz_eq(o);}
	bool operator== (const t_vector3& o) const { return this->cmp(o) == 0;}
    bool operator!= (const t_vector3& o) const { return this->cmp(o) != 0;}	
    bool operator<  (const t_vector3& o) const { return this->cmp(o)  < 0;}
    bool operator<= (const t_vector3& o) const { return this->cmp(o) <= 0;}
    bool operator>  (const t_vector3& o) const { return this->cmp(o)  > 0;}
    bool operator>= (const t_vector3& o) const { return this->cmp(o) >= 0;}

	//-- unary minus -------------------------------------------
	t_vector3 operator-() const { return t_vector3(-x, -y, -z); }
	
	//-- normalized vector -------------------------------------
	t_vector3 operator~() const { ntype l = len(); return (l > NT_ZERO ? (*this)*(NT_ONE/l) : *this); }

	//-- binary update operators ----------------------------------------------
	t_vector3& operator= (const t_vector3& o) { this->set(o);		return *this; }
	t_vector3& operator+=(const t_vector3& o) { this->self_add(o);	return *this; }
    t_vector3& operator-=(const t_vector3& o) { this->self_sub(o);	return *this; }
    t_vector3& operator*=(const ntype& s)	  { this->self_mul(s);	return *this; }
    t_vector3& operator/=(const ntype& s)	  { this->self_div(s);	return *this; }

	//-- binary operators -----------------------------------------------------
	t_vector3 operator+(const t_vector3& o) const { return t_vector3(x + o.x, y + o.y, z + o.z); }
    t_vector3 operator-(const t_vector3& o) const { return t_vector3(x - o.x, y - o.y, z - o.z); }
    t_vector3 operator*(const ntype& s)	const	  { return t_vector3(x * s, y * s, z * s); }
    t_vector3 operator/(const ntype& s)	const	  { return t_vector3(x / s, y / s, z / s); }

	//--- dot product ---------------------------------------------------------
	ntype operator%(const t_vector3& o) const { return x*o.x + y*o.y + z*o.z; }
	//--- cross product value (z component)------------------------------
	t_vector3 operator*(const t_vector3& o) const 
	{ 
		return t_vector3(y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x); 
	}		

	//--- by index access operator  -----------------------------------
	ntype& operator[](size_t i)			{ return v[i]; }
	ntype  operator[](size_t i) const	{ return v[i]; }
	//-- misc -------------------------------------------------------    
    ntype len_squared(void) const	{ return n_sqr(x) + n_sqr(y) + n_sqr(z); }
	ntype len(void) const			{ return n_sqrt(this->len_squared()); }
	
	const t_vector3& norm(void) 
	{        
		ntype l = this->len();
        if (l > NT_ZERO) 		
			this->self_div(l);  
		return *this;
    }

	ntype distance(const t_vector3& o) const
	{
		return t_vector3(*this - o).len();
	}

    //--- fuzzy comparison -----------------------------------
	bool fuz_eq(const t_vector3& o, ntype tol = NT_TOL) const { return t_vector3(*this - o).len_squared() < tol; }

	bool isident() {return n_eqz(x) && n_eqz(y) && n_eqz(z);}
    bool isone()   {return n_eq(x, NT_ONE) && n_eq(y, NT_ONE) && n_eq(z, NT_ONE);}	

	// projection of vector v on this
	t_vector3 proj(const t_vector3& o) const
	{
		t_vector3 w(~(*this));		
		return w*(v%w);		
	}

	// special vectors
    N_MATH_PUBLIC static const t_vector3 zero;
    N_MATH_PUBLIC static const t_vector3 unit_x;  // (1,0,0)
    N_MATH_PUBLIC static const t_vector3 unit_y;  // (0,1,0)
    N_MATH_PUBLIC static const t_vector3 unit_z;  // (0,0,1)    
    N_MATH_PUBLIC static const t_vector3 one;     // (1,1,1)
};


//------------------------------------------------------------------------------
/**
	class t_vector4
*/
template<typename ntype>
class t_vector4 
{
public:
	union {
		union {
			struct {ntype x, y, z, w;}; ///< coordinates
			struct {ntype r, g, b, a;}; ///< color
		};		
		struct { ntype v[4]; };
	};    
	
    //-- constructors -----------------------------------------------
	t_vector4()									: x(NT_ZERO),	y(NT_ZERO),	z(NT_ZERO), w(NT_ZERO) {}
    t_vector4(const ntype& nx, const ntype& ny, 
		const ntype& nz, const ntype& nw)		: x(nx),		y(ny),		z(nz),		w(nw) {}
    t_vector4(const t_vector4& o)				: x(o.x),		y(o.y),		z(o.z),		w(o.w){}
	t_vector4(const t_vector3<ntype>& o)		: x(o.x),		y(o.y),		z(o.z),		w(NT_ONE){}
	t_vector4(const ntype* p)					: x(p[0]),		y(p[1]),	z(p[2]),	w(p[3]) {}

    //-- setting elements -------------------------------------------
    void set(const ntype& nx, const ntype& ny, 
		const ntype& nz, const ntype& nw)	{ this->x = nx;  this->y = ny; this->z = nz; this->w = nw; }
	void set(const t_vector4& o) 			{ memcpy(v, o.v, sizeof(ntype) * 4);		 }

	//-- compare -------------------------------------------
	int cmp(const t_vector4& o) const		{ return memcmp(v, o.v, sizeof(ntype) * 4);	 }

	//-- self actions -------------------------------------------
	void self_add(const t_vector4& o)	{ x += o.x; y += o.y; z += o.z; w += o.w;}
	void self_sub(const t_vector4& o)	{ x -= o.x; y -= o.y; z -= o.z; w -= o.w;}
	void self_mul(const ntype& s)		{ x *= s; y *= s; z *= s; w *= s;}
	void self_div(const ntype& s)		{ x /= s; y /= s; z /= s; w /= s;}	

	//-- comparison operators ----------------------------------------------
	bool operator&& (const t_vector4& o) const { return this->fuz_eq(o);}
	bool operator== (const t_vector4& o) const { return this->cmp(o) == 0;}
    bool operator!= (const t_vector4& o) const { return this->cmp(o) != 0;}	
    bool operator<  (const t_vector4& o) const { return this->cmp(o)  < 0;}
    bool operator<= (const t_vector4& o) const { return this->cmp(o) <= 0;}
    bool operator>  (const t_vector4& o) const { return this->cmp(o)  > 0;}
    bool operator>= (const t_vector4& o) const { return this->cmp(o) >= 0;}

	//-- unary minus -------------------------------------------
	t_vector4 operator-() const { return t_vector4(-x, -y, -z, -w); }
	
	//-- normalized vector -------------------------------------
	t_vector4 operator~() const { ntype l = len(); return (l > NT_ZERO ? (*this)*(NT_ONE/l) : *this); }

	//-- binary update operators ----------------------------------------------
	t_vector4& operator= (const t_vector4& o) { this->set(o);		return *this; }
	t_vector4& operator+=(const t_vector4& o) { this->self_add(o);	return *this; }
    t_vector4& operator-=(const t_vector4& o) { this->self_sub(o);	return *this; }
    t_vector4& operator*=(const ntype& s)	  { this->self_mul(s);	return *this; }
    t_vector4& operator/=(const ntype& s)	  { this->self_div(s);	return *this; }

	//-- binary operators -----------------------------------------------------
	t_vector4 operator+(const t_vector4& o) const { return t_vector4(x + o.x, y + o.y, z + o.z, w + o.w); }
    t_vector4 operator-(const t_vector4& o) const { return t_vector4(x - o.x, y - o.y, z - o.z, w - o.w); }
    t_vector4 operator*(const ntype& s)	const	  { return t_vector4(x * s, y * s, z * s, w * s); }
    t_vector4 operator/(const ntype& s)	const	  { return t_vector4(x / s, y / s, z / s, w / s); }

	//--- dot product ---------------------------------------------------------
	ntype operator%(const t_vector4& o) const { return x*o.x + y*o.y + z*o.z + w*o.w; }
	
	//--- by index access operator  -----------------------------------
	ntype& operator[](size_t i)			{ return v[i]; }
	ntype  operator[](size_t i) const	{ return v[i]; }
	//-- misc -------------------------------------------------------    
    ntype len_squared(void) const	{ return n_sqr(x) + n_sqr(y) + n_sqr(z) + n_sqr(w); }
	ntype len(void) const			{ return n_sqrt(this->len_squared()); }
	
	const t_vector4& norm(void) 
	{        
		ntype l = this->len();
        if (l > NT_ZERO) 		
			this->self_div(l);  
		return *this;
    }

	ntype distance(const t_vector4& o) const
	{
		return t_vector4(*this - o).len();
	}

    //--- fuzzy comparison -----------------------------------
	bool fuz_eq(const t_vector4& o, ntype tol = NT_TOL) const { return t_vector4(*this - o).len_squared() < tol; }

	bool isident() {return n_eqz(x) && n_eqz(y) && n_eqz(z) && n_eqz(w);}
    bool isone()   {return n_eq(x, NT_ONE) && n_eq(y, NT_ONE) && n_eq(z, NT_ONE) && n_eq(w, NT_ONE);}
	
	// projection of vector v on this
	t_vector4 proj(const t_vector4& o) const
	{
		t_vector4 w(~(*this));		
		return w*(v%w);		
	}

	// special vectors
    N_MATH_PUBLIC static const t_vector4 zero;
    N_MATH_PUBLIC static const t_vector4 unit_x;  // (1,0,0,0)
    N_MATH_PUBLIC static const t_vector4 unit_y;  // (0,1,0,0)
    N_MATH_PUBLIC static const t_vector4 unit_z;  // (0,0,1,0)
    N_MATH_PUBLIC static const t_vector4 unit_w;  // (0,0,0,1)
    N_MATH_PUBLIC static const t_vector4 one;     // (1,1,1,1)
};

//-------------------------------------------------------------------
template<typename ntype> 
inline t_vector2<ntype> v32v2(const t_vector3<ntype>& v3) 
{
	return t_vector2<ntype>(v3.x, v3.y);
}

typedef t_vector2<nReal> vector2;
typedef t_vector3<nReal> vector3;
typedef t_vector4<nReal> vector4;

#pragma warning( pop ) 
#endif



