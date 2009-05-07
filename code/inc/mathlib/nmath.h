#ifndef N_MATH_H
#define N_MATH_H
//------------------------------------------------------------------------------
/**
    General math functions and macros.

    (C) 2003 RadonLabs GmbH
    - 26-Oct-00   floh    no longer depends on Nebula
	- 08-Dec-00   floh    + n_sin/n_cos
	- 25-Nov-02   Ilya    + n_sqrt use fastsqrt from nVidia
    - 07-Jun-05    kims    Added 'n_atan2', 'n_exp', 'n_floor', 'n_ceil' and
                           'n_pow' macros.
*/

#include <math.h>
#include <stdlib.h>
#include "mathlib/float_cast.h"

#ifdef _LIB // dll
#define N_MATH_PUBLIC __declspec(dllexport)
#else // client
#define N_MATH_PUBLIC __declspec(dllimport)
#endif

#ifdef _MSC_VER
#define isnan _isnan
#define isinf _isinf
#endif

union nfloat
{
public:
	nfloat() : l(0) { }
	nfloat(float _f) : f(_f) {}
	operator float() const { return f; }

	bool operator !=(const nfloat& nf) const { return l != nf.l; }
	bool operator ==(const nfloat& nf) const { return l == nf.l; }

	bool operator < (const nfloat& nf) { return f <  nf.f; }
	bool operator > (const nfloat& nf) { return f >  nf.f; }
	bool operator <=(const nfloat& nf) { return f <= nf.f; }
	bool operator >=(const nfloat& nf) { return f >= nf.f; }		
	
	nfloat& operator  =(const nfloat& nf) { l  = nf.l; return *this; }
	nfloat& operator +=(const nfloat& nf) { f += nf.f; return *this; }
	nfloat& operator -=(const nfloat& nf) { f -= nf.f; return *this; }
	nfloat& operator *=(const nfloat& nf) { f *= nf.f; return *this; }
	nfloat& operator /=(const nfloat& nf) { f /= nf.f; return *this; }

	nfloat operator +(const nfloat& nf) const { return f + nf.f; }
	nfloat operator -(const nfloat& nf) const { return f - nf.f; }
	nfloat operator *(const nfloat& nf) const { return f * nf.f; }
	nfloat operator /(const nfloat& nf) const { return f / nf.f; }

	float f;
	long l;
};


typedef float nReal;

//-------------------------------------------------------------------
#ifndef N_PI
#define N_PI (3.1415926535897932384626433832795)
#endif

#ifndef N_PI_DIV_180
#define N_PI_DIV_180 (0.017453292519943295769236907684886)
#endif

#ifndef N_180_DIV_PI
#define N_180_DIV_PI (57.295779513082320876798154814105)
#endif

#ifndef N_HALF_PI
#define N_HALF_PI (1.570796326794896)				   
#endif

#ifndef N_DOUBLE_PI
#define N_DOUBLE_PI (6.283185307179586)
#endif

#ifndef N_LN_2
#define N_LN_2 0.693147180559945
#endif

#ifndef N_TOL
#define N_TOL	(1e-06)
#endif

#ifndef N_TOL0
#define N_TOL0	(-N_TOL)
#endif

#ifndef N_TOL1
#define N_TOL1	(1.0 + N_TOL)
#endif


#ifndef N_TINY
#define N_TINY	(1e-06)
#endif

#define NT_ZERO ((ntype)0)
#define NT_ONE  ((ntype)1)
#define NT_TWO  ((ntype)2)
#define NT_TINY ((ntype)N_TINY)
#define NT_TOL  ((ntype)N_TOL)

#define NT_PI			((ntype)N_PI)
#define NT_HALF_PI		((ntype)N_HALF_PI)
#define NT_DOUBLE_PI	((ntype)N_DOUBLE_PI)

//------------------------------------------------------------------------------
/**
	Comparisons & Relations
*/
template <typename ntype> 
inline ntype n_max(const ntype& a, const ntype& b) { return (a > b ? a : b); }

template <typename ntype> 
inline ntype n_min(const ntype& a, const ntype& b) { return (a < b ? a : b); }

template <typename ntype> 
inline ntype n_max3(const ntype& a, const ntype& b, const ntype& c) { return n_max(n_max(a, b), c); }

template <typename ntype> 
inline ntype n_min3(const ntype& a, const ntype& b, const ntype& c) { return n_min(n_min(a, b), c); }

template <typename ntype>
inline ntype  n_abs(const ntype& f) { return ((f < NT_ZERO) ? -f : f); }

inline float  n_abs(float f) { return fabs(f); }
inline double n_abs(double f){ return fabs(f); }

template <typename ntype>
inline void n_swap(ntype& a, ntype& b) { ntype tmp=a; a=b; b=tmp;}


template <typename ntype>
inline ntype  n_half(const ntype& f) { return f/(ntype)2; }
inline float  n_half(float f) { return f*0.5f; }
inline double n_half(double f){ return f*0.5;  }

inline int		n_half(int i)	{ return (i>>1); }
inline long		n_half(long i)	{ return (i>>1); }
inline short	n_half(short i)	{ return (i>>1); }
inline unsigned int		n_half(unsigned int i)		{ return (i>>1); }
inline unsigned long	n_half(unsigned long i)		{ return (i>>1); }
inline unsigned short	n_half(unsigned short i)	{ return (i>>1); }


//------------------------------------------------------------------------------
/**
	Fuzzy Comparisons & Relations
*/
template <typename ntype>
inline bool n_eqz (const ntype& x, ntype tol = NT_TINY) { return (n_abs(x) < tol); }
template <typename ntype>
inline bool n_neqz(const ntype& x, ntype tol = NT_TINY) { return !n_eqz(x, tol); }

template <typename ntype>
inline bool n_eq  (const ntype& a, const ntype& b, ntype tol = NT_TINY)
{ 
	return n_eqz(a - b, tol); 
}

template <typename ntype>
inline bool n_neq  (const ntype& a, const ntype& b, ntype tol = NT_TINY)
{ 
	return n_neqz(a - b, tol); 
}

template <typename ntype>
inline ntype n_sgn(const ntype& f, ntype tol = NT_TINY) 
{ 
	return (n_eqz(f, tol) ? NT_ZERO : (f < NT_ZERO ? -NT_ONE : NT_ONE)); 
}

template <typename ntype>
inline long   n_isgn(const ntype& f, ntype tol = NT_TINY)
{ 
	return (n_eqz(f, tol) ? 0 : (f < NT_ZERO  ? -1	: 1)); 
}

template <typename ntype>
inline bool n_less(const ntype& f0, const ntype& f1, ntype tol = NT_TINY) 
{ 
	return (f0 - f1) < tol; 
}

template <typename ntype>
inline bool n_greater(const ntype& f0, const ntype& f1, ntype tol = NT_TINY) 
{ 
	return (f0 - f1) > tol; 
}

//------------------------------------------------------------------------------
/**
	check if value in range
*/
template <typename ntype>
inline bool n_inrange(const ntype& val, const ntype& lower, const ntype& upper, ntype tol = NT_ZERO)
{
	return (val >= lower - tol && val <= upper + tol);
}

//------------------------------------------------------------------------------
/**
	check if value in range 0-1
*/
template <typename ntype>
inline bool n_in1(const ntype& val, ntype tol = NT_TINY) { return (n_inrange(val, NT_ZERO, NT_ONE, tol)); }

//------------------------------------------------------------------------------
/**
    Clamp a value against lower und upper boundary.
*/
template <typename ntype>
inline ntype n_clamp(const ntype& val, const ntype& lower, const ntype& upper)
{	
	if (val < lower)      return lower;
    else if (val > upper) return upper;
    else                  return val;
}

//------------------------------------------------------------------------------
/**
    Saturate a value (clamps between 0.0f and 1.0f)
*/
template <typename ntype>
inline ntype n_saturate(const ntype& val) { return n_clamp(val, NT_ZERO, NT_ONE); }


//------------------------------------------------------------------------------
/**
	Trigonometric
*/
template <typename ntype>
inline ntype n_deg2rad(const ntype& d) { return d * (ntype)N_PI_DIV_180; }
template <typename ntype>
inline ntype n_rad2deg(const ntype& r) { return r * (ntype)N_180_DIV_PI; }


#define n_sin	sin
#define n_cos	cos
#define n_tan	tan
#define n_atan	atan
#define n_atan2 atan2
#define n_exp	exp
#define n_floor	floor
#define n_ceil	ceil
#define n_pow	pow

//------------------------------------------------------------------------------
/**
    log2() function.
*/
template <typename ntype>
inline ntype n_log2(const ntype& f) { return log(f) / (ntype)N_LN_2; }

template <typename ntype>
inline ntype n_sqrt(const ntype& x) { return sqrt(x); }


//inline nfloat n_sqrt(const nfloat& x) { return sqrt(x.f); }

template <typename ntype>
inline ntype n_sqr(const ntype& x) { return x*x; }


//------------------------------------------------------------------------------
/**
    acos with value clamping.
*/
template <typename ntype>
inline ntype n_acos(const ntype& x) { return acos(n_clamp(x, -NT_ONE, NT_ONE)); }

//------------------------------------------------------------------------------
/**
    asin with value clamping.
*/
template <typename ntype>
inline ntype n_asin(const ntype& x) { return asin(n_clamp(x, -NT_ONE, NT_ONE)); }

//------------------------------------------------------------------------------
/**
    Smooth a new value towards an old value using a change value.
*/
template <typename ntype>
inline ntype n_smooth(ntype& newVal, ntype& curVal, const ntype& maxChange)
{
    ntype diff = newVal - curVal;
    if (n_abs(diff) > maxChange)
    {
        if (diff > NT_ZERO)
        {
            curVal += maxChange;
            if (curVal > newVal)
            {
                curVal = newVal;
            }
        }
        else if (diff < NT_ZERO)
        {
            curVal -= maxChange;
            if (curVal < newVal)
            {
                curVal = newVal;
            }
        }
    }
    else
    {
        curVal = newVal;
    }
    return curVal;
}



//------------------------------------------------------------------------------
/**
    Return a pseudo random number between 0 and 1.
*/
template <typename ntype>
inline ntype n_rand()
{
    return (ntype)rand() / (ntype)RAND_MAX;
}

//------------------------------------------------------------------------------
/**
    Return a pseudo random number between min and max.
*/
template <typename ntype>
inline ntype n_rand(const ntype& _min, const ntype& _max)
{
    ntype unit = n_rand();
    ntype diff = _max - _min;

    return _min + unit * diff;
}

inline long n_ftol(float val)
{    
    return lrintf(val);
}

inline long n_ftol(double val)
{    
    return lrint(val);
}

//------------------------------------------------------------------------------
/**
    Chop float to int.
*/
template <typename ntype>
inline int n_fchop(ntype f)
{    
    return (int)f;
}

//------------------------------------------------------------------------------
/**
    Round float to integer.
*/
template <typename ntype>
inline int n_frnd(ntype f)
{
    return n_fchop(floor(f + (ntype)0.5));
}

//------------------------------------------------------------------------------
/**
    Linearly interpolate between 2 values: ret = x + l * (y - x)
*/
template<class vtype, class ntype>
inline ntype n_lerp(const vtype& a, const vtype& b, const ntype& l)
{
    return a + l * (b - a);
}

//------------------------------------------------------------------------------
/**
*/
template<class ntype>
inline ntype n_fmod(const ntype& x, const ntype& y)
{
    return fmod(x, y);
}

//------------------------------------------------------------------------------
/**
    Normalize an angular value into the range rad(0) to rad(360).
*/
template <typename ntype> 
inline ntype n_normangle(ntype a)
{
    while (a < NT_ZERO)
    {
        a += NT_DOUBLE_PI; //n_deg2rad(360.0f)
    }
    if (a >= NT_DOUBLE_PI) //n_deg2rad(360.0f)
    {
        a = n_fmod(a, NT_DOUBLE_PI);
    }
    return a;
}

//------------------------------------------------------------------------------
/**
    inverse value
*/
template <typename ntype>
inline ntype n_inv(const ntype& f) { return (n_neqz(f) ? NT_ONE / f : f);}

//------------------------------------------------------------------------------
/**
    Get angular distance.
*/
template <typename ntype> 
inline ntype n_angulardistance(ntype from, ntype to)
{
	ntype normFrom = n_normangle(from);
    ntype normTo   = n_normangle(to);
    ntype dist = normTo - normFrom;
    if (dist < -NT_PI) // n_deg2rad(-180.0f)
    {
        dist += NT_DOUBLE_PI; //n_deg2rad(360.0f)
    }
    else if (dist > NT_DOUBLE_PI) //n_deg2rad(180.0f)
    {
        dist -= NT_DOUBLE_PI; //n_deg2rad(360.0f)
    }
    return dist;
}

//-------------------------------------------------------------------
#endif
