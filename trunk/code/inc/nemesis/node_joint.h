//==============================================================================
//  misc/nOdeJoint.h
//  author: White Gold (Ken Melms)
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------
#pragma warning ( disable : 4244 )

#ifndef N_ODE_JOINT_H
#define N_ODE_JOINT_H

#include "nemesis/node_physics.h"

// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/common.h"

#undef 	 N_DEFINES
#define N_DEFINES nOdeJoint
#include "kernel/ndefdllclass.h"

#include "kernel/nroot.h"
#include "kernel/nref.h"

class nOdeCollideObject;

/*------------------------------------------------------------------------------
	Defines the base class for all "Joints" used by ODE.  Subclasses will 
	properly derive and define how to access and set parameters on those joints.

	Use of the base class is a "fixed" joint, meaning that this joint will "fix"
	a body into a position relative to the other body.
  ------------------------------------------------------------------------------*/
class N_PUBLIC nOdeJoint : public nRoot
{
	public:
		nOdeJoint();
		virtual ~nOdeJoint();

		static nClass* pClass;
		static nKernelServer* pKernelServer;

		enum nOdeJointType {
		  nOdeJointTypeNone = 0,		/* or "unknown" */
		  nOdeJointTypeBall,
		  nOdeJointTypeHinge,
		  nOdeJointTypeSlider,
		  nOdeJointTypeContact,
		  nOdeJointTypeHinge2,
		  nOdeJointTypeFixed,
		};

	public:
		
		virtual dJointID getJointID()
		{
			return myJointID;
		}

		virtual nOdeJointType getJointType()
		{

			return nOdeJointTypeFixed;
		}
		
		virtual void setJointParameter( int param, float value );
		void connectJointToObjects( nOdeCollideObject *obj1, nOdeCollideObject *obj2 );
		
		// Setters / Getters (none for a fixed joint)

	protected:
		dJointID myJointID;

		nAutoRef< nOdePhysics > myRefPhyz;

		// Called by connectJoint to finalize the initialization
		// of the joint once it's attached.  Subclasses will override
		// this to set their hinge/ball etc parameters.  This means
		// that the parameters MUST be set before calling "connect"
		// or they will be ignored.
		virtual void setInitialJointParameters();
};


//------------------------------------------------------------------------------
#endif
