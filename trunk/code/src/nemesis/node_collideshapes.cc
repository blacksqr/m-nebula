#define N_IMPLEMENTS nOdeCollideShapes

#include "nemesis/node_physics.h"
#include "nemesis/node_collideshapes.h"
#include "nemesis/node_collideobject.h"
#include "gfx/ngfxserver.h"
#include "mathlib/plane2.h"
// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/ode.h"

#define MYMAX(a,b)	( (a) > (b) ? (a) : (b) )

//
// Describes A Spherical ODE Shape by expanding on the  
// base nCollideShape's spherical properties.  All other
// collision shapes for ODE subclass this class.
//

nOdeSphereCollideShape::~nOdeSphereCollideShape()
{
}


void nOdeSphereCollideShape::Visualize(nGfxServer *gs)
{
	// Spherical representations are handled by the context
	// which can be confusing, so I overlay a purple one 
	// to distinguish the two..
    gs->Begin(N_PTYPE_LINE_LIST);
    gs->Rgba(0.4f,0.4f,0.4f,0.3f);
    
	// render the objects radii
	float radius = GetRadius();
    int dim;
    float dr = n_deg2rad(10.0f);
    for (dim=0; dim<3; dim++) {
        float r;
        for (r=0.0f; r<n_deg2rad(360.0f); r+=dr) {
            float sin_r0 = (float) sin(r);
            float cos_r0 = (float) cos(r);
            float sin_r1 = (float) sin(r+dr);
            float cos_r1 = (float) cos(r+dr);
            vector3 v0_x(0.5f, sin_r0*radius, cos_r0*radius);
            vector3 v1_x(0.5f, sin_r1*radius, cos_r1*radius);
            vector3 v00_x(-0.5f, sin_r0*radius, cos_r0*radius);
            vector3 v11_x(-0.5f, sin_r1*radius, cos_r1*radius);
            vector3 v0_y(sin_r0*radius, 0.5f, cos_r0*radius);
            vector3 v1_y(sin_r1*radius, 0.5f, cos_r1*radius);
            vector3 v00_y(sin_r0*radius, -0.5f, cos_r0*radius);
            vector3 v11_y(sin_r1*radius, -0.5f, cos_r1*radius);
            vector3 v0_z(sin_r0*radius, cos_r0*radius, -0.5f);
            vector3 v1_z(sin_r1*radius, cos_r1*radius, -0.5f);
            vector3 v00_z(sin_r0*radius, cos_r0*radius, 0.5f);
            vector3 v11_z(sin_r1*radius, cos_r1*radius, 0.5f);
            gs->Coord(v0_x.x,v0_x.y,v0_x.z); gs->Coord(v1_x.x,v1_x.y,v1_x.z);
            gs->Coord(v00_x.x,v00_x.y,v00_x.z); gs->Coord(v11_x.x,v11_x.y,v11_x.z);
            gs->Coord(v0_y.x,v0_y.y,v0_y.z); gs->Coord(v1_y.x,v1_y.y,v1_y.z);
            gs->Coord(v00_y.x,v00_y.y,v00_y.z); gs->Coord(v11_y.x,v11_y.y,v11_y.z);
            gs->Coord(v0_z.x,v0_z.y,v0_z.z); gs->Coord(v1_z.x,v1_z.y,v1_z.z);
            gs->Coord(v00_z.x,v00_z.y,v00_z.z); gs->Coord(v11_z.x,v11_z.y,v11_z.z);
        }
    }
    gs->End();
}

// Build the ODE object based on the information known for this subclass.
// Since we can share this class among many nCollideObjects, we don't hold
// the information locally, but instead let the nCollideObject ask for this
// data.  That way one shape can still feed MANY objects.
dGeomID nOdeSphereCollideShape::buildOdeObject()
{
	return(dCreateSphere( 0, GetRadius()));
}

// Uses mass information to build a proper mass object for this kind of shape
void nOdeSphereCollideShape::buildOdeMass( dMass *mass )
{
	if (!myOverrideDefaultMass)
	{
		dMassSetSphere( mass, myDensity, this->GetRadius() );
	} else {
		dMassSetParameters( mass, myMass, 
							myCenter.x, myCenter.y, myCenter.z, 
							myInertia.M11, myInertia.M22, myInertia.M33,
							myInertia.M12, myInertia.M13, myInertia.M23 );
	}
}

void nOdeSphereCollideShape::Begin( int numv, int numt)
{
	nCollideShape::Begin( numv, numt );
}

void nOdeSphereCollideShape::SetVertex(int id, vector3& v0)
{
	nCollideShape::SetVertex( id, v0);
	myAab.extendToFit( v0 );
}

void nOdeSphereCollideShape::SetTriangle(int id, int v0, int v1, int v2)
{
	nCollideShape::SetTriangle( id, v0, v1, v2 );
}


void nOdeSphereCollideShape::End()
{
	nCollideShape::End();
	float theBoxSides[3];

	theBoxSides[0] = fabs(myAab[3] - myAab[0]);
	theBoxSides[1] = fabs(myAab[4] - myAab[1]);
	theBoxSides[2] = fabs(myAab[5] - myAab[2]);

	if (myUseComputedCenter)
	{
		// Set the radius by the box, not the distance from vertex to 0/0/0
		myMatrix.M41 = myAab[3] + ( theBoxSides[0] * 0.5 );
		myMatrix.M42 = myAab[4] + ( theBoxSides[1] * 0.5 );
		myMatrix.M43 = myAab[5] + ( theBoxSides[2] * 0.5 );

//		vector3 test( myMatrix.pos_component() - myAab.boxMax );
//		myRadius = test.getLength();
	}
//	radius = MYMAX( theBoxSides[0], theBoxSides[1]);
//	radius = MYMAX( radius, theBoxSides[2] );
//	radius *= 0.5;
}


/******************************************************************************************
										Box Lunch
 ******************************************************************************************/
nOdeBoxCollideShape::~nOdeBoxCollideShape()
{
}


void nOdeBoxCollideShape::Visualize(nGfxServer *gs)
{
	// visualize the box
    gs->Begin(N_PTYPE_LINE_LIST);
    gs->Rgba(0.4f,0.4f,0.4f,0.3f);

	float x2 = fabs(myBoxSides[0]) *.5f;
	float y2 = fabs(myBoxSides[1]) *.5f;
	float z2 = fabs(myBoxSides[2]) *.5f;
	
    gs->Coord( -x2, -y2, -z2); gs->Coord(-x2, y2, -z2);
    gs->Coord( -x2,  y2, -z2); gs->Coord( x2, y2, -z2);
    gs->Coord(  x2,  y2, -z2); gs->Coord( x2,-y2, -z2);
    gs->Coord(  x2, -y2, -z2); gs->Coord(-x2,-y2, -z2);

    gs->Coord( -x2, -y2, z2); gs->Coord(-x2, y2, z2);
    gs->Coord( -x2,  y2, z2); gs->Coord( x2, y2, z2);
    gs->Coord(  x2,  y2, z2); gs->Coord( x2,-y2, z2);
    gs->Coord(  x2, -y2, z2); gs->Coord(-x2,-y2, z2);

    gs->Coord( -x2, -y2, -z2); gs->Coord(-x2, -y2, z2);
    gs->Coord(  x2, -y2, -z2); gs->Coord( x2, -y2, z2);
    gs->Coord(  x2,  y2, -z2); gs->Coord( x2,  y2, z2);
    gs->Coord(  -x2, y2, -z2); gs->Coord(-x2,  y2, z2);

    gs->End();
}

void nOdeBoxCollideShape::setBoundingBox(float length, float width, float height)
{

	myBoxSides[0] = length;
	myBoxSides[1] = width;
	myBoxSides[2] = height;
	
	float radius = GetRadius();
	for (int i=0; i < 3; i++)
	{
		if (radius < DIST_EPSILON + myBoxSides[i] * 0.5f )
			radius = DIST_EPSILON + myBoxSides[i] * 0.5f;
	}
	
	// Initialize the radius of the object
	vector3 v = vector3(0.0f, radius, 0.0f );
	SetVertex(0, v);

	isBoxInitialized = true;
}


// Build the ODE object based on the information known for this subclass.
// Since we can share this class among many nCollideObjects, we don't hold
// the information locally, but instead let the nCollideObject ask for this
// data.  That way one shape can still feed MANY objects.
dGeomID nOdeBoxCollideShape::buildOdeObject()
{
	return( dCreateBox( 0, myBoxSides[0], myBoxSides[1], myBoxSides[2]));
}

// Uses mass information to build a proper mass object for this kind of shape
void nOdeBoxCollideShape::buildOdeMass( dMass *mass )
{
	if (!myOverrideDefaultMass)
	{
		dMassSetBox( mass, myDensity, myBoxSides[0], myBoxSides[1], myBoxSides[2] );
	} else {
		dMassSetParameters( mass, myMass, 
							myCenter.x, myCenter.y, myCenter.z, 
							myInertia.M11, myInertia.M22, myInertia.M33,
							myInertia.M12, myInertia.M13, myInertia.M23 );
	}
}

void nOdeBoxCollideShape::End()
{
	nCollideShape::End();

	myBoxSides[0] = fabs(myAab[3] - myAab[0]);
	myBoxSides[1] = fabs(myAab[4] - myAab[1]);
	myBoxSides[2] = fabs(myAab[5] - myAab[2]);

	if (myUseComputedCenter)
	{
		// Set the radius by the box, not the distance from vertex to 0/0/0
		myMatrix.M41 = myAab[3] + ( myBoxSides[0] * 0.5 );
		myMatrix.M42 = myAab[4] + ( myBoxSides[1] * 0.5 );
		myMatrix.M43 = myAab[5] + ( myBoxSides[2] * 0.5 );
	}

	// Eek
	vector3 test( myMatrix.pos_component() - myAab.boxMax );
	SetVertex(0, test );
}
/******************************************************************************************
										Cylindrical Seas
 ******************************************************************************************/
nOdeCylinderCollideShape::~nOdeCylinderCollideShape()
{
}


void nOdeCylinderCollideShape::Visualize(nGfxServer *gs)
{
	// ze the Cylinder
	gs->Begin(N_PTYPE_LINE_LIST);
    gs->Rgba(0.4f,0.4f,0.4f,0.3f);

	float halfHeight = myCylHeight * .5;
	float dr = n_deg2rad(360.0f / (float)48);

	for (float r = 0.0f; r < n_deg2rad(360.0f); r += dr) {
	
		float sin_r0 = (float) sin(r);
		float cos_r0 = (float) cos(r);
		float sin_r1 = (float) sin(r + dr);
		float cos_r1 = (float) cos(r + dr);

		vector3 v0_y(sin_r0 * myCylRadius, 0.0f, cos_r0 * myCylRadius);
		vector3 v1_y(sin_r1 * myCylRadius, 0.0f, cos_r1 * myCylRadius);
		v0_y.y -= halfHeight;
		v1_y.y -= halfHeight;
		gs->Coord(v0_y.x,v0_y.z,v0_y.y);
		gs->Coord(v1_y.x,v1_y.z,v1_y.y);
		gs->Coord(v0_y.x,v0_y.z,v0_y.y);
		v0_y.y += myCylHeight;
		v1_y.y += myCylHeight;
		gs->Coord(v0_y.x,v0_y.z,v0_y.y);
		gs->Coord(v0_y.x,v0_y.z,v0_y.y);
		gs->Coord(v1_y.x,v1_y.z,v1_y.y);
	}

	gs->Coord(0, 0, -myCylHeight * .50);
	gs->Coord(0, 0, myCylHeight  * .50);

    // render the objects endcap
    int dim;
    for (dim=0; dim<3; dim++) {
        float r;
        for (r=n_deg2rad(-90.0f); r<n_deg2rad(90.0f); r+=dr) {
            float sin_r0 = (float) sin(r);
            float cos_r0 = (float) cos(r);
            float sin_r1 = (float) sin(r+dr);
            float cos_r1 = (float) cos(r+dr);
            vector3 v0_x(0.0f, sin_r0*this->myCylRadius, cos_r0*this->myCylRadius);
            vector3 v1_x(0.0f, sin_r1*this->myCylRadius, cos_r1*this->myCylRadius);
            vector3 v0_y(sin_r0*this->myCylRadius, 0.0f, cos_r0*this->myCylRadius);
            vector3 v1_y(sin_r1*this->myCylRadius, 0.0f, cos_r1*this->myCylRadius);
            gs->Coord(v0_x.x,v0_x.y,v0_x.z+halfHeight); gs->Coord(v1_x.x,v1_x.y,v1_x.z+halfHeight);
            gs->Coord(v0_y.x,v0_y.y,v0_y.z+halfHeight); gs->Coord(v1_y.x,v1_y.y,v1_y.z+halfHeight);
        }
    }
	
    // render the objects endcap
    for (dim=0; dim<3; dim++) {
        float r;
        for (r=n_deg2rad(90.0f); r<n_deg2rad(270.0f); r+=dr) {
            float sin_r0 = (float) sin(r);
            float cos_r0 = (float) cos(r);
            float sin_r1 = (float) sin(r+dr);
            float cos_r1 = (float) cos(r+dr);
            vector3 v0_x(0.0f, sin_r0*this->myCylRadius, cos_r0*this->myCylRadius);
            vector3 v1_x(0.0f, sin_r1*this->myCylRadius, cos_r1*this->myCylRadius);
            vector3 v0_y(sin_r0*this->myCylRadius, 0.0f, cos_r0*this->myCylRadius);
            vector3 v1_y(sin_r1*this->myCylRadius, 0.0f, cos_r1*this->myCylRadius);
            gs->Coord(v0_x.x,v0_x.y,v0_x.z-halfHeight); gs->Coord(v1_x.x,v1_x.y,v1_x.z-halfHeight);
            gs->Coord(v0_y.x,v0_y.y,v0_y.z-halfHeight); gs->Coord(v1_y.x,v1_y.y,v1_y.z-halfHeight);
        }
    }
    
	gs->End();
}

void nOdeCylinderCollideShape::End()
{
	nCollideShape::End();
	float theBoxSides[3];

	theBoxSides[0] = fabs(myAab[3] - myAab[0]);
	theBoxSides[1] = fabs(myAab[4] - myAab[1]);
	theBoxSides[2] = fabs(myAab[5] - myAab[2]);

	vector3 test( myMatrix.pos_component() - myAab.boxMax );
	myCylRadius = MYMAX( theBoxSides[1], theBoxSides[2]) * .5;
	myCylHeight = theBoxSides[2];

}
// Build the ODE object based on the information known for this subclass.
// Since we can share this class among many nCollideObjects, we don't hold
// the information locally, but instead let the nCollideObject ask for this
// data.  That way one shape can still feed MANY objects.
dGeomID nOdeCylinderCollideShape::buildOdeObject()
{
	return( dCreateCCylinder( 0, myCylRadius, myCylHeight) );
}

// Uses mass information to build a proper mass object for this kind of shape
void nOdeCylinderCollideShape::buildOdeMass( dMass *mass )
{
	if (!myOverrideDefaultMass)
	{
		dMassSetCappedCylinder( mass, myDensity, myDirection,
								myCylRadius, myCylHeight );
	} else {
		dMassSetParameters( mass, myMass, 
							myCenter.x, myCenter.y, myCenter.z, 
							myInertia.M11, myInertia.M22, myInertia.M33,
							myInertia.M12, myInertia.M13, myInertia.M23 );
	}
}

/******************************************************************************************
										Planar Myalgia
 ******************************************************************************************/
nOdePlaneCollideShape::~nOdePlaneCollideShape()
{
	delete myPlane;
}

void nOdePlaneCollideShape::setPlane( Plane p )
{
//	n_assert( myCollisionType == TEST_PLANE );
	
	myPlane = new Plane( p );

	vector3 v(0.0f, 1000.0f, 0.0f);
	
	// Sets the radius.. bleah
	SetVertex(0, v );

	isPlaneInitialized = true;
}

void nOdePlaneCollideShape::Visualize(nGfxServer *gs)
{
	// Uhmm -- given a normal, how does one get the Plane?
	// 
	// It seems easy, since the normal is perpendicular to the
	// Plane, and we have a distance, but... I don't seem to 
	// know the equation.
	//
	// Looks like Plane.makeWinding() may do it!
	gs->Begin(N_PTYPE_LINE_LIST);
	Winding w;
	float size = myPlane->getDistance();

	myPlane->makeWinding( gs->clip_maxz / 2.0, w);
	
    gs->Rgba(0.4f,0.4f,0.4f,0.3f);
	gs->Coord( w[0].position.x, w[0].position.y, w[0].position.z );
	gs->Coord( w[1].position.x, w[1].position.y, w[1].position.z );

	gs->Coord( w[1].position.x, w[1].position.y, w[1].position.z );
	gs->Coord( w[2].position.x, w[2].position.y, w[2].position.z );

	gs->Coord( w[2].position.x, w[2].position.y, w[2].position.z );
	gs->Coord( w[0].position.x, w[0].position.y, w[0].position.z );
	
	gs->Coord( w[0].position.x, w[0].position.y, w[0].position.z );
	gs->Coord( w[3].position.x, w[3].position.y, w[3].position.z );

	gs->Coord( w[3].position.x, w[3].position.y, w[3].position.z );
	gs->Coord( w[2].position.x, w[2].position.y, w[2].position.z );

	gs->Coord( w[2].position.x, w[2].position.y, w[2].position.z );
	gs->Coord( w[1].position.x, w[1].position.y, w[1].position.z );

	gs->Coord( w[1].position.x, w[1].position.y, w[1].position.z );
	gs->Coord( w[3].position.x, w[3].position.y, w[3].position.z );

	gs->End();

}

// Build the ODE object based on the information known for this subclass.
// Since we can share this class among many nCollideObjects, we don't hold
// the information locally, but instead let the nCollideObject ask for this
// data.  That way one shape can still feed MANY objects.
dGeomID nOdePlaneCollideShape::buildOdeObject()
{
	vector3 normal = myPlane->getNormal();
	return( dCreatePlane( 0, normal.x, normal.y, normal.z, myPlane->getDistance()) );
}

// Uses mass information to build a proper mass object for this kind of shape
void nOdePlaneCollideShape::buildOdeMass( dMass *mass )
{
	// Planes are massless entities for now.
	dMassSetZero( mass );
}


/******************************************************************************************
										TriList Liberty
 ******************************************************************************************/

nOdeTriListCollideShape::~nOdeTriListCollideShape()
{
}


void nOdeTriListCollideShape::Visualize(nGfxServer *gs)
{
	// Spherical representations are handled by the context
	// which can be confusing, so I overlay a purple one 
	// to distinguish the two..
    gs->Begin(N_PTYPE_LINE_LIST);
	
	int face = 0;
	for( int i = 0; i < numIndices; i += 3, face++) 
	{
		if (testFaces.find(face) != testFaces.end())
			gs->Rgba(1.0f,1.0f,0.0f,0.3f);
		else if (hitFaces.find(face) != hitFaces.end() || (lastHitFace == face) )
			gs->Rgba(1.0f,0.0f,0.0f,0.3f);
		else 
			gs->Rgba(0.4f,0.4f,0.4f,0.3f);

			gs->Coord(myVertices[myIndices[i]].x,myVertices[myIndices[i]].y,myVertices[myIndices[i]].z);
            gs->Coord(myVertices[myIndices[i+1]].x,myVertices[myIndices[i+1]].y,myVertices[myIndices[i+1]].z);

            gs->Coord(myVertices[myIndices[i+1]].x,myVertices[myIndices[i+1]].y,myVertices[myIndices[i+1]].z);
            gs->Coord(myVertices[myIndices[i+2]].x,myVertices[myIndices[i+2]].y,myVertices[myIndices[i+2]].z);

            gs->Coord(myVertices[myIndices[i+2]].x,myVertices[myIndices[i+2]].y,myVertices[myIndices[i+2]].z);
            gs->Coord(myVertices[myIndices[i]].x,myVertices[myIndices[i]].y,myVertices[myIndices[i]].z);
	}
    gs->End();

	testFaces.clear();
	hitFaces.clear();
}

// Build the ODE object based on the information known for this subclass.
// Since we can share this class among many nCollideObjects, we don't hold
// the information locally, but instead let the nCollideObject ask for this
// data.  That way one shape can still feed MANY objects.
dGeomID nOdeTriListCollideShape::buildOdeObject()
{
	// If dTriCallBack or dTriArrayCallback are null, it is ok
	dGeomID id = dCreateTriList( 0, &dTriCallback, &dTriArrayCallback, &dTriHitCallback/*myTriRayCallback*/ );
	int vertexStride = sizeof(dcVector3);
	int indexStride = sizeof(int);
	int triStride = 3*sizeof(int);
	dGeomTriListBuild( id, myVertices, vertexStride, numVertices, myIndices, indexStride, numIndices, triStride );


	return(id); 
}

// Uses mass information to build a proper mass object for this kind of shape
void nOdeTriListCollideShape::buildOdeMass( dMass *mass )
{
	if (!myOverrideDefaultMass)
	{
		dMassSetSphere( mass, myDensity, this->GetRadius() );
	} else {
		dMassSetParameters( mass, myMass, 
							myCenter.x, myCenter.y, myCenter.z, 
							myInertia.M11, myInertia.M22, myInertia.M33,
							myInertia.M12, myInertia.M13, myInertia.M23 );
	}
}

void nOdeTriListCollideShape::Begin( int numv, int numt)
{
	myVertices = new dcVector3[ numv ];
	numVertices = numv;

	myIndices = new int[numt * 3];
	numIndices = numt * 3;

	nCollideShape::Begin( numv, numt );
}

void nOdeTriListCollideShape::SetVertex(int id, vector3& v0)
{
	memcpy(&myVertices[id], &v0, sizeof(dVector3));

	nCollideShape::SetVertex( id, v0);
	myAab.extendToFit( v0 );
}

void nOdeTriListCollideShape::SetTriangle(int id, int v0, int v1, int v2)
{
	int base = id * 3;
	myIndices[ base ] = v0;
	myIndices[ base + 1] = v1;
	myIndices[ base + 2] = v2;

	nCollideShape::SetTriangle( id, v0, v1, v2 );
}


void nOdeTriListCollideShape::End()
{
	nCollideShape::End();
	float theBoxSides[3];

	theBoxSides[0] = fabs(myAab[3] - myAab[0]);
	theBoxSides[1] = fabs(myAab[4] - myAab[1]);
	theBoxSides[2] = fabs(myAab[5] - myAab[2]);

	if (myUseComputedCenter)
	{
		myMatrix.M41 = myAab[3] + ( theBoxSides[0] * 0.5 );
		myMatrix.M42 = myAab[4] + ( theBoxSides[1] * 0.5 );
		myMatrix.M43 = myAab[5] + ( theBoxSides[2] * 0.5 );

	}
}

void nOdeTriListCollideShape::setTestFaces( const int *faces, int numFaces )
{
	for (int i = 0; i < numFaces; i++ )
	{
		testFaces.insert( faces[i] );
	}
}

void nOdeTriListCollideShape::setHitFace( int face )
{
	
	if (testFaces.find( face ) != testFaces.end())
		testFaces.erase( face );

	hitFaces.insert( face );
	lastHitFace = face;
}


void nOdeTriListCollideShape::dTriArrayCallback(dGeomID TriList, dGeomID RefObject, const int* TriIndices, int TriCount)
{

	nOdeTriListCollideShape *self = (nOdeTriListCollideShape *)((nOdeCollideObject *)dGeomGetData(TriList))->GetShape();
	self->setTestFaces( TriIndices, TriCount );
}

int nOdeTriListCollideShape::myTriRayCallback(dGeomID TriList, dGeomID Ray, int TriangleIndex, dReal u, dReal v)
{
	// nlin - copied from outdated dTriHitCallback - hope it is correct
	nOdeTriListCollideShape *self = (nOdeTriListCollideShape *)((nOdeCollideObject *)dGeomGetData(TriList))->GetShape();
	self->setHitFace( TriangleIndex );
}

void nOdeTriListCollideShape::dTriHitCallback(dGeomID TriList, dGeomID RefObject, int TriIndex)
{

	nOdeTriListCollideShape *self = (nOdeTriListCollideShape *)((nOdeCollideObject *)dGeomGetData(TriList))->GetShape();
	self->setHitFace( TriIndex );
}

int nOdeTriListCollideShape::dTriCallback(dGeomID TriList, dGeomID RefObject, int TriangleIndex)
{
	return 1;
}

