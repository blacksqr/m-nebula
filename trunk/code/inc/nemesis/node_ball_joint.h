//==============================================================================
//  misc/nOdeBallJoint.h
//  author: White Gold (Ken Melms)
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

#ifndef N_ODE_BALL_JOINT_H
#define N_ODE_BALL_JOINT_H


// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/common.h"

#undef 	 N_DEFINES
#define N_DEFINES nOdeBallJoint
#include "kernel/ndefdllclass.h"

#include "nemesis/node_joint.h"

/*------------------------------------------------------------------------------
		Defines a Ball Joint, and accessors to set this joints parameters
  ------------------------------------------------------------------------------*/
class N_PUBLIC nOdeBallJoint : public nOdeJoint
{
	public:
		nOdeBallJoint();
		virtual ~nOdeBallJoint();

		static nClass* pClass;
		static nKernelServer* pKernelServer;
		
		void setAnchor( vector3* anchor );
		vector3 getAnchor();

	protected:
		virtual void setInitialJointParameters();


		vector3 myAnchor;
		
};


//------------------------------------------------------------------------------
#endif
