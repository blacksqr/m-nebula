#define N_IMPLEMENTS nOdeBallJoint
//==============================================================================
//  misc/nOdeJoint_main.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_ball_joint.h"

// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/ode.h"

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
nOdeBallJoint::nOdeBallJoint() :
	myAnchor( 0.0f, 0.0f, 0.0f )
{
	myJointID = dJointCreateBall( myRefPhyz->getWorldID(), 0 );
}

//==============================================================================
//  2001.9.27  WhiteGold       created
//------------------------------------------------------------------------------
nOdeBallJoint::~nOdeBallJoint()
{
	// Ball/socket param deletion
}

// Initial init phase called from base class.  Don't call super!!!
// This is called just prior to connecting 2 objects
void nOdeBallJoint::setInitialJointParameters()
{
	dJointSetBallAnchor( myJointID, myAnchor.x, myAnchor.y, myAnchor.z );
	return;
}

void nOdeBallJoint::setAnchor( vector3* anchor )
{
	myAnchor = *anchor;
}

vector3 nOdeBallJoint::getAnchor()
{
	return( myAnchor );
}
