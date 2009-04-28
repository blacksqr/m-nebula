//==============================================================================
//  misc/nOdeHingeJoint.h
//  author: White Gold (Ken Melms)
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

#ifndef N_ODE_HINGE_JOINT_H
#define N_ODE_HINGE_JOINT_H


// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/common.h"

#undef 	 N_DEFINES
#define N_DEFINES nOdeHingeJoint
#include "kernel/ndefdllclass.h"

#include "nemesis/node_ball_joint.h"

/*------------------------------------------------------------------------------
		Defines a Hinge Joint, and accessors to set this joints parameters
  ------------------------------------------------------------------------------*/
class N_PUBLIC nOdeHingeJoint : public nOdeBallJoint
{
	public:
		nOdeHingeJoint();
		virtual ~nOdeHingeJoint();

		static nClass* pClass;
		static nKernelServer* pKernelServer;
		
		void setAxis( vector3* anchor );
		vector3 getAxis();

		virtual float getAngle();
		virtual float getAngleRate();

		virtual void setJointParameter( int param, float value );

	protected:
		virtual void setInitialJointParameters();

		vector3 myAxis;
};


//------------------------------------------------------------------------------
#endif
