#ifndef N_MATRIX_H
#define N_MATRIX_H
//-------------------------------------------------------------------
/**
    @class t_matrix33
    @brief 3x3 matrix
*/
/**
    @class t_matrix44
    @brief 4x4 matrix
*/
//-------------------------------------------------------------------

#include <string.h>

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#ifndef N_EULERANGLES_H
#include "mathlib/eulerangles.h"
#endif

#define M11 m[0][0]
#define M12 m[0][1]
#define M13 m[0][2]
#define M14 m[0][3]
#define M21 m[1][0]
#define M22 m[1][1]
#define M23 m[1][2]
#define M24 m[1][3]
#define M31 m[2][0]
#define M32 m[2][1]
#define M33 m[2][2]
#define M34 m[2][3]
#define M41 m[3][0]
#define M42 m[3][1]
#define M43 m[3][2]
#define M44 m[3][3]

template<typename ntype> class t_matrix33;
template<typename ntype> class t_matrix44;
//static t_matrix33 operator * (const t_matrix33& m0, const t_matrix33& m1);

//-------------------------------------------------------------------
/// 3x3 matrix class
template<typename ntype>
class t_matrix33 
{
public:
	typedef t_vector3<ntype>	t_vec3;
	typedef t_quaternion<ntype> t_quat;

public:
	/// matrix
    ntype m[3][3];

	/// constatnt identity matrix
	N_MATH_PUBLIC static const ntype identity[9];
	N_MATH_PUBLIC static const ntype epsilon;

public:
    //-- constructors -----------------------------------------------
	/// default
    t_matrix33() { this->ident(); }
	/// copy
	t_matrix33(const t_matrix33& m1) { this->set(m1); }
	/// copy
	t_matrix33(const t_matrix44<ntype>& m1) { this->set(m1); }
	/// from axis
    t_matrix33(const t_vec3& v0, const t_vec3& v1, const t_vec3& v2) { this->set(v0, v1, v2); }
	/// from array
    t_matrix33(ntype* p) { this->set(p); }	
	/// from values
    t_matrix33(const ntype& _m11, const ntype& _m12, const ntype& _m13,
             const ntype& _m21, const ntype& _m22, const ntype& _m23,
             const ntype& _m31, const ntype& _m32, const ntype& _m33)
    {
		this->set(_m11, _m12, _m13, _m21, _m22, _m23, _m31, _m32 ,_m33);
    }
	/// from quaternion
	t_matrix33(const t_quat& q) { this->set(q);	}

	//-- set functions -----------------------------------------------
	/// from matrix33
	void set(const t_matrix33& m1) { memcpy(m, m1.m, sizeof(m)); }
    /// from matrix44
	void set(const t_matrix44<ntype>& m1);    
	/// from axis
	void set(const t_vec3& v0, const t_vec3& v1, const t_vec3& v2) 
	{
		memcpy(&M11, v0.v, sizeof(ntype)*3);
		memcpy(&M21, v1.v, sizeof(ntype)*3);
		memcpy(&M31, v2.v, sizeof(ntype)*3);
    }
	/// from array
	void set(ntype* p) { memcpy(m, p, sizeof(m)); }
	/// from values
	void set(const ntype& m11, const ntype& m12, const ntype& m13,
             const ntype& m21, const ntype& m22, const ntype& m23,
             const ntype& m31, const ntype& m32, const ntype& m33) 
    {
        M11=m11; M12=m12; M13=m13;
        M21=m21; M22=m22; M23=m23;
        M31=m31; M32=m32; M33=m33;
    }
	/// from quaternion
    void set(const t_quat& q) 
	{
        ntype xx = q.x*q.x; ntype yy = q.y*q.y; ntype zz = q.z*q.z;
        ntype xy = q.x*q.y; ntype xz = q.x*q.z; ntype yz = q.y*q.z;
        ntype wx = q.w*q.x; ntype wy = q.w*q.y; ntype wz = q.w*q.z;

        m[0][0] = NT_ONE - NT_TWO * (yy + zz);
        m[1][0] =          NT_TWO * (xy - wz);
        m[2][0] =          NT_TWO * (xz + wy);

        m[0][1] =          NT_TWO * (xy + wz);
        m[1][1] = NT_ONE - NT_TWO * (xx + zz);
        m[2][1] =          NT_TWO * (yz - wx);

        m[0][2] =          NT_TWO * (xz - wy);
        m[1][2] =          NT_TWO * (yz + wx);
        m[2][2] = NT_ONE - NT_TWO * (xx + yy);
    }   
	/// make matrix ident
    void ident(void) { memcpy(m, identity, sizeof(identity)); }

    /// get quaternion representation
    t_quat get_quaternion(void) const 
	{
        ntype qa[4];
        ntype tr = m[0][0] + m[1][1] + m[2][2];
        if (tr > NT_ZERO) {
            ntype s = n_sqrt (tr + NT_ONE);
            qa[3] = n_half(s);
            s = NT_ONE / (s * NT_TWO);
            qa[0] = (m[1][2] - m[2][1]) * s;
            qa[1] = (m[2][0] - m[0][2]) * s;
            qa[2] = (m[0][1] - m[1][0]) * s;
        } 
		else 
		{
			static const int nxt[3] = {1, 2, 0};
            int i, j, k;
            i = 0;
            if (m[1][1] > m[0][0]) i=1;
            if (m[2][2] > m[i][i]) i=2;
            j = nxt[i];
            k = nxt[j];
            ntype s = n_sqrt((m[i][i] - (m[j][j] + m[k][k])) + NT_ONE);
            qa[i] = n_half(s);
            s = NT_ONE / (s * NT_TWO);
            qa[3] = (m[j][k] - m[k][j])* s;
            qa[j] = (m[i][j] + m[j][i]) * s;
            qa[k] = (m[i][k] + m[k][i]) * s;
        }
        t_quat q(qa[0],qa[1],qa[2],qa[3]);
        return q;
    }
	/// get Euler representation
    t_vec3 to_euler() const 
	{    
        t_vec3 ea;
        int i,j,k,h,n,s,f;
        EulGetOrd(EulOrdXYZs,i,j,k,h,n,s,f);
        if (s == EulRepYes) {
            ntype sy = n_sqrt(M12*M12 + M13*M13);
			if (sy > (ntype)16*matrix33::epsilon) 
			{
                ea.x = n_atan2(M12,  M13);
                ea.y = n_atan2(sy,	 M11);
                ea.z = n_atan2(M21, -M31);
            } 
			else 
			{
                ea.x = n_atan2(-M23, M22);
                ea.y = n_atan2(	 sy, M11);
                ea.z = 0;
            }
        } 
		else 
		{
            ntype cy = n_sqrt(M11*M11 + M21*M21);
			if (cy > (ntype)16*matrix33::epsilon) {
                ea.x = n_atan2(M32, M33);
                ea.y = n_atan2(-M31, cy);
                ea.z = n_atan2(M21, M11);
            } 
			else 
			{
                ea.x = n_atan2(-M23, M22);
                ea.y = n_atan2(-M31, cy);
                ea.z = 0;
            }
        }
        if (n == EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}
        if (f == EulFrmR) {ntype t = ea.x; ea.x = ea.z; ea.z = t;}

        return ea;
    }
    /// get Euler representation, 2nd approach
 	t_vec3 to_euler2() const 
	{    
    	t_vec3 ea;
    
    	// work on matrix with flipped row/columns
    	t_matrix33 tmp(*this);
    	tmp.transpose();

    	int i,j,k,h,n,s,f;
    	EulGetOrd(EulOrdXYZs,i,j,k,h,n,s,f);
    	if ( s == EulRepYes) {
	        ntype sy = n_sqrt(tmp.M12 * tmp.M12 + tmp.M13 * tmp.M13);
	        if (sy > (ntype)16*matrix33::epsilon) 
	        {
	            ea.x = n_atan2(tmp.M12, tmp.M13);
	            ea.y = n_atan2(sy,		tmp.M11);
	            ea.z = n_atan2(tmp.M21, -tmp.M31);
	        } 
			else 
			{
	            ea.x = n_atan2(-tmp.M23, tmp.M22);
	            ea.y = n_atan2( sy,		 tmp.M11);
	            ea.z = 0;
	        }
	    } 
	    else 
	    {
	        ntype cy = n_sqrt(tmp.M11 * tmp.M11 + tmp.M21 * tmp.M21);
	        if (cy > (ntype)16*matrix33::epsilon) 
	        {
	            ea.x = n_atan2(tmp.M32, tmp.M33);
	            ea.y = n_atan2(-tmp.M31, (ntype)cy);
	            ea.z = n_atan2(tmp.M21, tmp.M11);
	        } 
	        else 
	        {
	            ea.x = n_atan2(-tmp.M23, tmp.M22);
	            ea.y = n_atan2(-tmp.M31, (ntype)cy);
	            ea.z = 0;
	        }
	    }
	    if (n == EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}
	    if (f == EulFrmR) {ntype t = ea.x; ea.x = ea.z; ea.z = t;}
	
	    return ea;
	}
	
	/// create matrix from Euler angles
    void from_euler(const t_vec3& ea) 
	{
        t_vec3 tea = ea;
        ntype ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
        int i,j,k,h,n,s,f;
        EulGetOrd(EulOrdXYZs, i, j, k, h, n, s, f);
        if (f == EulFrmR) {ntype t = ea.x; tea.x = ea.z; tea.z = t;}
        if (n == EulParOdd) {tea.x = -ea.x; tea.y = -ea.y; tea.z = -ea.z;}
        ti = tea.x;   tj = tea.y;   th = tea.z;
        ci = n_cos(ti); cj = n_cos(tj); ch = n_cos(th);
        si = n_sin(ti); sj = n_sin(tj); sh = n_sin(th);
        cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
        if (s == EulRepYes) {
            M11 = (cj);     M12 = (sj*si);     M13 = (sj*ci);
            M21 = (sj*sh);  M22 = (-cj*ss+cc); M23 = (-cj*cs-sc);
            M31 = (-sj*ch); M23 = ( cj*sc+cs); M33 = ( cj*cc-ss);
        } 
		else 
		{
            M11 = (cj*ch); M12 = (sj*sc-cs); M13 = (sj*cc+ss);
            M21 = (cj*sh); M22 = (sj*ss+cc); M23 = (sj*cs-sc);
            M31 = (-sj);   M32 = (cj*si);    M33 = (cj*ci);
        }
    }

    /**
        @brief free lookat function 

    @param from Position of viewpoint.
    @param to   Position to look at.
    @param up   Up vector.
    */
    void lookat(const t_vec3& from, const t_vec3& to, const t_vec3& up) {

        t_vec3 z(from - to);
        z.norm();
        t_vec3 y(up);
        t_vec3 x(y * z);   // x = y cross z
        y = z * x;      // y = z cross x
        x.norm();
        y.norm();

        M11=x.x;  M12=x.y;  M13=x.z;  
        M21=y.x;  M22=y.y;  M23=y.z;  
        M31=z.x;  M32=z.y;  M33=z.z;  
    }

    /**
        @brief restricted lookat function

    @param from Position of viewpoint.
    @param to   Position to look at.
    @param up   Up vector.
    */
    void billboard(const t_vec3& from, const t_vec3& to, const t_vec3& up)
    {
        t_vec3 z(from - to);
        z.norm();
        t_vec3 y(up);
        t_vec3 x(y * z);
        z = x * y;       
        x.norm();
        y.norm();
        z.norm();

        M11=x.x;  M12=x.y;  M13=x.z;  
        M21=y.x;  M22=y.y;  M23=y.z;  
        M31=z.x;  M32=z.y;  M33=z.z;  
    }

    //-- misc -------------------------------------------------------
    /// transpose matrix
    void transpose(void) {        
        n_swap(m[0][1], m[1][0]);
        n_swap(m[0][2], m[2][0]);
        n_swap(m[1][2], m[2][1]);
    }

	/// check if matrix is orthonormal
    bool orthonorm(const ntype& limit) 
	{
        if (((M11*M21+M12*M22+M13*M23)<limit) &&
            ((M11*M31+M12*M32+M13*M33)<limit) &&
            ((M31*M21+M32*M22+M33*M23)<limit) &&
            ((M11*M11+M12*M12+M13*M13)>(NT_ONE-limit)) &&
            ((M11*M11+M12*M12+M13*M13)<(NT_ONE+limit)) &&
            ((M21*M21+M22*M22+M23*M23)>(NT_ONE-limit)) &&
            ((M21*M21+M22*M22+M23*M23)<(NT_ONE+limit)) &&
            ((M31*M31+M32*M32+M33*M33)>(NT_ONE-limit)) &&
            ((M31*M31+M32*M32+M33*M33)<(NT_ONE+limit)))
            return true;
        else
            return false;
    }

	/// scale x, y, z axis 	
    void scale(const ntype& x, const ntype& y, const ntype& z) 
	{        
        for (int i = 0; i < 3; i++) 
		{
            m[i][0] *= x;
            m[i][1] *= y;
            m[i][2] *= z;
        }
    }

    /// rotate about global x
    void rotate_x(const ntype& a)
    {
        ntype c = n_cos(a);
        ntype s = n_sin(a);
        for (int i = 0; i < 3; i++) 
        {
            ntype mi1 = m[i][1];
            ntype mi2 = m[i][2];
            m[i][1] = mi1*c + mi2*-s;
            m[i][2] = mi1*s + mi2*c;
        }
    }

    /// rotates matrix about global y
    void rotate_y(const ntype& a)
    {
        ntype c = n_cos(a);
        ntype s = n_sin(a);
        for (int i = 0; i < 3; i++) 
        {
            ntype mi0 = m[i][0];
            ntype mi2 = m[i][2];
            m[i][0] = mi0*c + mi2*s;
            m[i][2] = mi0*-s + mi2*c;
        }
    }

    /// rotate about global z
    void rotate_z(const ntype& a)
    {
        ntype c = n_cos(a);
        ntype s = n_sin(a);
        for (int i = 0; i < 3; i++) 
        {
            ntype mi0 = m[i][0];
            ntype mi1 = m[i][1];
            m[i][0] = mi0*c + mi1*-s;
            m[i][1] = mi0*s + mi1*c;
        }
    }
    
    /// rotate about local x (not very fast)
    void rotate_local_x(const ntype& a)
    {
        t_matrix33 rotM;  // initialized as identity matrix
        rotM.M22 = n_cos(a); rotM.M23 = -n_sin(a);
        rotM.M32 = n_sin(a); rotM.M33 =  n_cos(a);

        (*this) = rotM * (*this); 
    }

    /// rotate about local y (not very fast)
    void rotate_local_y(const ntype& a)
    {
        t_matrix33 rotM;  // initialized as identity matrix
        rotM.M11 =  n_cos(a); rotM.M13 =  n_sin(a);
        rotM.M31 = -n_sin(a); rotM.M33 =  n_cos(a);

        (*this) = rotM * (*this); 
    }

    /// rotate about local z (not very fast)
    void rotate_local_z(const ntype& a)
    {
        t_matrix33 rotM;  // initialized as identity matrix
        rotM.M11 =  n_cos(a); rotM.M12 = - n_sin(a);
        rotM.M21 =  n_sin(a); rotM.M22 =   n_cos(a);

        (*this) = rotM * (*this); 
    }

    /// rotate about any axis
    void rotate(const t_vec3& vec, const ntype& a)
    {
        t_vec3 v(vec);
        v.norm();
        ntype sa = n_sin(a);
        ntype ca = n_cos(a);

        t_matrix33 rotM;
        rotM.M11 = ca + (NT_ONE - ca) * v.x * v.x;
        rotM.M12 = (NT_ONE - ca) * v.x * v.y - sa * v.z;
        rotM.M13 = (NT_ONE - ca) * v.z * v.x + sa * v.y;
        rotM.M21 = (NT_ONE - ca) * v.x * v.y + sa * v.z;
        rotM.M22 = ca + (NT_ONE - ca) * v.y * v.y;
        rotM.M23 = (NT_ONE - ca) * v.y * v.z - sa * v.x;
        rotM.M31 = (NT_ONE - ca) * v.z * v.x - sa * v.y;
        rotM.M32 = (NT_ONE - ca) * v.y * v.z + sa * v.x;
        rotM.M33 = ca + (NT_ONE - ca) * v.z * v.z;
        
        (*this) = (*this) * rotM;
    }

    /// get x component as t_vec3
    t_vec3 x_component(void) const { return t_vec3(M11, M12, M13); }

    /// get y component as t_vec3
	t_vec3 y_component(void) const { return t_vec3(M21, M22, M23); }

    /// get z component as t_vec3
    t_vec3 z_component(void) const { return t_vec3(M31, M32, M33); }

    /// get scale as t_vec3
    t_vec3 get_scale(void) const 
    {
        return t_vec3(
			ntype(this->x_component()), 
			ntype(this->y_component()),
			ntype(this->z_component())
		);
    }

    /// compute matrix determinant
    ntype t_matrix33::det()
    {
        return ( M11 * ( M22 * M33 - M23 * M32 ) -
                 M12 * ( M21 * M33 - M23 * M31 ) +
                 M13 * ( M21 * M32 - M22 * M31 ) );
    }
    /// t_matrix33 inversion
    void t_matrix33::invert()
    {        
        t_matrix33 H;
        
        H.M11 =   M22 * M33 - M23 * M32;
        H.M12 = -(M21 * M33 - M23 * M31);
        H.M13 =   M21 * M32 - M22 * M31;
        H.M21 = -(M12 * M33 - M13 * M32);
        H.M22 =   M11 * M33 - M13 * M31;
        H.M23 = -(M11 * M32 - M12 * M31);
        H.M31 =   M12 * M23 - M13 * M22;
        H.M32 = -(M11 * M23 - M13 * M21);
        H.M33 =   M11 * M22 - M12 * M21;

        H.transpose();

        ntype d = det();
        if (n_neqz(d)) 
		{
            d = NT_ONE / d;
            M11 = H.M11 * d;
            M12 = H.M12 * d;
            M13 = H.M13 * d;
            M21 = H.M21 * d;
            M22 = H.M22 * d;
            M23 = H.M23 * d;
            M31 = H.M31 * d;
            M32 = H.M32 * d;
            M33 = H.M33 * d;
		}
    }

    //-- operators --------------------------------------------------
	/// self multiply
    t_matrix33& operator *= (const t_matrix33& m1) 
	{        
        for (int i = 0; i < 3; i++) 
		{
            ntype mi0 = m[i][0];
            ntype mi1 = m[i][1];
            ntype mi2 = m[i][2];
            m[i][0] = mi0*m1.m[0][0] + mi1*m1.m[1][0] + mi2*m1.m[2][0];
            m[i][1] = mi0*m1.m[0][1] + mi1*m1.m[1][1] + mi2*m1.m[2][1];
            m[i][2] = mi0*m1.m[0][2] + mi1*m1.m[1][2] + mi2*m1.m[2][2];
        }
        return *this;
    }

	//-------------------------------------------------------------------
	/// multiply
	t_matrix33 operator * (const t_matrix33& m1) const {
		return t_matrix33(
			m[0][0]*m1.m[0][0] + m[0][1]*m1.m[1][0] + m[0][2]*m1.m[2][0],
			m[0][0]*m1.m[0][1] + m[0][1]*m1.m[1][1] + m[0][2]*m1.m[2][1],
			m[0][0]*m1.m[0][2] + m[0][1]*m1.m[1][2] + m[0][2]*m1.m[2][2],

			m[1][0]*m1.m[0][0] + m[1][1]*m1.m[1][0] + m[1][2]*m1.m[2][0],
			m[1][0]*m1.m[0][1] + m[1][1]*m1.m[1][1] + m[1][2]*m1.m[2][1],
			m[1][0]*m1.m[0][2] + m[1][1]*m1.m[1][2] + m[1][2]*m1.m[2][2],

			m[2][0]*m1.m[0][0] + m[2][1]*m1.m[1][0] + m[2][2]*m1.m[2][0],
			m[2][0]*m1.m[0][1] + m[2][1]*m1.m[1][1] + m[2][2]*m1.m[2][1],
			m[2][0]*m1.m[0][2] + m[2][1]*m1.m[1][2] + m[2][2]*m1.m[2][2]
		);		
	}

	//-------------------------------------------------------------------
	/// multiply vector
	t_vec3 operator * (const t_vec3& v) const
	{
		return t_vec3(
			M11*v.x + M21*v.y + M31*v.z,
			M12*v.x + M22*v.y + M32*v.z,
			M13*v.x + M23*v.y + M33*v.z
		);
	}

    /// comparison with threshold
    bool fuz_eq(const t_matrix33& o, const ntype& t = NT_TOL) const 
	{
        for (int i = 0; i < 9; i++)		
			if (n_neq(o.m[i], m[i], t)) return false;
        return true;
    }
    /// multiply source vector with matrix and store in destination vector
    /// this eliminates the construction of a temp t_vec3 object
    void mult(const t_vec3& src, t_vec3& dst)
    {
        dst.x = M11*src.x + M21*src.y + M31*src.z;
        dst.y = M12*src.x + M22*src.y + M32*src.z;
        dst.z = M13*src.x + M23*src.y + M33*src.z;
    }
	/// check for identity
	bool isident() const
	{
		return memcmp(this->m, identity, sizeof(identity)) == 0;
	}
	/// check for orthogonality
	bool isorthogonal(ntype tol = N_TOL) const
	{
		t_vec3 x(this->x_component());
		t_vec3 y(this->y_component());
		t_vec3 z(this->z_component());
		x.norm();
		y.norm();
		z.norm();
		return  (n_eqz(x%y, tol) && n_eqz(x%z, tol) && n_eqz(y%z, tol));
	}
	/// check for lefthand
	bool islefthand() const
	{
		t_vec3 z(this->z_component());
		t_vec3 dir(this->y_component()*this->x_component());		
		z.norm();
		dir.norm();

		return n_eq(dir%z, -NT_ONE);
	}
};


//-------------------------------------------------------------------
template<typename ntype>
class t_matrix44 
{
public:
	typedef t_vector3<ntype>	t_vec3;
	typedef t_vector4<ntype>	t_vec4;
	typedef t_quaternion<ntype> t_quat;

public:
    ntype m[4][4];

	N_MATH_PUBLIC static const ntype identity[16];
	N_MATH_PUBLIC static const ntype epsilon;

public:
    //-- constructors -----------------------------------------------
	/// default
    t_matrix44() { this->ident(); }	
	/// copy
	t_matrix44(const t_matrix44& m1) { this->set(m1); }	
	/// from axis
	t_matrix44(const t_vec4& v0, const t_vec4& v1, const t_vec4& v2, const t_vec4& v3) 
	{ 
		this->set(v0, v1, v2, v3); 
	}
	/// from array
    t_matrix44(ntype* p) { this->set(p); }	
	/// from values
	t_matrix44(const ntype& _m11, const ntype& _m12, const ntype& _m13, const ntype& _m14,
             const ntype& _m21, const ntype& _m22, const ntype& _m23, const ntype& _m24,
             const ntype& _m31, const ntype& _m32, const ntype& _m33, const ntype& _m34,
             const ntype& _m41, const ntype& _m42, const ntype& _m43, const ntype& _m44)   
    
    {
		this->set(_m11, _m12, _m13, _m14, 
				  _m21, _m22, _m23, _m24, 
				  _m31, _m32 ,_m33, _m34, 
				  _m41, _m42 ,_m43, _m44);
    }
	/// from quaternion
	t_matrix44(const t_quat& q) { this->set(q); }    
	/// from matrix 3x3
    t_matrix44(const t_matrix33<ntype>& m1) 
	{
        set( m1 );
        M41 = M42 = M43 = NT_ZERO;  M44 = NT_ONE; 
    }        
	/// like glLookAt, point of viewer/camera, sight direction and up vector
    t_matrix44(const t_vec3& from, const t_vec3& to, const t_vec3& up ) 
	{
        t_vec3 z(from - to);
        z.norm();
        t_vec3 y(up);
        t_vec3 x(y * z);   // x = y cross z
        y = z * x;      // y = z cross x
        x.norm();
        y.norm();

        M11=x.x;  M12=x.y;  M13=x.z;  M14=NT_ZERO;
        M21=y.x;  M22=y.y;  M23=y.z;  M24=NT_ZERO;
        M31=z.x;  M32=z.y;  M33=z.z;  M34=NT_ZERO;    
        M41 = from.x ; M42 = from.y ; M43 = from.z ; M44=NT_ONE; 
    }

	//-- misc -------------------------------------------------------
	/// copy
	void set(const t_matrix44& m1) { memcpy(m, m1.m, sizeof(m)); }
	/// from axis
    void set(const t_vec4& v0, const t_vec4& v1, const t_vec4& v2, const t_vec4& v3) 
	{
		memcpy(&M11, v0.v, sizeof(ntype)*4);
		memcpy(&M21, v1.v, sizeof(ntype)*4);
		memcpy(&M31, v2.v, sizeof(ntype)*4);
		memcpy(&M41, v3.v, sizeof(ntype)*4);
    }    
	/// from array
    void set(ntype* p) { memcpy(m, p, sizeof(m)); }	
	/// from values
    void set(const ntype& _m11, const ntype& _m12, const ntype& _m13, const ntype& _m14,
             const ntype& _m21, const ntype& _m22, const ntype& _m23, const ntype& _m24,
             const ntype& _m31, const ntype& _m32, const ntype& _m33, const ntype& _m34,
             const ntype& _m41, const ntype& _m42, const ntype& _m43, const ntype& _m44)
    {
        M11=_m11; M12=_m12; M13=_m13; M14=_m14;
        M21=_m21; M22=_m22; M23=_m23; M24=_m24;
        M31=_m31; M32=_m32; M33=_m33; M34=_m34;
        M41=_m41; M42=_m42; M43=_m43; M44=_m44;
    }
	/// from quaternion
    void set(const t_quat& q) 
	{
		float xx = q.x*q.x; float yy = q.y*q.y; float zz = q.z*q.z;
        float xy = q.x*q.y; float xz = q.x*q.z; float yz = q.y*q.z;
        float wx = q.w*q.x; float wy = q.w*q.y; float wz = q.w*q.z;

        m[0][0] = NT_ONE -	NT_TWO * (yy + zz);
        m[0][1] =			NT_TWO * (xy - wz);
        m[0][2] =			NT_TWO * (xz + wy);

        m[1][0] =			NT_TWO * (xy + wz);
        m[1][1] = NT_ONE -	NT_TWO * (xx + zz);
        m[1][2] =			NT_TWO * (yz - wx);

        m[2][0] =			NT_TWO * (xz - wy);
        m[2][1] =			NT_TWO * (yz + wx);
        m[2][2] = NT_ONE -	NT_TWO * (xx + yy);

        m[0][3] = m[1][3] = m[2][3] = NT_ZERO;
        m[3][0] = m[3][1] = m[3][2] = NT_ZERO;
        m[3][3] = NT_ONE;		
    }
	/// from matrix 3x3
    void set(const t_matrix33<ntype>& m1) { //Add by Clockwise - Zeev Hadar
        for (int i = 0; i < 3; i++ )   {
            memcpy(m[i], m1.m[i], 3*sizeof(ntype));
            m[i][3] = 0;
        }
    }	

    /**
        @brief convert orientation of 4x4 matrix into quaternion, 
        4x4 matrix may not be scaled!
    */
    t_quat get_quaternion(void) const 
	{
        ntype qa[4];
        ntype tr = m[0][0] + m[1][1] + m[2][2];
        if (tr > NT_ZERO) {
            ntype s = n_sqrt (tr + NT_ONE);
            qa[3] = n_half(s);
            s = NT_ONE / (s * NT_TWO);
            qa[0] = (m[1][2] - m[2][1]) * s;
            qa[1] = (m[2][0] - m[0][2]) * s;
            qa[2] = (m[0][1] - m[1][0]) * s;
        } 
		else 
		{
			static const int nxt[3] = {1,2,0};
            int i, j, k; 
            i = 0;
            if (m[1][1] > m[0][0]) i=1;
            if (m[2][2] > m[i][i]) i=2;
            j = nxt[i];
            k = nxt[j];
            ntype s = n_sqrt((m[i][i] - (m[j][j] + m[k][k])) + NT_ONE);
            qa[i] = n_half(s);
            s = NT_ONE / (s * NT_TWO);
            qa[3] = (m[j][k] - m[k][j])* s;
            qa[j] = (m[i][j] + m[j][i]) * s;
            qa[k] = (m[i][k] + m[k][i]) * s;
        }
        t_quat q(qa[0],qa[1],qa[2],qa[3]);
        return q;
    }
	/// ident matrix
    void ident(void) { memcpy(m, identity, sizeof(identity)); }
	/// transpose
    void transpose(void) 
	{
        n_swap(M12,M21);
        n_swap(M13,M31);
        n_swap(M14,M41);
        n_swap(M23,M32);
        n_swap(M24,M42);
        n_swap(M34,M43);
    }
	/// determinant
    ntype det(void) const 
	{
        return
            (M11*M22 - M12*M21)*(M33*M44 - M34*M43)
           -(M11*M23 - M13*M21)*(M32*M44 - M34*M42)
           +(M11*M24 - M14*M21)*(M32*M43 - M33*M42)
           +(M12*M23 - M13*M22)*(M31*M44 - M34*M41)
           -(M12*M24 - M14*M22)*(M31*M43 - M33*M41)
           +(M13*M24 - M14*M23)*(M31*M42 - M32*M41);
    }
	/// invert matrix
    void invert(void) {
        ntype s = det();
        if (n_eqz(s)) return;
        s = NT_ONE/s;
        this->set(
            s*(M22*(M33*M44 - M34*M43) + M23*(M34*M42 - M32*M44) + M24*(M32*M43 - M33*M42)),
            s*(M32*(M13*M44 - M14*M43) + M33*(M14*M42 - M12*M44) + M34*(M12*M43 - M13*M42)),
            s*(M42*(M13*M24 - M14*M23) + M43*(M14*M22 - M12*M24) + M44*(M12*M23 - M13*M22)),
            s*(M12*(M24*M33 - M23*M34) + M13*(M22*M34 - M24*M32) + M14*(M23*M32 - M22*M33)),
            s*(M23*(M31*M44 - M34*M41) + M24*(M33*M41 - M31*M43) + M21*(M34*M43 - M33*M44)),
            s*(M33*(M11*M44 - M14*M41) + M34*(M13*M41 - M11*M43) + M31*(M14*M43 - M13*M44)),
            s*(M43*(M11*M24 - M14*M21) + M44*(M13*M21 - M11*M23) + M41*(M14*M23 - M13*M24)),
            s*(M13*(M24*M31 - M21*M34) + M14*(M21*M33 - M23*M31) + M11*(M23*M34 - M24*M33)),
            s*(M24*(M31*M42 - M32*M41) + M21*(M32*M44 - M34*M42) + M22*(M34*M41 - M31*M44)),
            s*(M34*(M11*M42 - M12*M41) + M31*(M12*M44 - M14*M42) + M32*(M14*M41 - M11*M44)),
            s*(M44*(M11*M22 - M12*M21) + M41*(M12*M24 - M14*M22) + M42*(M14*M21 - M11*M24)),
            s*(M14*(M22*M31 - M21*M32) + M11*(M24*M32 - M22*M34) + M12*(M21*M34 - M24*M31)),
            s*(M21*(M33*M42 - M32*M43) + M22*(M31*M43 - M33*M41) + M23*(M32*M41 - M31*M42)),
            s*(M31*(M13*M42 - M12*M43) + M32*(M11*M43 - M13*M41) + M33*(M12*M41 - M11*M42)),
            s*(M41*(M13*M22 - M12*M23) + M42*(M11*M23 - M13*M21) + M43*(M12*M21 - M11*M22)),
            s*(M11*(M22*M33 - M23*M32) + M12*(M23*M31 - M21*M33) + M13*(M21*M32 - M22*M31)));
    }

    /**
        @brief inverts a 4x4 matrix consisting of a 3x3 rotation matrix and
        a translation (eg. everything that has [0,0,0,1] as the rightmost
    column) MUCH cheaper then a real 4x4 inversion
    */
    void invert_simple(void) 
	{

        ntype s = det();
        if (n_eqz(s)) return;
        s = NT_ONE/s;
        this->set(
            s * ((M22 * M33) - (M23 * M32)),
            s * ((M32 * M13) - (M33 * M12)),
            s * ((M12 * M23) - (M13 * M22)),
            NT_ZERO,
            s * ((M23 * M31) - (M21 * M33)),
            s * ((M33 * M11) - (M31 * M13)),
            s * ((M13 * M21) - (M11 * M23)),
            NT_ZERO,
            s * ((M21 * M32) - (M22 * M31)),
            s * ((M31 * M12) - (M32 * M11)),
            s * ((M11 * M22) - (M12 * M21)),
            NT_ZERO,
            s * (M21*(M33*M42 - M32*M43) + M22*(M31*M43 - M33*M41) + M23*(M32*M41 - M31*M42)),
            s * (M31*(M13*M42 - M12*M43) + M32*(M11*M43 - M13*M41) + M33*(M12*M41 - M11*M42)),
            s * (M41*(M13*M22 - M12*M23) + M42*(M11*M23 - M13*M21) + M43*(M12*M21 - M11*M22)),
            NT_ONE);
    }

    /// optimized multiplication, assumes that M14==M24==M34==0 AND M44==1
    void mult_simple(const t_matrix44& m1) 
	{        
        for (int i = 0; i < 4; i++) {
            ntype mi0 = m[i][0];
            ntype mi1 = m[i][1];
            ntype mi2 = m[i][2];
            m[i][0] = mi0*m1.m[0][0] + mi1*m1.m[1][0] + mi2*m1.m[2][0];
            m[i][1] = mi0*m1.m[0][1] + mi1*m1.m[1][1] + mi2*m1.m[2][1];
            m[i][2] = mi0*m1.m[0][2] + mi1*m1.m[1][2] + mi2*m1.m[2][2];
        }
        m[3][0] += m1.m[3][0];
        m[3][1] += m1.m[3][1];
        m[3][2] += m1.m[3][2];
        m[0][3] = NT_ZERO;
        m[1][3] = NT_ZERO;
        m[2][3] = NT_ZERO;
        m[3][3] = NT_ONE;
    }
    
    /// Transforms a vector by the matrix, projecting the result back into w=1.
    t_vec3 transform_coord(const t_vec3 v) const
    {
        ntype d = NT_ONE / (M14*v.x + M24*v.y + M34*v.z + M44);
        return t_vec3(
            (M11*v.x + M21*v.y + M31*v.z + M41) * d,
            (M12*v.x + M22*v.y + M32*v.z + M42) * d,
            (M13*v.x + M23*v.y + M33*v.z + M43) * d);
    }           

    /// return the x component as 3d vector
    t_vec3 x_component(void) const { return t_vec3(M11, M12, M13); }
    /// return the y component as 3d vector
    t_vec3 y_component(void) const { return t_vec3(M21, M22, M23); }
    /// return the z component as 3d vector
    t_vec3 z_component(void) const { return t_vec3(M31, M32, M33); }
    /// return the position component as 3d vector
    t_vec3 pos_component(void) const { return t_vec3(M41,M42,M43); }
    /// return the position component as 3d vector
    t_vec3 get_scale(void) const 
    {
        return t_vec3(
			ntype(this->x_component()),
			ntype(this->y_component()),
			ntype(this->z_component())
			);
    }

    /// Rotate about the global X axis.
    void rotate_x(const ntype& a) 
	{
        ntype c = n_cos(a);
        ntype s = n_sin(a);        
        for (int i = 0; i < 4; i++) {
            ntype mi1 = m[i][1];
            ntype mi2 = m[i][2];
            m[i][1] = mi1*c + mi2*-s;
            m[i][2] = mi1*s + mi2*c;
        }
    }
    /// Rotate about the global Y axis.
    void rotate_y(const ntype a) 
	{
        ntype c = n_cos(a);
        ntype s = n_sin(a);        
        for (int i = 0; i < 4; i++) {
            ntype mi0 = m[i][0];
            ntype mi2 = m[i][2];
            m[i][0] = mi0*c + mi2*s;
            m[i][2] = mi0*-s + mi2*c;
        }
    }
    /// Rotate about the global Z axis.
    void rotate_z(const ntype a) 
	{
        ntype c = n_cos(a);
        ntype s = n_sin(a);    
        for (int i = 0; i < 4; i++) {
            ntype mi0 = m[i][0];
            ntype mi1 = m[i][1];
            m[i][0] = mi0*c + mi1*-s;
            m[i][1] = mi0*s + mi1*c;
        }
    }
    void set_translation(const t_vec3& t) 
	{
        M41 = t.x;
        M42 = t.y;
        M43 = t.z;
    }

    void set_translation(const ntype& x, const ntype& y, const ntype& z) {
        M41 = x;
        M42 = y;
        M43 = z;
    }

    void translate(const t_vec3& t) {
        M41 += t.x;
        M42 += t.y;
        M43 += t.z;
    }
    void translate(const ntype& x, const ntype& y, const ntype& z) {
        M41 += x;
        M42 += y;
        M43 += z;
    }
    void scale(const t_vec3& s) 
	{        
        for (int i = 0; i < 4; i++) 
		{
            m[i][0] *= s.x;
            m[i][1] *= s.y;
            m[i][2] *= s.z;
        }
    }
    void scale(const ntype& x, const ntype& y, const ntype& z) 
	{
        for (int i = 0; i < 4; i++) 
		{
            m[i][0] *= x;
            m[i][1] *= y;
            m[i][2] *= z;
        }
    }

    /**
        @brief free lookat function

    @param to Position to look at.
    @param up Up vector.
    */
    void lookat(const t_vec3& to, const t_vec3& up)
	{

        t_vec3 from(M41, M42, M43);
        t_vec3 z(from - to);
        z.norm();
        t_vec3 y(up);
        t_vec3 x(y * z);   // x = y cross z
        y = z * x;      // y = z cross x
        x.norm();
        y.norm();

        M11 = x.x;  M12 = x.y;  M13 = x.z;  M14 = NT_ZERO;
        M21 = y.x;  M22 = y.y;  M23 = y.z;  M24 = NT_ZERO;
        M31 = z.x;  M32 = z.y;  M33 = z.z;  M34 = NT_ZERO;
    }

    /**
        @brief restricted lookat function

    @param to Position to look at.
    @param up Up vector.
    */
    void billboard(const t_vec3& to, const t_vec3& up)
    {
        t_vec3 from(M41, M42, M43);
        t_vec3 z(from - to);
        z.norm();
        t_vec3 y(up);
        t_vec3 x(y * z);
        z = x * y;       
        x.norm();
        y.norm();
        z.norm();

        M11 = x.x;  M12 = x.y;  M13 = x.z;  M14 = NT_ZERO;
        M21 = y.x;  M22 = y.y;  M23 = y.z;  M24 = NT_ZERO;
        M31 = z.x;  M32 = z.y;  M33 = z.z;  M34 = NT_ZERO;
    }

    //-- operators --------------------------------------------------
    t_matrix44& operator *= (const t_matrix44& m1)
	{        
        for (int i = 0; i < 4; i++) {
            ntype mi0 = m[i][0];
            ntype mi1 = m[i][1];
            ntype mi2 = m[i][2];
            ntype mi3 = m[i][3];
            m[i][0] = mi0*m1.m[0][0] + mi1*m1.m[1][0] + mi2*m1.m[2][0] + mi3*m1.m[3][0];
            m[i][1] = mi0*m1.m[0][1] + mi1*m1.m[1][1] + mi2*m1.m[2][1] + mi3*m1.m[3][1];
            m[i][2] = mi0*m1.m[0][2] + mi1*m1.m[1][2] + mi2*m1.m[2][2] + mi3*m1.m[3][2];
            m[i][3] = mi0*m1.m[0][3] + mi1*m1.m[1][3] + mi2*m1.m[2][3] + mi3*m1.m[3][3];
        }
        return *this;
    }

	//-------------------------------------------------------------------
	t_matrix44 operator * (const t_matrix44& m1) const 
	{
		return t_matrix44(
			m[0][0]*m1.m[0][0] + m[0][1]*m1.m[1][0] + m[0][2]*m1.m[2][0] + m[0][3]*m1.m[3][0],
			m[0][0]*m1.m[0][1] + m[0][1]*m1.m[1][1] + m[0][2]*m1.m[2][1] + m[0][3]*m1.m[3][1],
			m[0][0]*m1.m[0][2] + m[0][1]*m1.m[1][2] + m[0][2]*m1.m[2][2] + m[0][3]*m1.m[3][2],
			m[0][0]*m1.m[0][3] + m[0][1]*m1.m[1][3] + m[0][2]*m1.m[2][3] + m[0][3]*m1.m[3][3],

			m[1][0]*m1.m[0][0] + m[1][1]*m1.m[1][0] + m[1][2]*m1.m[2][0] + m[1][3]*m1.m[3][0],
			m[1][0]*m1.m[0][1] + m[1][1]*m1.m[1][1] + m[1][2]*m1.m[2][1] + m[1][3]*m1.m[3][1],
			m[1][0]*m1.m[0][2] + m[1][1]*m1.m[1][2] + m[1][2]*m1.m[2][2] + m[1][3]*m1.m[3][2],
			m[1][0]*m1.m[0][3] + m[1][1]*m1.m[1][3] + m[1][2]*m1.m[2][3] + m[1][3]*m1.m[3][3],

			m[2][0]*m1.m[0][0] + m[2][1]*m1.m[1][0] + m[2][2]*m1.m[2][0] + m[2][3]*m1.m[3][0],
			m[2][0]*m1.m[0][1] + m[2][1]*m1.m[1][1] + m[2][2]*m1.m[2][1] + m[2][3]*m1.m[3][1],
			m[2][0]*m1.m[0][2] + m[2][1]*m1.m[1][2] + m[2][2]*m1.m[2][2] + m[2][3]*m1.m[3][2],
			m[2][0]*m1.m[0][3] + m[2][1]*m1.m[1][3] + m[2][2]*m1.m[2][3] + m[2][3]*m1.m[3][3],

			m[3][0]*m1.m[0][0] + m[3][1]*m1.m[1][0] + m[3][2]*m1.m[2][0] + m[3][3]*m1.m[3][0],
			m[3][0]*m1.m[0][1] + m[3][1]*m1.m[1][1] + m[3][2]*m1.m[2][1] + m[3][3]*m1.m[3][1],
			m[3][0]*m1.m[0][2] + m[3][1]*m1.m[1][2] + m[3][2]*m1.m[2][2] + m[3][3]*m1.m[3][2],
			m[3][0]*m1.m[0][3] + m[3][1]*m1.m[1][3] + m[3][2]*m1.m[2][3] + m[3][3]*m1.m[3][3]);
	}
	//-------------------------------------------------------------------
	t_vec3 operator * (const t_vec3& v) const
	{
		return t_vec3(
			M11*v.x + M21*v.y + M31*v.z + M41,
			M12*v.x + M22*v.y + M32*v.z + M42,
			M13*v.x + M23*v.y + M33*v.z + M43);
	}
	//-------------------------------------------------------------------
	t_vec4 operator * (const t_vec4& v) const
	{
		return t_vec4(
			M11*v.x + M21*v.y + M31*v.z + M41*v.w,
			M12*v.x + M22*v.y + M32*v.z + M42*v.w,
			M13*v.x + M23*v.y + M33*v.z + M43*v.w,
			M14*v.x + M24*v.y + M34*v.z + M44*v.w);
	}


	void normalize() {		
		for (register int i = 0; i < 4; i++ )	
			for (register int j = 0; j < 4; j++ )	
				m[i][j] /= m[3][3];	
	}

	/// comparison with threshold
    bool fuz_eq(const t_matrix44& other, const ntype& t) const 
	{
		for (int i = 0; i < 16; i++)		
			if (n_neq(o.m[i], m[i], t)) return false;
        return true;
    }

	bool isident() const
	{
		return memcmp(this->m, identity, sizeof(identity)) == 0;
	}

	bool isorthogonal(ntype tol=N_TOL) const
	{
		t_vec3 x(this->x_component());
		t_vec3 y(this->y_component());
		t_vec3 z(this->z_component());
		x.norm();
		y.norm();
		z.norm();
		return  (n_eqz(x%y, tol) && n_eqz(x%z, tol) && n_eqz(y%z, tol));
	}

	bool islefthand() const
	{
		t_vec3 z(this->z_component());
		t_vec3 dir(this->y_component()*this->x_component());		
		z.norm();
		dir.norm();

		return n_eq(dir%z, -NT_ONE);
	}
};

//-------------------------------------------------------------------
template<typename ntype> 
inline void t_matrix33<ntype>::set(const t_matrix44<ntype>& m1) {
    for ( int i = 0; i < 3; i++ )
        memcpy(m[i], &(m1.m[i][0]), 3*sizeof(ntype));
}
//-------------------------------------------------------------------

typedef t_matrix33<nReal> matrix33;
typedef t_matrix44<nReal> matrix44;
#endif
