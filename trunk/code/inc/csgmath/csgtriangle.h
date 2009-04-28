#ifndef N_CSGTRIANGLE_H
#define N_CSGTRIANGLE_H
#include "mathlib/triangle.h"
#include "csgmath/csgplane.h"
//-------------------------------------------------------------------
/**
	@class triangle
	@brief expands a triangle for CSG purposes
*/
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//  Triangle points are tri(s,t)=b + s*e0 + t*e1 where
//  0<=s<=1, 0<=t<=1 and 0<=s+t<=1
//-------------------------------------------------------------------
class csgtriangle : public triangle {
public:		
	/// default constructor
	csgtriangle() : triangle(){}	
	/// 3 vertices constructor
	csgtriangle(const vector3& v0, const vector3& v1, const vector3& v2) :  triangle(v0, v1, v2) {}
	/// copy constructor
    csgtriangle(const triangle& t) : triangle(t) {} 
	/// set by 3 vertices
	void set(const vector3& v0, const vector3& v1, const vector3& v2) { 
		triangle::set(v0, v1, v2);		
	}
	/// return 1st vertex
    vector3 va() const { return v[a]; }
    /// return 2nd vertex
	vector3 vb() const { return v[b];}
	/// return 3rd vertex
	vector3 vc() const { return v[c];}
	
	float area() const; // + 15-04-2007, Gilad triangle area	
	
	float squared_distance(const vector3& p);

	float distance(const vector3& p) {
		return n_sqrt(this->squared_distance(p));
	}


	//--- get one the sides ---------------------------------------------
	line3 side(int i) const {	
		switch (i)
        {
        case 0: 
			return line3(n_min(va(), vb()), n_max(va(), vb()));	//line3(va(), vb());			
        case 1: 
			return line3(n_min(vc(), vb()), n_max(vc(), vb())); //line3(vb(), vc());
        case 2: 
			return line3(n_min(va(), vc()), n_max(va(), vc())); //line3(vc(), va());
        default: 
			return line3();
        }		
	}
	
	//--- get intersecting t of segment include ends with one-sided plane -----------
    //  return false if line is parallel to plane
    //
    bool intersect_seg_with_ends(const line3& l, float& t) const {
        bool isec = triangle::intersect(l, t);
        if (isec) {
            if (t >= 0.0f && t <= 1.0f)
                return true;
            else 
                return false;
        } else {
            return false;
        }
    }

    //--- get intersecting t of segment with one-sided plane -----------
    //  return false if line is parallel to plane
    //
    bool intersect_seg(const line3& l, float& t) const {
        bool isec = triangle::intersect(l, t);
        if (isec) {
            if (t > 0.0f && t < 1.0f)
                return true;
            else 
                return false;
        } else {
            return false;
        }
    }
	
    //--- get intersecting t of segment include ends with one-sided plane -----------
    //  return false if line is parallel to plane
    //
    bool intersect_both_sides_seg_with_ends(const line3& l, float& t) const {
		bool isec = triangle::intersect(l, t);
        if (isec) {
            if (t >= 0.0f && t <= 1.0f)
                return true;
            else 
                return false;
        } else {
            return false;
        }
    }

    //--- get intersecting t of segment with one-sided plane -----------
    //  return false if line is parallel to plane
    //
    bool intersect_both_sides_seg(const line3& l, float& t) const {
        bool isec = triangle::intersect(l, t);
        if (isec) {
            if (t > 0.0f && t < 1.0f)
                return true;
            else 
                return false;
        } else {
            return false;
        }
    }

	/// returns 0 if no cut, -1 if all 3 points are on the plane, 1 if cut exists and saves cut in l
	int intersect(const csgplane& p, line3& l) const 
	{	
		int a_side = p.side(this->va());

		if ( a_side &&			// not on plane
			(a_side == p.side(this->vb())) &&  // same side as va
			(a_side == p.side(this->vc()))) // same side as va					
			return 0;	
		line3 lab(this->side(0));
		line3 lbc(this->side(1));
		line3 lca(this->side(2));
/*
		float tol = (float)N_TOL;
		bool aonp = p.abs_distance(this->va()) < tol;
		bool bonp = p.abs_distance(this->vb()) < tol;
		bool conp = p.abs_distance(this->vc()) < tol;
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

		vector3 vab;
		vector3 vbc;
		vector3 vca;

		bool bab = p.intersect_seg_with_ends(lab, it);
		if (bab)  {
			vab = lab.ipol(it);
			isec++;
		}

		bool bbc = p.intersect_seg_with_ends(lbc, it);
		if (bbc) {
			vbc = lbc.ipol(it);
			isec++;
		}
		
		bool bca = p.intersect_seg_with_ends(lca, it);
		if (bca) {
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
    int intersect(const csgtriangle& t, line3& l) const {
        vector3 n1 = normal();
        vector3 n2 = t.normal();
		
		csgplane p(t.getplane());

		int a_side = p.side(this->va());

		if (a_side && 
			(a_side == p.side(this->vb())) && 
			(a_side == p.side(this->vc())))
			return 0;
       
        if (n_eq(n_abs(n1%n2), 1.0f)) { // coplanar
            float dist = getplane().distance(t.v[b]);
            if (dist < N_TINY) { // same plane
				return -2;
            }
            return -1;
        } else {
            int i, num = 0;
            float tm;
            line3 l1, l2;
            vector3 v[6];

            for (i = 0; i < 3; i++) {
                line3 ls(t.side(i));
                if (intersect_both_sides_seg_with_ends(ls, tm)) {
                    v[num] = ls.ipol(tm);                    
                    num++;
                }
            }
            
            for (i = 0; i < 3; i++) {
                line3 ls(side(i));
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
				float maxd = 0.0f;
				vector3& p0 = v[0];

				for (i = 1; i < num; i++) {
					float d = (p0-v[i]).len();
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

inline float csgtriangle::squared_distance(const vector3& p) {
    vector3 kDiff = point(0) - p;
    float fA00 = v[e0].len_squared();
    float fA01 = v[e0] % v[e1];
    float fA11 = v[e1].len_squared();
    float fB0 = kDiff % v[e0];
    float fB1 = kDiff % v[e1];
    float fC = kDiff.len_squared();
    float fDet = n_abs(fA00*fA11-fA01*fA01);
    float fS = fA01*fB1-fA11*fB0;
    float fT = fA01*fB0-fA00*fB1;
    float fSqrDistance;

    if (fS + fT <= fDet) {
		if (fS < 0.0f) {
            if (fT < 0.0f) { // region 4 
                if (fB0 < 0.0f) {
                    fT = 0.0f;
                    if (-fB0 >= fA00) {
                        fS = 1.0f;
                        fSqrDistance = fA00 + (2.0f)*fB0 + fC;
                    } else {
                        fS = -fB0/fA00;
                        fSqrDistance = fB0*fS+fC;
                    }
                } else {
                    fS = 0.0f;
                    if (fB1 >= 0.0f) {
                        fT = 0.0f;
                        fSqrDistance = fC;
                    } else if (-fB1 >= fA11) {
                        fT = 1.0f;
                        fSqrDistance = fA11+(2.0f)*fB1+fC;
                    } else {
                        fT = -fB1/fA11;
                        fSqrDistance = fB1*fT+fC;
                    }
                }
            } else { // region 3
                fS = 0.0f;
                if (fB1 >= 0.0f) {
                    fT = 0.0f;
                    fSqrDistance = fC;
                } else if (-fB1 >= fA11) {
                    fT = 1.0f;
                    fSqrDistance = fA11+(2.0f)*fB1+fC;
                } else {
                    fT = -fB1/fA11;
                    fSqrDistance = fB1*fT+fC;
                }
            }
        } else if (fT < 0.0f) { // region 5
            fT = 0.0f;
            if (fB0 >= 0.0f) {
                fS = 0.0f;
                fSqrDistance = fC;
            } else if (-fB0 >= fA00) {
                fS = 1.0f;
                fSqrDistance = fA00+(2.0f)*fB0+fC;
            } else {
                fS = -fB0/fA00;
                fSqrDistance = fB0*fS+fC;
            }
        } else { // region 0
            // minimum at interior point
            float fInvDet = (1.0f)/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDistance = fS*(fA00*fS+fA01*fT+(2.0f)*fB0) +
                fT*(fA01*fS+fA11*fT+(2.0f)*fB1)+fC;
        }
    } else {
        float fTmp0, fTmp1, fNumer, fDenom;

        if (fS < 0.0f) { // region 2
            fTmp0 = fA01 + fB0;
            fTmp1 = fA11 + fB1;
            if (fTmp1 > fTmp0) {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-2.0f*fA01+fA11;
                if (fNumer >= fDenom) {
                    fS = 1.0f;
                    fT = 0.0f;
                    fSqrDistance = fA00+(2.0f)*fB0+fC;
                } else {
                    fS = fNumer/fDenom;
                    fT = 1.0f - fS;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                        fT*(fA01*fS+fA11*fT+(2.0f)*fB1)+fC;
                }
            } else {
                fS = 0.0f;
                if (fTmp1 <= 0.0f) {
                    fT = 1.0f;
                    fSqrDistance = fA11+(2.0f)*fB1+fC;
                } else if (fB1 >= 0.0f) {
                    fT = 0.0f;
                    fSqrDistance = fC;
                } else {
                    fT = -fB1/fA11;
                    fSqrDistance = fB1*fT+fC;
                }
            }
        } else if (fT < 0.0f) { // region 6
            fTmp0 = fA01 + fB1;
            fTmp1 = fA00 + fB0;
            if (fTmp1 > fTmp0) {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-(2.0f)*fA01+fA11;
                if (fNumer >= fDenom) {
                    fT = 1.0f;
                    fS = 0.0f;
                    fSqrDistance = fA11+(2.0f)*fB1+fC;
                } else {
                    fT = fNumer/fDenom;
                    fS = 1.0f - fT;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+(2.0f)*fB0) +
                        fT*(fA01*fS+fA11*fT+(2.0f)*fB1)+fC;
                }
            } else {
                fT = 0.0f;
                if (fTmp1 <= 0.0f) {
                    fS = 1.0f;
                    fSqrDistance = fA00+(2.0f)*fB0+fC;
                } else if (fB0 >= 0.0f) {
                    fS = 0.0f;
                    fSqrDistance = fC;
                } else {
                    fS = -fB0/fA00;
                    fSqrDistance = fB0*fS+fC;
                }
            }
        } else { // region 1
            fNumer = fA11 + fB1 - fA01 - fB0;
            if (fNumer <= 0.0f) {
                fS = 0.0f;
                fT = 1.0f;
                fSqrDistance = fA11+(2.0f)*fB1+fC;
            } else {
                fDenom = fA00-2.0f*fA01+fA11;
                if (fNumer >= fDenom) {
                    fS = 1.0f;
                    fT = 0.0f;
                    fSqrDistance = fA00+(2.0f)*fB0+fC;
                } else {
                    fS = fNumer/fDenom;
                    fT = 1.0f - fS;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+(2.0f)*fB0) +
                        fT*(fA01*fS+fA11*fT+(2.0f)*fB1)+fC;
                }
            }
        }
    }

    // account for numerical round-off error
    if (fSqrDistance < 0.0f) {
        fSqrDistance = 0.0f;
    }
	
    return fSqrDistance;
}

// Gilad 15/4/07 triangle area
inline float csgtriangle::area() const
{
	float a = v[e0].len_squared();
	float b = v[e1].len_squared();
	float c = v[e0] % v[e1];
	float s2 = max(a*b - c*c, 0.f);
	float area = 0.5f * sqrtf(s2);
	return area;
}

//-------------------------------------------------------------------
#endif

