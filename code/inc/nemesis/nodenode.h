//==============================================================================
//  misc/nrenderableobject.h
//  author: White Gold (Ken Melms)
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

#ifndef N_NODENODE_H
#define N_NODENODE_H

//  includes
#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#include "mathlib/vector.h"
#include "gfx/ngfxserver.h"
#include "nemesis/node_physics.h"

#include <vector>

using namespace std;

//  forward references
class nSGNode;
class nSceneGraph;
class nMeshNode;
class nTexArrayNode;
class nShaderNode;
class nOdeCollideServer;
class nCollideShape;
class nOdeCollideContext;	
class nOdeCollideObject;
class nCollideObject;
class nOdePhysics;
class n3DNode;
class Plane;
class nOdeJoint;
class nShadowServer;
class nFileServer2;

#undef 	 N_DEFINES
#define N_DEFINES nOdeNode
#include "kernel/ndefdllclass.h"

#define DEFAULT_VERTEX	-1
#define CYLINDRICAL		 0
#define SHPERICAL		 1
#define PLANAR           2
#define BOX              3

struct JointContainer
{
	nOdeJoint         *joint;
	nOdeCollideObject *otherObject;
	bool			  connected;
};

//------------------------------------------------------------------------------
class N_DLLCLASS nOdeNode : public nVisNode
{
	protected:
	    nAutoRef<nSceneGraph2>      ref_sg;
		nAutoRef<nOdePhysics>		ref_pz;
		nAutoRef<nGfxServer>		ref_gs;
		nAutoRef<nShadowServer>		ref_shadow;
		nAutoRef<nFileServer2>		ref_fs2;

		// Collision Subsystem: Subclasses may put
		// any kind of "nCollideShape" they want, and
		// add themselves to any kind of context they
		// wish -- for now everyone lives in the 
		// default context.
		nCollideShape				*myCollideShape;
		nOdeCollideContext			*myCollideContext;
		nOdeCollideObject 			*myCollideObject;
									
		int							myPhysicsBodyID;
									
		ushort flags;				
									
		n3DNode						*myParent3DNode;
									
		nOdePhysics::nOdeTypes		myShapeType;
		bbox3						myBox;
		float						myRadius;
		float						myHeight;
		float						myCylRadius;

		// A lot of this would be unnecessary if I could just
		// make nMeshNode's "GetVertexBuffer()" inlined load_mesh
		// not break my link phase
		bool						myJointsNeedConnecting;
		vector<JointContainer *>    myJoints;

		nVisNode					*myReference;
		char                        myFileName[N_MAXPATH];
		
		Plane						*myPlane;
									
		bool						visualize;
		bool						collidable;		// Does it collide?
		bool						physicsable;    // Does it move?
		
		matrix44					lastt, debugt;
 
	public:
		nOdeNode();
		virtual ~nOdeNode();
	    virtual bool SaveCmds(nPersistServer *);

		static nClass* pClass;
		static nKernelServer* pKernelServer;
	
		// So we can manually alter the collision BV used
		virtual void setCollisionClass( const char *coll_type );
				
		// We'll be sure our parent depends on us in here :)
		virtual bool Attach(nSceneGraph2 *sg);
		// So we can mack it here.
		virtual void Compute(nSceneGraph2 *sg);

		void setVisualize( bool toVis ){ visualize = toVis; };

		// Safe to cast to a nOdeCollideObject
		nOdeCollideObject *getCollideObject()
		{
			return( myCollideObject );
		}

		// Set the collision information, linking in from the
		// given nVisNode.  Use this OR use the direct setters
		// 
		// type = DEFAULT_VERTEX --
		//        the node will be checked to see if it is a
		//        nMeshNode derivative and if so, it will 
		//        use the vertexbuffer in there to create the
		//        collide shape.
		// type = CYLINDRICAL --
		//        The node will attempt to create a cylindrical
		//        bounding volume.  If the reference node passed 
		//		  in is a nMeshNode derivative, the radius and 
		//        heights will be sensed from the vertex buffer
		//	      otherwise SetHeight() and SetRadius() should be
		//        called
		// type = SPHERICAL --
		//        Same as cylindrical without the height information
		// type = PLANAR --
		//        Using the nVisNode as a base of information,
		//        create a PLANAR Physics shape
		// type = BOX --
		//        Using the nVisNode as a base of information,
		//        create a BOX Physics shape
		void setCollisionLink(nVisNode *visible, nOdePhysics::nOdeTypes type );
		void setCollisionFile(const char *visible, nOdePhysics::nOdeTypes type );

		// Helper class because right now it's IMPOSSIBLE to script
		// 2 EGO objects loading and then linking together with joints
		virtual const char* connectWithFixedJoint( const char *other );
		virtual const char* connectWithBallJoint( const char *other, vector3 *anchor );
		virtual const char* connectWithHingeJoint( const char *other, vector3 *anchor, vector3 *axis );
		virtual const char* connectWithHinge2Joint( const char *other, vector3 *anchor, vector3 *anchor2, vector3 *axis );
		virtual const char* connectWithSliderJoint( const char *other, vector3 *anchor );
//		virtual void connectWithAMotorJoint( const char *other, int mode, vector3 anchor );
		
		// Pass-thru methods to our nOdeCollideObject because it's impossible
		// to get the contained object in EGO/Java.. this is becoming cumbersome
		virtual void addTorque( vector3 *torque );
		virtual void addForce( vector3 *force );

		// MUST call this before Attach()/Compute() is run.
		virtual void setBoundingShapeStyle( nOdePhysics::nOdeTypes shape_style ){ myShapeType = shape_style; };

		// Passes through the information to the CD / Physics system
		// Always valid, always used.  Radius can be determined from
		// a nMeshNode using setCollisionLink()
		virtual void setRadius( float radii ){ myRadius = radii; };
		
		// Passes through the information to the CD / Physics system
		// Validif type is cylindrical.  Height can be determined from
		// a nMeshNode using setCollisionLink()
		virtual void setHeight( float height ){ myHeight = height; };

		// Used if the type is BOX
		virtual void setBoundingBox( bbox3 *box )
		{
			myBox.set( box->vmin, box->vmax );
		}

		// Helper methods to streamline the physics object creation process
		// You Should ALWAYS use the plane helper if you're making a plane.
		void MakeSphere( float radius );
		void MakeCylinder( float radius, float height );
		void MakeBox( float minx, float miny, float minz, float maxx, float maxy, float maxz );
		void MakePlane( float a, float b, float c, float d);
		
		void SetCollideable( bool collide );
		void SetPhysicsable( bool physit  );

		// renders the collision data's bounding volume
		// subclasses may do this in any way they want
		virtual void renderCollisionBoundingVolume( matrix44 &vm );

	protected:
		bool setCollisionData( int type = DEFAULT_VERTEX );

};

// Use me, abuse me.
class WireframeSphere {
public:
	WireframeSphere(const vector3& center, float radius, int lod = 72)
		: myCenter(center), myRadius(radius), myLOD(lod)
	{
	}

	void render(nGfxServer* gs) const {

		gs->Begin(N_PTYPE_LINE_LIST);

		float dr = n_deg2rad(360.0f / (float)myLOD);

		for (float r = 0.0f; r < n_deg2rad(360.0f); r += dr) {
		
			float sin_r0 = (float) sin(r);
			float cos_r0 = (float) cos(r);
			float sin_r1 = (float) sin(r + dr);
			float cos_r1 = (float) cos(r + dr);
			
			vector3 v0_x(0.0f, sin_r0 * myRadius, cos_r0 * myRadius);
			vector3 v1_x(0.0f, sin_r1 * myRadius, cos_r1 * myRadius);
			vector3 v0_y(sin_r0 * myRadius, 0.0f, cos_r0 * myRadius);
			vector3 v1_y(sin_r1 * myRadius, 0.0f, cos_r1 * myRadius);
			vector3 v0_z(sin_r0 * myRadius, cos_r0 * myRadius, 0.0f);
			vector3 v1_z(sin_r1 * myRadius, cos_r1 * myRadius, 0.0f);

			v0_x += myCenter;
			v1_x += myCenter;
			v0_y += myCenter;
			v1_y += myCenter;
			v0_z += myCenter;
			v1_z += myCenter;

			gs->Coord(v0_x.x,v0_x.y,v0_x.z); gs->Coord(v1_x.x,v1_x.y,v1_x.z);
			gs->Coord(v0_y.x,v0_y.y,v0_y.z); gs->Coord(v1_y.x,v1_y.y,v1_y.z);
			gs->Coord(v0_z.x,v0_z.y,v0_z.z); gs->Coord(v1_z.x,v1_z.y,v1_z.z);
		}

		gs->End();
	}

private:
	vector3 myCenter;
	float myRadius;
	int myLOD;
};


// Use me, abuse me.
class WireframeCylinder {
public:
	WireframeCylinder(const vector3& bottomCenter, float radius, float height, int lod = 24)
		: myCenter(bottomCenter), myRadius(radius), myHeight(height), myLOD(lod)
	{
	}

	void render(nGfxServer* gs) const {

		gs->Begin(N_PTYPE_LINE_LIST);

		float dr = n_deg2rad(360.0f / (float)myLOD);

		for (float r = 0.0f; r < n_deg2rad(360.0f); r += dr) {
		
			float sin_r0 = (float) sin(r);
			float cos_r0 = (float) cos(r);
			float sin_r1 = (float) sin(r + dr);
			float cos_r1 = (float) cos(r + dr);

			vector3 v0_y(sin_r0 * myRadius, 0.0f, cos_r0 * myRadius);
			vector3 v1_y(sin_r1 * myRadius, 0.0f, cos_r1 * myRadius);
			v0_y += myCenter;
			v1_y += myCenter;
			gs->Coord(v0_y.x,v0_y.y,v0_y.z);
			gs->Coord(v1_y.x,v1_y.y,v1_y.z);
			gs->Coord(v0_y.x,v0_y.y,v0_y.z);
			v0_y.y += myHeight;
			v1_y.y += myHeight;
			gs->Coord(v0_y.x,v0_y.y,v0_y.z);
			gs->Coord(v0_y.x,v0_y.y,v0_y.z);
			gs->Coord(v1_y.x,v1_y.y,v1_y.z);
		}

		gs->Coord(myCenter.x, myCenter.y, myCenter.z);
		gs->Coord(myCenter.x, myCenter.y + myHeight, myCenter.z);
	
		gs->End();
	}

private:
	vector3 myCenter;
	float myRadius;
	float myHeight;
	int myLOD;
};

//------------------------------------------------------------------------------
#endif
