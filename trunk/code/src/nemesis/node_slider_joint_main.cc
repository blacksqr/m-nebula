#define N_IMPLEMENTS nOdeSliderJoint
//==============================================================================
//  misc/nOdeJoint_main.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_slider_joint.h"

// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/ode.h"

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
nOdeSliderJoint::nOdeSliderJoint() :
	myAxis( 0.0f, 0.0f, 0.0f )
{
	myJointID = dJointCreateSlider( myRefPhyz->getWorldID(), 0 );
}

//==============================================================================
//  2001.9.27  WhiteGold       created
//------------------------------------------------------------------------------
nOdeSliderJoint::~nOdeSliderJoint()
{
	// Ball/socket param deletion
}

// Initial init phase called from base class.  Don't call super!!!
// This is called just prior to connecting 2 objects
void nOdeSliderJoint::setInitialJointParameters()
{
	dJointSetSliderAxis( myJointID, myAxis.x, myAxis.y, myAxis.z );
	return;
}

void nOdeSliderJoint::setAxis( vector3* axis )
{
	myAxis = *axis;
}

vector3 nOdeSliderJoint::getAxis()
{
	return( myAxis );
}

float nOdeSliderJoint::getSliderPosition()
{
	return( dJointGetSliderPosition( myJointID ) );
}

float nOdeSliderJoint::getSliderPositionRate()
{
	return( dJointGetSliderPositionRate( myJointID ) );
}
