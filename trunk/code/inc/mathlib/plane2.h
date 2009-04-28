#ifndef N_PLANE_H
#define N_PLANE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//-------------------------------------------------------------------
//  CLASSES
//  plane   -- a plane in 3d space
//-------------------------------------------------------------------

#include "mathlib/line.h"
#include "mathlib/winding.h"
#include "mathlib/bbox.h"
#include "mathlib/aabb.h"

inline bool approximatelyEqual(const float& a, const float& b, const float& epsilon = EQUAL_EPSILON)
	{ return float(fabs(a - b)) <= epsilon; }

// Nasty boy... Nasty nasty nasty boy
// access Vectors as an array
inline int getAlignmentValue(const vector3& v, const int i)
	{ return i == 0 ? v.x : (i == 1 ? v.y : v.z); }
inline void setAlignmentValue(vector3& v, const int i, float val)
	{ i == 0 ? v.x = val : (i == 1 ? v.y = val : v.z = val); }

//vector& operator [] (const int i)
//	{ return i == 0 ? x : (i == 1 ? y : z); }

// TBD: epsilon is used in default argument values.  Should we analyze them and come up with
//      a more obvious set of epsilon values?

// TBD: Most of the inlines were meant as a temporary solution until an actual math dll
//      could capture them in a cpp file.

class Plane {
public:
	Plane()
		: myDistance(float(0)) {}
	Plane(const Plane& plane)
		{ copy(plane); }
	
	Plane(const vector3& normal, float distance)
	{
	#ifdef _DEBUG
		float length = normal.len();
		n_assert(length >= 1.0 - EQUAL_EPSILON);
		n_assert(length <= 1.0 + EQUAL_EPSILON);
	#endif

		myNormal = normal;
		myDistance = distance;
		myAlignment = alignmentOf(myNormal);
	}
	
	Plane(vector3 a, vector3 b, vector3 c)
	{
		myNormal = (b - a) * (c - a); // normal = ab X ac

		// This SHOULD be a vector.normalize function... bleah
        float l = myNormal.len();
        if (l > TINY) {
            myNormal.x /= l;
            myNormal.y /= l;
        }
		else {
            myNormal.x = 0.0f;
            myNormal.y = 1.0f;
        }
		myDistance = myNormal % a;
		myAlignment = alignmentOf(myNormal);
	}

	Plane(const Winding& winding)
	{
		n_assert(winding.size() > 2);

		const vector3& a = winding[0].position;
		const vector3& b = winding[1].position;
		const vector3& c = winding[2].position;

		myNormal = (b - a) * (c - a); // normal = ab X ac
		// This SHOULD be a vector.normalize function... bleah
        float l = myNormal.len();
        if (l > TINY) {
            myNormal.x /= l;
            myNormal.y /= l;
        }
		else {
            myNormal.x = 0.0f;
            myNormal.y = 1.0f;
        }
		myDistance = myNormal % a;
		myAlignment = alignmentOf(myNormal);
	}

	inline const Plane& operator=(const Plane& plane)
		{ copy(plane); return *this; }

	inline const Plane operator+(const float offset) const
		{ return Plane(myNormal, myDistance + offset); }
	inline const Plane operator-(const float offset) const
		{ return Plane(myNormal, myDistance - offset); }

	inline bool equals(const Plane& x) const
		{ return myNormal.isequal(x.myNormal, DIST_EPSILON) && approximatelyEqual(myDistance, x.myDistance, DIST_EPSILON); }

	inline void mirror()
		{ myNormal = -myNormal; myDistance = -myDistance; }

	inline void orientNormalInPositiveDirection();

	const vector3& getNormal() const
		{ return myNormal; }
	const float& getDistance() const
		{ return myDistance; }

	// distance from point from plane (+ in dir of normal, - in opposite dir)
	float distanceTo(vector3 point) const
		{ return (myNormal % point) - myDistance; }

	// performs "in/out" test and returns true if out and false if on or in.
	bool hasFront(vector3 point) const
		{ return (myNormal % point) > myDistance; }

	bool intersectsRay(const vector3& from, const vector3& direction, vector3& hitPoint, const float epsilon = ANGLE_EPSILON) const
	{
		float denom = myNormal % direction;
		if (denom < epsilon) {
			// parallel
			return false;
		}

		// parametric equation
		float t = (myDistance - (myNormal % from)) / denom;
		if (t >= float(0)) {
			// intersection not behind ray
			hitPoint = from + direction * t;
			return true;
		}

		return false;
	}

	bool intersectsLineSegment(const vector3& from, const vector3& to, vector3& hitPoint, const float epsilon = ANGLE_EPSILON) const
	{
		vector3 direction(to - from);
		float denom = myNormal % direction;
		if (denom > epsilon || denom < -epsilon) {
			float t = (myDistance - (myNormal % from)) / denom;
			if (t >= 0.f && t <= 1.f) {
				hitPoint = from + (direction * t);
				return true;
			}
		}

		return false;
	}

	bool intersects(const Winding& winding, const float epsilon = DIST_EPSILON) const
	{
		bool front = false;
		bool back  = false;

		if (isAxial()) {
			for (Winding::const_iterator i = winding.begin(); i != winding.end(); ++i) {
				const float& v = getAlignmentValue( i->position, myAlignment);
				if (v > myDistance + epsilon) {
					if (back) {
						return true;
					}
					
					front = true;
				}
				else if (v < myDistance - epsilon) {
					if (front) {
						return true;
					}

					back = true;
				}
			}
		}
		else {
			for (Winding::const_iterator i = winding.begin(); i != winding.end(); ++i) {
				float distance = distanceTo(i->position);
				if (distance > epsilon) {
					if (back) {
						return true;
					}

					front = true;
				}
				else if (distance < -epsilon) {
					if (front) {
						return true;
					}

					back = true;
				}
			}
		}

		return true;
	}

    // Computes intersecting parameter t of line with plane.
	// Return false if line is parallel to plane.
    //
	// TBD: I'm not sure 0.0 is a good epsilon.
	//
    bool intersectsLine(const line3& l, float& t, const float epsilon = 0.0f) const {
        float dot = myNormal % l.m;
        if (dot > epsilon) {
            // line is not parallel, compute intersection point
            t = (myDistance - myNormal % l.b) / dot;
            return true;
        }
		else {
            // line is parallel to plane, return infinity
            return false;
        }
    }

    // Computes line of intersection with other plane.
    // Returns false if planes are parallel.
	//
    bool intersectsPlane(const Plane& p, line3& l, const float epsilon = 1e-06f) const {
		const vector3& hisNormal = p.myNormal;
		const float& hisDistance = p.myDistance;

		// TBD: Can we just write a determinant method instead of
		//      repeating this everywhere?
        float n00 = myNormal % myNormal;
        float n01 = myNormal % hisNormal;
        float n11 = hisNormal % hisNormal;
        float det = n00 * n11 - n01 * n01;

        if (fabs(det) < epsilon) {
			return false;
		}
		else {
            float inv_det = 1.0f / det;
            float c0 = (n11 * myDistance - n01 * hisDistance) * inv_det;
            float c1 = (n00 * hisDistance - n01 * myDistance) * inv_det;
            l.m = myNormal * hisNormal;
            l.b = myNormal * c0 + hisNormal * c1;

            return true;
        }
    }

	enum Relation {
		FRONT = 0, // something is in front of the plane (in positive direction)
		BACK = 1,  // something is behind the plane (in negative direction)
		ON = 2     // something is "smack dab" on the plane
	};

	enum Alignment {
		YZ = 0, // YZ plane
		XZ = 1, // XZ plane
		XY = 2, // XY plane
		NEAR_YZ = 3,
		NEAR_XZ = 4,
		NEAR_XY = 5
	};

	Alignment getAlignment() const {
		return myAlignment;
	}

	inline static Alignment alignmentOf(const vector3& normal);
	
	bool isAxial() const {
		return myAlignment < NEAR_YZ;
	}

	// Splice a bounding box into two bounding boxes.
	inline void splice(const AxisAlignedBox& in, AxisAlignedBox& front, AxisAlignedBox& back) const;

	// Splice a winding by the plane returning two windings.
	//
	// onBias - controls the result from a winding with all of its points lying on the plane.
	//    BACK: send to back
	//      ON: send to both
	//   FRONT: send to front
	inline void splice(const Winding& in, Winding& front, Winding& back, Relation onBias = Plane::ON) const;
	void clipBack(Winding& inOut, bool keepPointsIfOnPlane = true) const {
		Winding in(inOut), dummy;
		splice(in, inOut, dummy, keepPointsIfOnPlane ? Plane::FRONT : Plane::BACK);
	}
	void clipFront(Winding& inOut, bool keepPointsIfOnPlane = true) const {
		Winding in(inOut), dummy;
		splice(in, dummy, inOut, keepPointsIfOnPlane ? Plane::BACK : Plane::FRONT);
	}

	// Make a really big box on the plane at least as large as MAX_WORLD_VECTOR in every direction.
	inline void makeWinding(const float& size, Winding& winding, bool clockwise = true) const;

private:
	void copy(const Plane& plane) {
		myNormal = plane.myNormal;
		myDistance = plane.myDistance;
		myAlignment = plane.myAlignment;
	}

	vector3 myNormal;
	float myDistance;
	Plane::Alignment myAlignment;
};

/*
//-------------------------------------------------------------------
class plane {
public:
    float a,b,c,d;

    plane() : a(0.0f), b(0.0f), c(0.0f), d(1.0f) {};
    plane(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {};
    plane(const plane& p) : a(p.a), b(p.b), c(p.c), d(p.d) {};

    void set(float _a, float _b, float _c, float _d) {
        a = _a;
        b = _b;
        c = _c;
        d = _d;
    };

    //--- construct plane from 3 vectors ----------------------------
    void set(const vector3& v0, const vector3& v1, const vector3& v2) {
        vector3 cross((v2 - v0).cross(v1 - v0));
        cross.normalize();
        a = cross.x;
        b = cross.y;
        c = cross.z;
        d = -(a * v0.x + b * v0.y + c * v0.z);
    }

    plane(const vector3& v0, const vector3& v1, const vector3& v2) {
        set(v0,v1,v2);
    }

    // compute distance of point to plane, distance is 0.0 if point is on plane
    float distance(const vector3& v) const {
        return a*v.x + b*v.y + c*v.z + d;
    }

    vector3 normal() const {
        return vector3(a, b, c);
    }
    
    //--- get intersecting t of line with plane ---------------------
    //  return false if line is parallel to plane
    //
    bool intersect(const line3& l, float& t) const {
        vector3 abc(a,b,c);
        float dot = abc.dot(l.m);
        if (dot > 0.0f) {
            // line is not parallel, compute intersection point
            t = ((abc.dot(l.b)) + d) / dot;
            return true;
        }
		else {
            // line is parallel to plane, return infinity
            return false;
        }
    }

    // does this interset another plane?, get line of intersection,
    // return false if planes are parallel
    bool intersect(const plane& p, line3& l) const {
        vector3 n0 = normal();
        vector3 n1 = p.normal();
        float n00 = n0.dot(n0);
        float n01 = n0.dot(n1);
        float n11 = n1.dot(n1);
        float det = n00 * n11 - n01 * n01;
        const float tol = 1e-06f;
        if (fabs(det) < tol) return false;
        else {
            float inv_det = 1.0f/det;
            float c0 = (n11*d   - n01*p.d) * inv_det;
            float c1 = (n00*p.d - n01*d)   * inv_det;
            l.m = n0.cross(n1);
            l.b = n0*c0 + n1*c1;
            return true;
        }
    }
};
*/

Plane::Alignment Plane::alignmentOf(const vector3& normal)
{
	if (approximatelyEqual(normal.x, float(1), DIST_EPSILON) ||
		approximatelyEqual(normal.x, float(-1), DIST_EPSILON)) {
		return YZ;
	}

	if (approximatelyEqual(normal.y, float(1), DIST_EPSILON) ||
		approximatelyEqual(normal.y, float(-1), DIST_EPSILON)) {
		return XZ;
	}

	if (approximatelyEqual(normal.z, float(1), DIST_EPSILON) ||
		approximatelyEqual(normal.z, float(-1), DIST_EPSILON)) {
		return XY;
	}

	float x(fabs(normal.x));
	float y(fabs(normal.y));
	float z(fabs(normal.z));

	if (x >= y && x >= z) {
		return NEAR_YZ;
	}

	if (y >= x && y >= z) {
		return NEAR_XZ;
	}

	return NEAR_XY;
}

inline bool isPositivelyOrientedNormal(const vector3& v) {
	if (!approximatelyEqual(v.len(), float(1))) {
		return false;
	}

	Plane::Alignment alignment = Plane::alignmentOf(v);

	if (alignment >= Plane::NEAR_YZ) {
		return getAlignmentValue( v, alignment - Plane::NEAR_YZ) > float(0);
	}

	return
		approximatelyEqual(v.x, float(1), DIST_EPSILON) ||
		approximatelyEqual(v.y, float(1), DIST_EPSILON) ||
		approximatelyEqual(v.z, float(1), DIST_EPSILON)
		;
}

void Plane::orientNormalInPositiveDirection()
{
	if (approximatelyEqual(myNormal.x, float(-1), DIST_EPSILON)) {
		n_assert(approximatelyEqual(myNormal.y, float(0), ON_EPSILON));
		n_assert(approximatelyEqual(myNormal.z, float(0), ON_EPSILON));

		myNormal.x = float(1);
		myDistance = -myDistance;
		myAlignment = YZ;

		return;
	}

	if (approximatelyEqual(myNormal.y, float(-1), DIST_EPSILON)) {
		n_assert(approximatelyEqual(myNormal.x, float(0), ON_EPSILON));
		n_assert(approximatelyEqual(myNormal.z, float(0), ON_EPSILON));

		myNormal.y = float(1);
		myDistance = -myDistance;
		myAlignment = XZ;
		
		return;
	}

	if (approximatelyEqual(myNormal.z, float(-1), DIST_EPSILON)) {
		n_assert(approximatelyEqual(myNormal.x, float(0), ON_EPSILON));
		n_assert(approximatelyEqual(myNormal.y, float(0), ON_EPSILON));

		myNormal.z = float(1);
		myDistance = -myDistance;
		myAlignment = XY;
		
		return;
	}

	myAlignment = alignmentOf(myNormal);

	if (myAlignment >= NEAR_YZ) {
		if ( getAlignmentValue( myNormal, myAlignment - NEAR_YZ ) < float(0)) {
			mirror();
		}
	}
}

void Plane::makeWinding(const float& size, Winding& winding, bool clockwise) const
{
	float vmax(0);
	int majorAxis(0);
	for (int i = 0 ; i < 3; ++i) {
		float v(abs(getAlignmentValue(myNormal, i) ));
		if (v > vmax) {
			vmax = v;
			majorAxis = i;
		}
	}

	n_assert(vmax > float(0));

	vector3 up;
	if (majorAxis == 2) {
		up.x = float(1); // Z axis major
	}
	else {
		up.z = float(1); // X or Y axis major
	}

	// This was: up -= myNormal * up.dot(myNormal);
	float dot = up % myNormal;
	up.x -= myNormal.x * dot;
	up.y -= myNormal.y * dot;
	up.z -= myNormal.z * dot;

	// This SHOULD be a float.normalize function... bleah
    float l = up.len();
    if (l > TINY) {
        up.x /= l;
        up.y /= l;
    }
	else {
        up.x = 0.0f;
        up.y = 1.0f;
    }

	vector3 org(myNormal * myDistance);
	vector3 right(up * myNormal);

	// scale to something beyond any world vertex
	up *= size;
	right *= size;

	// Make a huge box.
	winding.clear();
	winding.resize(4);

	if (clockwise) {
		winding[0].position = org - right + up;
		winding[1].position = org + right + up;
		winding[2].position = org + right - up;
		winding[3].position = org - right - up;
	}
	else {
		winding[0].position = org - right + up;
		winding[1].position = org - right - up;
		winding[2].position = org + right - up;
		winding[3].position = org + right + up;
	}
}

void Plane::splice(const Winding& in, Winding& front, Winding& back, Plane::Relation onBias) const
{
	n_assert(in.size() > 2);

	front.clear();
	back.clear();

	int counts[3];
	counts[FRONT] = counts[BACK] = counts[ON] = 0;

	float* distances = new float[1 + in.size()];
	Relation* sides = new Relation[1 + in.size()];
	memset(sides, 0, sizeof(Relation) * (1 + in.size()));

	int j = 0;
	for (Winding::const_iterator v = in.begin(); v != in.end(); ++v, ++j) {
		distances[j] = distanceTo(v->position);
		if (distances[j] > ON_EPSILON) {
			sides[j] = FRONT;
		}
		else if (distances[j] < -ON_EPSILON) {
			sides[j] = BACK;
		}
		else {
			sides[j] = ON;
		}

		++counts[sides[j]];
	}

	sides[j] = sides[0];
	distances[j] = distances[0];

	if (!counts[FRONT] && !counts[BACK]) {
		// All points are exactly ON the plane.
		switch (onBias) {
		case FRONT: front = in; return;
		case ON:    front = in; back = in; return;
		case BACK:  back = in; return;
		}
	}
	else if (!counts[FRONT]) {
		back = in;
	}
	else if (!counts[BACK]) {
		front = in;
	}
	else {
		int i = 0;
		for (Winding::const_iterator p = in.begin(); p != in.end(); ++p, ++i) {
			if (sides[i] == ON) {
				front.push_back(*p);
				back.push_back(*p);
				continue;
			}

			if (sides[i] == FRONT) {
				front.push_back(*p);
			}

			if (sides[i] == BACK) {
				back.push_back(*p);
			}

			if (sides[i + 1] == ON || sides[i + 1] == sides[i]) {
				continue;
			}

			Winding::const_iterator p2 = p;
			++p2;
			if (p2 == in.end()) {
				p2 = in.begin();
			}

			float dot = distances[i] / (distances[i] - distances[i + 1]);

			// The linearly interpolated vertex.
			Vertex vert;

			// cartesian coordinates.
			for (int j = 0 ; j < 3 ; ++j) {
				// reduce round-off error by checking for special cases
				if (getAlignmentValue(myNormal, j) == float(1)) {
					setAlignmentValue(vert.position, j, myDistance);
				}
				else if (getAlignmentValue(myNormal, j) == float(-1)) {
					setAlignmentValue(vert.position, j, -myDistance);
				}
				else {
					setAlignmentValue(vert.position, j, getAlignmentValue(p->position, j) + 
						                                dot * (getAlignmentValue(p2->position, j) - 
														getAlignmentValue(p->position,j)));
				}
			}
/*
I think this is never really desired because we do not output sliced faces.

			// texture UV coordinates
			for (j = 0; j < 2; ++j) {
				vert.texture[j] = p->texture[j] + dot * (p2->texture[j] - p->texture[j]);
			}

			// TODO: rgb
			// TODO: norm
*/
			front.push_back(vert);
			back.push_back(vert);
		}
	}

	if (isDegenerate(front)) {
		front.clear();
	}
	
	if (isDegenerate(back)) {
		back.clear();
	}

#ifdef _DEBUG
	if (!front.empty()) {
		n_assert(front.size() > 2);
	}
	if (!back.empty()) {
		n_assert(back.size() > 2);
	}
#endif

	delete[] sides;
	delete[] distances;
}

void Plane::splice(const AxisAlignedBox& in, AxisAlignedBox& front, AxisAlignedBox& back) const
{
	front = in;
	back = in;

	switch (myAlignment) {
	case Plane::YZ:
		front.boxMin.x = back.boxMax.x = myDistance;
		break;
	case Plane::XZ:
		front.boxMin.y = back.boxMax.y = myDistance;
		break;
	case Plane::XY:
		front.boxMin.z = back.boxMax.z = myDistance;
		break;
	case Plane::NEAR_YZ:
	case Plane::NEAR_XZ:
	case Plane::NEAR_XY:
		// TODO: Compute a better bounds for alignments > Plane::XY.
		break;
	default:
		n_assert(false);
	}

	n_assert(front.isWellFormed());
	n_assert(back.isWellFormed());
}

#endif
