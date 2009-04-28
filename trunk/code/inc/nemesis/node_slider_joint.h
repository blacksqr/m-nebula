//==============================================================================
//  misc/nOdeSliderJoint.h
//  author: White Gold (Ken Melms)
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

#ifndef N_ODE_SLIDER_JOINT_H
#define N_ODE_SLIDER_JOINT_H


// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/common.h"

#undef 	 N_DEFINES
#define N_DEFINES nOdeSliderJoint
#include "kernel/ndefdllclass.h"

#include "nemesis/node_joint.h"

/*------------------------------------------------------------------------------
		Defines a Slider Joint, and accessors to set this joints parameters
  ------------------------------------------------------------------------------*/
class N_PUBLIC nOdeSliderJoint : public nOdeJoint
{
	public:
		nOdeSliderJoint();
		virtual ~nOdeSliderJoint();

		static nClass* pClass;
		static nKernelServer* pKernelServer;
		
		void setAxis( vector3 *anchor );
		vector3 getAxis();
	
		float getSliderPosition();
		float getSliderPositionRate();

	protected:
		virtual void setInitialJointParameters();
		vector3 myAxis;
};

//------------------------------------------------------------------------------
#endif
