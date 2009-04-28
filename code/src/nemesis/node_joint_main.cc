#define N_IMPLEMENTS nOdeJoint
//==============================================================================
//  misc/nOdeJoint_main.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_joint.h"
#include "nemesis/node_collideobject.h"

// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/ode.h"

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
nOdeJoint::nOdeJoint() :
	myRefPhyz( pKernelServer, this )
{
	myRefPhyz = "/sys/servers/physics";

	myJointID = dJointCreateFixed( myRefPhyz->getWorldID(), 0 );
}

//==============================================================================
//  2001.9.27  WhiteGold       created
//------------------------------------------------------------------------------
nOdeJoint::~nOdeJoint()
{
	if (myJointID && myRefPhyz.isvalid())
		dJointDestroy( myJointID );
}

// Body 2 can be NULL - this will fix the given to the object to the world
void nOdeJoint::connectJointToObjects( nOdeCollideObject *obj1, nOdeCollideObject *obj2)
{
	n_assert( myJointID );

	dGeomID g1, g2 = NULL;
	dBodyID b1, b2 = NULL;

	g1 = obj1->getOdeGeometryID();
	b1 = dGeomGetBody( g1 );
		
	if (obj2)
	{
		g2 = obj2->getOdeGeometryID();
		b2 = dGeomGetBody( g2 );
	}

	dJointAttach( myJointID, b1, b2 );

	setInitialJointParameters();
}

// Initial init phase.  Don't call super!!!
void nOdeJoint::setInitialJointParameters()
{
	dJointSetFixed( myJointID );
	return;
}

void nOdeJoint::setJointParameter( int param, float value )
{
	//dJointSetParameter( param, value );
	// No parameters for a fixed joint.. 
}
