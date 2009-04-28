#ifndef N_ODE_COLLIDESHAPE_H
#define N_ODE_COLLIDESHAPE_H

//  nODECollideShape -- describes shapes for ODE collission system
//
//  OVERVIEW
//
//	Currently defines one of a few restricted primitive types
//		Sphere, Box, Cylinder
//
//	Complex (Triangle Lists) need to be worked out so we can
//	find the penetration depth of two overlapping convex shapes
//
//  One nCollideShape object may be shared between several
//  or many nCollideObject objects. 2 nCollideShape objects may 
//  also be queried directly whether they intersect.
//
//  The nCollideShape base class only can do sphere tests so this
//  class defers to ODE for others.

#define GENERATEBODIES

typedef unsigned char byte;

using namespace std;
#include <set>

//#include "ode/array.h"
#include "ode/dTriList.h"
//#include "ode/dxTriList.h"
//#include "ode/dcTriListCollider.h"

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#include "mathlib/aabb.h"

#undef N_DEFINES
#define N_DEFINES nOdeCollideShapes
#include "kernel/ndefdllclass.h"

//#include "mathlib/plane.h"

class vector3;
class matrix44;
class nFileServer;
class nGfxServer;
class nCollideReport;
class Plane;
struct dMass;

//
//
// Describes A Spherical ODE Shape by expanding on the  
// base nCollideShape's spherical properties.  All other
// collision shapes for ODE subclass this class.
//
//
//

class N_PUBLIC nOdeSphereCollideShape : public nCollideShape {
public:
	nOdeSphereCollideShape(const char *name) :
	    nCollideShape( name ),
        myOverrideDefaultMass( false ),
		myDensity( 0.1f ),
		myUseComputedCenter( false )
	{
	}
	virtual ~nOdeSphereCollideShape();

    virtual void Visualize(nGfxServer *);

    virtual bool isInitialized() const {
        return true;
    }

	// These must be called before buildOdeObject!!!!
	// Modifications will not take effect until a new
	// buildOdeObject call is made!
	//
	// The thought is that the SHAPE is defining a kind of thing
	// which has a constant mass/inertia/center.  You should 
	// define another shape/objetc pair if you have different
	// data to use.
	void setMassOverride( float newMass, matrix33 inertia, vector3 center )
	{
		myMass    = newMass;
		myInertia = inertia;
		myCenter  = center;
		
		// Use the mass as described here not the default based on 
		// radius
		myOverrideDefaultMass = true;
	}

//	void setRadius( float rad ) 
//	{
//		this->radius = rad;
//	}

	// Not considered a mass override but still must be called
	// before buildOdeObject unless the default 1.0 is intended
	void setDensity( float newDensity )
	{
		myDensity = newDensity;
	}

	float getDensity()
	{
		return( myDensity );
	}

	matrix33 getInertia()
	{
		return( myInertia );
	}

	vector3 getCenter()
	{
		return( myCenter );
	}

    virtual void Begin(int numVertices, int numTriangles);
    virtual void SetVertex(int id, vector3& v0);
    virtual void SetTriangle(int id, int vertex1, int vertex2, int vertex3);
    virtual void End();
	
	// Set useComputedCenter true if you want the shape to use the center point
	// of the AABB for its location computations, otherwise a 0/0/0 is assumed
	// and the gfxserver will provide a well-formed matrix.
	// 
	// (This is to fix a problem with the fact that lightweight object's 
	//  transforms are all 0/0/0 with massive space from 0/0/0 to the actual
	//  object definition.)
	void setUseComputedGeometricCenter( bool useComputed )
	{
		myUseComputedCenter = useComputed;
	}
	
	vector3 getCurrentGeometricCenter()
	{
		return( myMatrix.pos_component() );
	}
	
	// Build the ODE object based on the information known for this subclass.
	// Since we can share this class among many nCollideObjects, we don't hold
	// the information locally, but instead let the nCollideObject ask for this
	// data.  That way one shape can still feed MANY objects.
	virtual dGeomID buildOdeObject();
	
	// Uses mass information to build a proper mass object for this shape
	virtual void buildOdeMass( dMass *);

protected:
	friend class nOdeCollideObject;

	bool myOverrideDefaultMass;
	float myDensity, myMass;

	matrix33 myInertia; // inertia tensor for this shape
	vector3 myCenter;

	AxisAlignedBox myAab; // A hopefully well formed AABB
	bool myUseComputedCenter;
	matrix44 myMatrix;
};

class N_PUBLIC nOdeBoxCollideShape : public nOdeSphereCollideShape {
public:
	nOdeBoxCollideShape(const char *name) :
	    nOdeSphereCollideShape( name ),
		isBoxInitialized( false )
	{
//		myCollisionType = TEST_BOX;
	}

	virtual ~nOdeBoxCollideShape();

    virtual void Visualize(nGfxServer *);

    virtual bool isInitialized() const {
        return isBoxInitialized;
    }

    virtual void End();

	void setBoundingBox(float length, float width, float height);
	vector3 getBoxSides() const {
		return (vector3( myBoxSides[0], myBoxSides[1], myBoxSides[2]) );
	}

	// Build the ODE object based on the information known for this subclass.
	// Since we can share this class among many nCollideObjects, we don't hold
	// the information locally, but instead let the nCollideObject ask for this
	// data.  That way one shape can still feed MANY objects.
	virtual dGeomID buildOdeObject();
	
	// Uses mass information to build a proper mass object for this shape
	virtual void buildOdeMass( dMass *);
private:
	// Boxical
	float myBoxSides[3];
	bool isBoxInitialized;
};

class N_PUBLIC nOdeCylinderCollideShape : public nOdeSphereCollideShape {
public:
	nOdeCylinderCollideShape(const char *name) :
	    nOdeSphereCollideShape( name ),
		isCylinderInitialized( false ),
		myDirection( 3 )
	{
	}

	virtual ~nOdeCylinderCollideShape();

    virtual void Visualize(nGfxServer *);
    virtual void End();

	virtual bool isInitialized() const {
        return isCylinderInitialized;
    }

	// Direction?  X/Y/Z direction the cylinder lies along?
	void setBoundingCylinder(float radius, float height, int direction) {
		myCylRadius = radius;
		myCylHeight = height;
		vector3 v = vector3(0.0f, DIST_EPSILON + (float)sqrt(radius * radius + height * height), 0.0f );
		SetVertex(0, v);
		myDirection = direction;

		isCylinderInitialized = true;
	}

	// Used for bounding cylinder testing
	float getCylRadius() const {
		return myCylRadius;
	}
	
	float getCylHeight() const {
		return myCylHeight;
	}

	// Build the ODE object based on the information known for this subclass.
	// Since we can share this class among many nCollideObjects, we don't hold
	// the information locally, but instead let the nCollideObject ask for this
	// data.  That way one shape can still feed MANY objects.
	virtual dGeomID buildOdeObject();
	
	// Uses mass information to build a proper mass object for this shape
	virtual void buildOdeMass( dMass *);
private:
	// Cylindrical
	float myCylRadius;
	float myCylHeight;
	int  myDirection; // Not sure yet how ODE handles this (it seems to be only for mass computations)
	bool isCylinderInitialized;
};

class N_PUBLIC nOdePlaneCollideShape : public nOdeSphereCollideShape {
public:
	nOdePlaneCollideShape(const char *name) :
	    nOdeSphereCollideShape( name ),
		isPlaneInitialized( false )
	{
//		myCollisionType = TEST_PLANE;
	}

	virtual ~nOdePlaneCollideShape();

    virtual void Visualize(nGfxServer *);

	void setPlane( Plane p );

    virtual bool isInitialized() const {
        return isPlaneInitialized;
    }

	Plane *getPlane() const {
//		n_assert(myCollisionType == TEST_PLANE);
		return ( myPlane );
	}

	// Build the ODE object based on the information known for this subclass.
	// Since we can share this class among many nCollideObjects, we don't hold
	// the information locally, but instead let the nCollideObject ask for this
	// data.  That way one shape can still feed MANY objects.
	virtual dGeomID buildOdeObject();
	
	// Uses mass information to build a proper mass object for this shape
	virtual void buildOdeMass( dMass *);
private:
	// Planical
	Plane *myPlane;
	bool isPlaneInitialized;
};


class N_PUBLIC nOdeTriListCollideShape : public nOdeSphereCollideShape {
public:
	nOdeTriListCollideShape(const char *name) :
	    nOdeSphereCollideShape( name ),
		isTriListInitialized( false ),
		isVisualizing( false ),
		lastHitFace( -1 )
	{
	}

	virtual ~nOdeTriListCollideShape();

	// Build the shape
    virtual void Begin(int numVertices, int numTriangles);
    virtual void SetVertex(int id, vector3& v0);
    virtual void SetTriangle(int id, int vertex1, int vertex2, int vertex3);
    virtual void End();

    virtual void Visualize(nGfxServer *);

    virtual bool isInitialized() const {
        return isTriListInitialized;
    }

	// Set when the tri callbacks are hit, cleared every frame
	void setTestFaces( const int *faces, int numFaces );
	void setHitFace( int faces );

	static void dTriArrayCallback(dGeomID TriList, dGeomID RefObject, const int* TriIndices, int TriCount);
	static void dTriHitCallback(dGeomID TriList, dGeomID RefObject, int TriCount);
	static int myTriRayCallback(dGeomID TriList, dGeomID Ray, int TriangleIndex, dReal u, dReal v);

	static int dTriCallback(dGeomID TriList, dGeomID RefObject, int TriangleIndex);

	// Build the ODE object based on the information known for this subclass.
	// Since we can share this class among many nCollideObjects, we don't hold
	// the information locally, but instead let the nCollideObject ask for this
	// data.  That way one shape can still feed MANY objects.
	virtual dGeomID buildOdeObject();
	
	// Uses mass information to build a proper mass object for this shape
	virtual void buildOdeMass( dMass *);
private:
	// These are shared between Opcode and ODE
	dcVector3* myVertices;
	int      * myIndices;

	int numIndices;
	int numVertices;

	static nGfxServer *mygs; // HACK AHCK AHCK AHCKHAKCHAKHCKAHCKAC
    
	set<int> testFaces; // Render in yellow -- should never grow beyond numVertices
	set<int> hitFaces;  // Render in red -- should never grow beyond numVertices
	int lastHitFace;

	bool isVisualizing;

	bool isTriListInitialized;
};


#endif

