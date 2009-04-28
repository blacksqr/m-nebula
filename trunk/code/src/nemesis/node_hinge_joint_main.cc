#define N_IMPLEMENTS nOdeHingeJoint
//==============================================================================
//  misc/nOdeJoint_main.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_hinge_joint.h"

// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/ode.h"

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
nOdeHingeJoint::nOdeHingeJoint() :
	myAxis( 0.0f, 1.0f, 0.0f )
{
	myJointID = dJointCreateHinge( myRefPhyz->getWorldID(), 0 );
}

//==============================================================================
//  2001.9.27  WhiteGold       created
//------------------------------------------------------------------------------
nOdeHingeJoint::~nOdeHingeJoint()
{
}

// Initial init phase called from base class.  Don't call super!!!
// This is called just prior to connecting 2 objects
void nOdeHingeJoint::setInitialJointParameters()
{
	dJointSetHingeAnchor( myJointID, myAnchor.x, myAnchor.y, myAnchor.z );
	dJointSetHingeAxis( myJointID, myAxis.x, myAxis.y, myAxis.z);
	return;
}

void nOdeHingeJoint::setJointParameter( int param, float value ){
	dJointSetHingeParam( myJointID, param, value );
}

void nOdeHingeJoint::setAxis( vector3* axis )
{
	myAxis = *axis;
}

vector3 nOdeHingeJoint::getAxis()
{
	return( myAxis );
}

float nOdeHingeJoint::getAngle()
{
	return( dJointGetHingeAngle(myJointID) );
}
float nOdeHingeJoint::getAngleRate()
{
	return( dJointGetHingeAngleRate(myJointID) );
}
