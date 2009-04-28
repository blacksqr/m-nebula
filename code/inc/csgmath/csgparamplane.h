#ifndef N_CSGPARAMPLANE_H
#define N_CSGPARAMPLANE_H
//-------------------------------------------------------------------
/**
@class csgparamplane
@brief extends a plane in 3d space for CSG purposes
*/
#ifndef N_PLANE_H
#include "csgmath/csgplane.h"
#endif


//-------------------------------------------------------------------
class csgparamplane : public csgplane {
public:
	vector3 o;
	vector3 x;
	vector3 y;
    csgparamplane() : csgplane() {};

	csgparamplane(float _a, float _b, float _c, float _d) : csgplane(_a, _b, _c, _d) 
	{
		this->build_axis();
	};

	csgparamplane(const csgparamplane& p) {
        this->a = p.a;
        this->b = p.b;
        this->c = p.c;
        this->d = p.d;
		this->build_axis();
	};

	csgparamplane(const csgplane& p) {
        this->a = p.a;
        this->b = p.b;
        this->c = p.c;
        this->d = p.d;
		this->build_axis();
	};

	csgparamplane(const vector3& v0, const vector3& v1, const vector3& v2){
		this->set(v0,v1,v2);		
	};

	void set(float _a, float _b, float _c, float _d) {
		csgplane::set(_a, _b, _c, _d);
		this->build_axis();
    };

    //--- construct plane from 3 vectors ----------------------------
    void set(const vector3& v0, const vector3& v1, const vector3& v2) {
        csgplane::set(v0, v1, v2);
		this->build_axis();
    };	

	void build_axis()
	{
		this->normalize();
		vector3 n(this->normal());
		this->o = n*(-this->d);
		this->x = n*vector3(1.0f, 0.0f, 0.0f);
		if (this->x.isident())
			this->x = n*vector3(0.0f, 1.0f, 0.0f);
		this->x.norm();
		this->y = n*this->x;
		this->y.norm();
	}    

	void build_axis(const vector3& _o, const vector3& _x)
	{
		this->normalize();		
		this->o = _o;
		this->x = _x;		
		this->x.norm();
		this->y = this->normal()*this->x;
		this->y.norm();
	}    

	vector2 toplane(const vector3& v) 
	{
		vector3 p(this->proj(v) - this->o);
		vector3 xproj = this->x.proj(p);
		vector3 yproj = this->y.proj(p);
		xproj.norm();
		return vector2(
			xproj.len() * (xproj%this->x), 
			yproj.len() * (yproj%this->y)
		);
	}

	vector3 fromplane(const vector2& p) 
	{
		return vector3(
			this->o.x + p.x * this->x.x + p.y * this->y.x,
			this->o.y + p.x * this->x.y + p.y * this->y.y,
			this->o.z + p.x * this->x.z + p.y * this->y.z
		);		
	}
};
#endif
