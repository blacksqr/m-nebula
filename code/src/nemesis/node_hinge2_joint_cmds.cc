#define N_IMPLEMENTS nOdeHingeJoint
//==============================================================================
//  subdir/nOdeHingeJoint_cmds.cc
//  author: Your Name
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_hinge_joint.h"

#include "kernel/nclass.h"

static void n_setaxis(void* vpObj, nCmd* pCmd);
static void n_getaxis(void* vpObj, nCmd* pCmd);
static void n_getangle(void* vpObj, nCmd* pCmd);
static void n_getanglerate(void* vpObj, nCmd* pCmd);

//==============================================================================
//  CLASS
//  nOdeJoint
//  SUPERCLASS
//  nsuperclassname
//  INCLUDE
//  subdir/nOdeJoint.h
//  INFO
//
//------------------------------------------------------------------------------
void
n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setaxis2_fff",   'SAXX', n_setaxis);
    cl->AddCmd("fff_getaxis2_v",   'GAXX', n_getaxis);
    cl->AddCmd("f_getanglerate2_v",'GART', n_getanglerate);
    cl->EndCmds();
}

static void n_setaxis(void* vpObj, nCmd* pCmd)
{
	nOdeHingeJoint* pSelf = static_cast<nOdeHingeJoint*>(vpObj);
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	pSelf->setAxis(&vector3(x,y,z));
}

static void n_getaxis(void* vpObj, nCmd* pCmd)
{
	nOdeHingeJoint* pSelf = static_cast<nOdeHingeJoint*>(vpObj);
	
	vector3 v(pSelf->getAxis());
	pCmd->Out()->SetF( v.x );
	pCmd->Out()->SetF( v.y );
	pCmd->Out()->SetF( v.z );
}

static void n_getangle(void* vpObj, nCmd* pCmd)
{
	nOdeHingeJoint* pSelf = static_cast<nOdeHingeJoint*>(vpObj);
	
	pCmd->Out()->SetF( pSelf->getAngle() );
}

static void n_getanglerate(void* vpObj, nCmd* pCmd)
{
	nOdeHingeJoint* pSelf = static_cast<nOdeHingeJoint*>(vpObj);
	
	pCmd->Out()->SetF( pSelf->getAngleRate() );
}
