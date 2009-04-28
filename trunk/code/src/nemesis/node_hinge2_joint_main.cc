#define N_IMPLEMENTS nOdeHinge2Joint
//==============================================================================
//  misc/nOdeJoint_main.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_hinge2_joint.h"

// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/ode.h"

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
nOdeHinge2Joint::nOdeHinge2Joint() :
	myAxis2( 0.0f, 1.0f, 0.0f )
{
	myJointID = dJointCreateHinge2( myRefPhyz->getWorldID(), 0 );
}

//==============================================================================
//  2001.9.27  WhiteGold       created
//------------------------------------------------------------------------------
nOdeHinge2Joint::~nOdeHinge2Joint()
{
}

// Initial init phase called from base class.  Don't call super!!!
// This is called just prior to connecting 2 objects
void nOdeHinge2Joint::setInitialJointParameters()
{
	dJointSetHinge2Anchor( myJointID, myAnchor.x, myAnchor.y, myAnchor.z );
	dJointSetHinge2Axis1( myJointID, myAxis.x, myAxis.y, myAxis.z);
	dJointSetHinge2Axis2( myJointID, myAxis2.x, myAxis2.y, myAxis2.z);
	return;
}

void nOdeHinge2Joint::setJointParameter( int param, float value ){
	dJointSetHinge2Param(myJointID, param, value );
}

void nOdeHinge2Joint::setAxis2( vector3* axis2 )
{
	myAxis2 = *axis2;
}

vector3 nOdeHinge2Joint::getAxis2()
{
	return( myAxis2 );
}

float nOdeHinge2Joint::getAngle()
{
	return( dJointGetHinge2Angle1(myJointID) );
}

float nOdeHinge2Joint::getAngleRate()
{
	return( dJointGetHinge2Angle1Rate(myJointID) );
}

float nOdeHinge2Joint::getAngle2Rate()
{
	return( dJointGetHinge2Angle2Rate(myJointID) );
}
