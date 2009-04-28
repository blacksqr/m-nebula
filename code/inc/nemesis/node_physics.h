//==============================================================================
//  misc/nOdePhysics.h
//  author: White Gold (Ken Melms)
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------
#pragma warning ( disable : 4244 )

#ifndef N_ODE_PHYSICS_H
#define N_ODE_PHYSICS_H


// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/common.h"

#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "collide/ncollidereporthandler.h"
#include "collide/ncollideserver.h"

#include <map>
#include <set>

//  forward references
class nCollideServer;
class nCollideObject;
class nCollideContext;

#undef 	 N_DEFINES
#define N_DEFINES nOdePhysics
#include "kernel/ndefdllclass.h"


// Should this be called from the COLLIDE SERVER as it derives new contacts, or
// shall we let the Actor's physics add these as it needs.
//
//
// 1 -- Collide server adds
//	   pros - Actor doesn't need to concern himself with generic bumping and sliding motions
//			  on contact, only with responding via AI.
//     cons - All collisions are added (but perhaps ignored by) the physics system.  Must also
//			  store and track the objects' physical properties, actor can't change himself to 
//			  bouncy based on a certain kind of object collision (the object collided must be
//			  bouncy... so what?)
//
// 2 -- Actor adds
//     pros - Actors retain complete control over their movements, deciding to bounce
//			  or not to bounce, setting surface parameters etc.
//     
//     cons - More complex actor code.  
//
// 3 -- Physics Server polls Collide server for all collisions.  Then for each physics object
//		being tracked, it finds earlist collision, and adds a joint for it.. <DING>

/*------------------------------------------------------------------------------
	Issues -- 
		Exact Poly:Poly collision detection is currently unavailable until we
		determine what to do about solving for penetration depth in such 
		instances.
  ------------------------------------------------------------------------------*/
class N_PUBLIC nOdePhysics : public nCollideServer
{
	protected:
		static int unique;
		
		nAutoRef<nCollideServer> myRefCs;
		
		int myNemesisClassNum;

		dWorldID myWorld;
//		dSpaceID myDefaultSpace; // Default 'Context'

		dJointGroupID myCollisionJoints;

		static double myLastTime; // time of last step 

		static void nearCallback(void *data, dGeomID o1, dGeomID o2);

		float myMaxStepTime;

	public: // nCollideServer Overrides
		nOdePhysics();

		virtual ~nOdePhysics();

		static nClass* pClass;
		static nKernelServer* pKernelServer;

		enum nOdeTypes {
			ODE_SPHERE,
			ODE_BOX,
			ODE_CYLINDER,
			ODE_PLANE,
			ODE_TRILIST,
			ODE_UNKNOWN
		};

		virtual nCollideContext *NewContext(void);
		virtual nCollideShape   *NewShape(const char *id);

		// PREFERRED METHOD -- the other older method will only create spheres
		virtual nCollideShape   *NewShape(const char *id, nOdeTypes type);

	public:
		
		dWorldID getWorldID()
		{
			return(myWorld); 
		}

		dJointGroupID getCollJointGroupID()
		{
			return( myCollisionJoints );
		}

		// A normalized vector describing gravity
		// (Mmm.. spinning gravity fed from a nipol?!)
		void setGravity( float x, float y, float z );

		// Set using a real vector3
		void setGravity( vector3 &gravity );

		// Get the current gravity vector (Mmm.. spinning gravity fed from a nipol?!)
		vector3 getGravity();
		
		// Error Reduction Parameter
		void setWorldERP( float erp );
		float getWorldERP();

		// Constraint Force Mixing
		void setWorldCFM( float cfm );
		float getWorldCFM();
		
		float getWorldMaxStepTime()
		{ return myMaxStepTime; };

		void setWorldMaxStepTime( float d )
		{ myMaxStepTime = d; };

		// Called once per frame, if we don't screw it up
		bool Trigger();
};


//------------------------------------------------------------------------------
#endif
