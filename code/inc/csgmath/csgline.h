#ifndef N_CSGLINE_H
#define N_CSGLINE_H
#include "mathlib/line.h"

/**
    @class csgline3
    @brief expands line3 for a CSG purposes
    
*/
//-------------------------------------------------------------------
class csgline3 : public line3 
{
public:    
	
	csgline3() : line3() {}
	csgline3(const vector3& v0, const vector3& v1) : line3(v0, v1) {}
	csgline3(const line3& l) : line3(l) {}
    csgline3(const csgline3& l)  { this->set(l.va(), l.vb()); }

	bool samesegment(const line3& l) {
		if (this->va() == l.va() && this->vb() == l.vb())
			return true;
		else if (this->vb() == l.va() && this->va() == l.vb())
			return true;
		else return false;
	}

	/**
	Calculate the segment dl that is the shortest route between
	two lines this and l. 
	Return: 
		-1 if no solution exists.
		0 if no segment intersection
		1 if segments intersects
	*/
	int distance(const line3& l, line3& dl) const {
		//vector3 p1, vector3 p2, vector3 p3, vector3 p4,vector3& pa, vector3& pb) {   
		vector3 v1(this->dir());
		vector3 v2(l.dir());
		vector3 vd(this->va() - l.va());
   
		
		float dvdv2, dv2v1, dvdv1, dv2v2, dv1v1;
		float numer, denom;

		if (v2.len_squared() < N_TINY)
			return -1;
		
		if (v1.len_squared() < N_TINY)
			return -1;   

		dvdv2 = vd%v2; //p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
		dv2v1 = v2%v1;	//p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
		dvdv1 = vd%v1;	//p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
		dv2v2 = v2%v2;	//p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
		dv1v1 = v1%v1;	//p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;
		
		denom = dv1v1 * dv2v2 - dv2v1 * dv2v1;
		if (n_abs(denom) < N_TINY)
			return -1;
		
		numer = dvdv2 * dv2v1 - dvdv1 * dv2v2;
		
		float t1 = numer / denom;
		float t2 = (dvdv2 + dv2v1 * (t1)) / dv2v2;
		
		dl.set(this->ipol(t1), l.ipol(t2));   
		
		if ((t1 >= 0.0f && t1 <= 1.0f) && (t2 >= 0.0f && t2 <= 1.0f) && dl.len() <= N_TINY)
			return 1;
		else 
			return 0;
	} 
	
	float projection_t(const vector3& p) const
	{	
		float sqrMag = m.len_squared();
		if(sqrMag < N_TINY)
			return 0.0f;
		float t = (m % (p-this->va())) / sqrMag;
		return t;
	}

};
//-------------------------------------------------------------------
#endif
