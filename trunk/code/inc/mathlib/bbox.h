#ifndef N_BBOX_H
#define N_BBOX_H
//-------------------------------------------------------------------
/**
    @class t_bbox3
    @brief (non-oriented) bounding box
*/
//-------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

#define INV_SIDE ((ntype)1000000)

//-------------------------------------------------------------------
//  t_bbox3
//-------------------------------------------------------------------
template<class ntype> 
class t_bbox3 
{
public:
	typedef t_vector3<ntype> t_vector;
	typedef t_line_n<t_vector3, ntype> t_line;

public:
    t_vector vmin;
    t_vector vmax;

    enum 
	{
        CLIP_LEFT   = (1<<0),
        CLIP_RIGHT  = (1<<1),
        CLIP_BOTTOM = (1<<2),
        CLIP_TOP    = (1<<3),
        CLIP_NEAR   = (1<<4),
        CLIP_FAR    = (1<<5),
    };

    enum {
        OUTSIDE     = 0,
        ISEQUAL     = (1<<0),
        ISCONTAINED = (1<<1),
        CONTAINS    = (1<<2),
        CLIPS       = (1<<3),
    };

    //--- constructors ----------------------------------------------
    t_bbox3() {}
    t_bbox3(const t_vector& _vmin, const t_vector& _vmax)	
	{
		this->set(_vmin, _vmax);
	}
    t_bbox3(const t_bbox3<ntype>& bb)
	{
		this->set(bb.vmin, bb.vmax);
	}
    
    //--- initialize from t_vector cloud -----------------------------
    t_bbox3(const t_vector* varray, size_t num) 
	{
		this->set(varray, num);        
    }

    //--- setting elements ------------------------------------------
    void set(const t_vector& _vmin, const t_vector& _vmax) 
	{
        this->vmin = _vmin;
        this->vmax = _vmax;
    }

    void set(const t_vector* varray, size_t num) 
	{
        this->set(varray[0], varray[0]);
		for (size_t i = 0; i < num; i++) 
		{
            this->grow(varray[i]);
        }        
    }

    //--- invalidate bounding box to prepare for growing ------------
    void begin_grow(void) 
	{
        vmin.set( INV_SIDE, INV_SIDE, INV_SIDE);
        vmax.set(-INV_SIDE,-INV_SIDE,-INV_SIDE);
    } 

    void grow(const t_vector& v) 
	{
		vmin.x = n_min(vmin.x, v.x);
		vmax.x = n_max(vmax.x, v.x);
		vmin.y = n_min(vmin.y, v.y);
		vmax.y = n_max(vmax.y, v.y);
		vmin.z = n_min(vmin.z, v.z);
		vmax.z = n_max(vmax.z, v.z);
    }

    void grow(t_vector* varray, size_t num) 
	{        
        for (size_t i = 0; i < num; i++) 
		{
            this->grow(varray[i]);
        }
    }

    void grow(const t_bbox3<ntype>& bb) 
	{
		vmin.x = n_min(bb.vmin.x, vmin.x);
		vmin.y = n_min(bb.vmin.y, vmin.y);
		vmin.z = n_min(bb.vmin.z, vmin.z);
		vmax.x = n_max(bb.vmax.x, vmax.x);		
		vmax.y = n_max(bb.vmax.y, vmax.y);		
		vmax.z = n_max(bb.vmax.z, vmax.z);        
    }

    // get point of intersection of 3d line with planes
    // on const x,y,z
    bool isect_const_x(const ntype& x, const t_line& l, 
		t_vector& out) const 
	{
        if (n_neqz(l.dir().x)) 
		{
            ntype t = (x - l.va().x) / l.dir().x;
            if (n_in1(t)) 
			{// point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }

    bool isect_const_y(const ntype& y, const t_line& l, t_vector& out) const 
	{
        if (n_neqz(l.dir().y)) 
		{
            ntype t = (y - l.va().y) / l.dir().y;
            if (n_in1(t)) 
			{// point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }
    bool isect_const_z(const ntype& z, const t_line& l,	t_vector& out) const 
	{
        if (n_neqz(l.dir().z))
		{
            ntype t = (z - l.va().z) / l.dir().z;
            if (n_in1(t)) 
			{// point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }
    // point in polygon check for sides with constant x,y and z
    bool pip_const_x(const t_vector& p) const {
		return (n_inrange(p.y, vmin.y, vmax.y) && n_inrange(p.z, vmin.z, vmax.z));
    }
    bool pip_const_y(const t_vector& p) const {
		return (n_inrange(p.x, vmin.x, vmax.x) && n_inrange(p.z, vmin.z, vmax.z));		
    }
    bool pip_const_z(const t_vector& p) const {
		return (n_inrange(p.x, vmin.x, vmax.x) && n_inrange(p.y, vmin.y, vmax.y));
    }

	bool inside (const t_vector& p) const {
		return (n_inrange(p.x, vmin.x, vmax.x) && 
			n_inrange(p.y, vmin.y, vmax.y) &&
			n_inrange(p.z, vmin.z, vmax.z));
	}
    //--- check if box intersects or contains line ------------------
    bool intersect(const t_line& line) const 
	{
        // For each side of box, check if point of intersection
        // lies within this sides 2d rectangle. If at least one
        // intersection occurs, the line intersects the box
        // (usually, there will be 2 intersections).
        // The line parameter t for the intersection is computed
        // by resolving the formula:
        //  p = line.a + line.dir()*t
        // after t:
        //  t = (p - line.a) / line.dir()
        // if line.dir() is zero, the line is parallel to the plane in
        // question.
        // MAY BE EXTENDED TO RETURN CLOSEST POINT OF INTERSECTION!
        
        // check if at least one of the 2 points is included in the volume
        t_vector s(line.va());
        t_vector e(line.vb());
        if (this->inside(s) || this->inside(e))
        {
            return true;
        } 
		else 
		{
            // otherwise do intersection check
            int i;
            t_vector ipos;
            for (i=0; i<6; i++) {
                switch (i) {
                    // left side, vmin.x is constant
                    case 0:
                        if (isect_const_x(vmin.x,line,ipos) && pip_const_x(ipos)) return true;
                        break;
                    case 1:
                        if (isect_const_x(vmax.x,line,ipos) && pip_const_x(ipos)) return true;
                        break;
                    case 2:
                        if (isect_const_y(vmin.y,line,ipos) && pip_const_y(ipos)) return true;
                        break;
                    case 3:
                        if (isect_const_y(vmax.y,line,ipos) && pip_const_y(ipos)) return true;
                        break;
                    case 4:
                        if (isect_const_z(vmin.z,line,ipos) && pip_const_z(ipos)) return true;
                        break;
                    case 5:
                        if (isect_const_z(vmax.z,line,ipos) && pip_const_z(ipos)) return true;
                        break;
                }
            }
        }
        return false;
    }

    /**
        @brief Gets closest intersection with AABB.
        If the line starts inside the box,  start point is returned in ipos.
        @param line the pick ray
        @param ipos closest point of intersection if successful, trash otherwise
        @return true if an intersection occurs
    */
    bool intersect(const t_line& line, t_vector& ipos) const
    {
        // Handle special case for start point inside box
        if (this->inside(line.start()))
        {
            ipos = line.start();
            return true;
        }

        // Order planes to check, closest three only
        int plane[3];
        if (line.dir().x > 0) plane[0] = 0;
        else              plane[0] = 1;
        if (line.dir().y > 0) plane[1] = 2;
        else              plane[1] = 3;
        if (line.dir().z > 0) plane[2] = 4;
        else              plane[2] = 5;
        
        for (int i = 0; i < 3; ++i)
        {
            switch (plane[i]) {
                case 0:
                    if (isect_const_x(vmin.x,line,ipos) && pip_const_x(ipos)) return true;
                    break;
                case 1:
                    if (isect_const_x(vmax.x,line,ipos) && pip_const_x(ipos)) return true;
                    break;
                case 2:
                    if (isect_const_y(vmin.y,line,ipos) && pip_const_y(ipos)) return true;
                    break;
                case 3:
                    if (isect_const_y(vmax.y,line,ipos) && pip_const_y(ipos)) return true;
                    break;
                case 4:
                    if (isect_const_z(vmin.z,line,ipos) && pip_const_z(ipos)) return true;
                    break;
                case 5:
                    if (isect_const_z(vmax.z,line,ipos) && pip_const_z(ipos)) return true;
                    break;
            }
        }

        return false;
    }

    //--- check if box intersects, contains or is contained in other box
    //--- by doing 3 projection tests for each dimension, if all 3 test 
    //--- return true, then the 2 boxes intersect
    int line_test(const ntype& v0, const ntype& v1, const ntype& w0, const ntype& w1) const
    {
        // quick rejection test
        if ((v1<w0) || (v0>w1)) return OUTSIDE;
        else if ((v0==w0) && (v1==w1)) return ISEQUAL;
        else if ((v0>=w0) && (v1<=w1)) return ISCONTAINED;
        else if ((v0<=w0) && (v1>=w1)) return CONTAINS;
        else return CLIPS;
    }

    int intersect(const t_bbox3& box) const
	{
        int and_code = 0xffff;
        int or_code  = 0;
        int cx,cy,cz;
        cx = line_test(vmin.x,vmax.x,box.vmin.x,box.vmax.x);
        and_code&=cx; or_code|=cx;
        cy = line_test(vmin.y,vmax.y,box.vmin.y,box.vmax.y);
        and_code&=cy; or_code|=cy;
        cz = line_test(vmin.z,vmax.z,box.vmin.z,box.vmax.z);
        and_code&=cz; or_code|=cz;
        if (or_code == 0) return OUTSIDE;
        else if (and_code != 0) {
            return and_code;
        } else {
            // only if all test produced a non-outside result,
            // an intersection has occured
            if (cx && cy && cz) return CLIPS;
            else                return OUTSIDE;
        }
    }
};

typedef t_bbox3<nReal> bbox3;
//-------------------------------------------------------------------
#endif
