//==============================================================================
//  misc/nOdeHingeJoint.h
//  author: White Gold (Ken Melms)
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

#ifndef N_ODE_HINGE2_JOINT_H
#define N_ODE_HINGE2_JOINT_H


// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/common.h"

#undef 	 N_DEFINES
#define N_DEFINES nOdeHinge2Joint
#include "kernel/ndefdllclass.h"

#include "nemesis/node_hinge_joint.h"

/*------------------------------------------------------------------------------
		Defines a Hinge Joint, and accessors to set this joints parameters
  ------------------------------------------------------------------------------*/
class N_PUBLIC nOdeHinge2Joint : public nOdeHingeJoint
{
	public:
		nOdeHinge2Joint();
		virtual ~nOdeHinge2Joint();

		static nClass* pClass;
		static nKernelServer* pKernelServer;
		
		void setAxis2( vector3* anchor );
		vector3 getAxis2();

		virtual float getAngle();
		virtual float getAngleRate();
		virtual float getAngle2Rate();

		virtual void setJointParameter( int param, float value );
	
	protected:
		virtual void setInitialJointParameters();

		vector3 myAxis2;
};


//------------------------------------------------------------------------------
#endif
