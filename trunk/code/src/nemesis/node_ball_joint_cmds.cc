#define N_IMPLEMENTS nOdeBallJoint
//==============================================================================
//  subdir/nOdeBallJoint_cmds.cc
//  author: Your Name
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_ball_joint.h"

#include "kernel/nclass.h"

static void n_setanchor(void* vpObj, nCmd* pCmd);
static void n_getanchor(void* vpObj, nCmd* pCmd);

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
    cl->AddCmd("v_setanchor_fff", 'JANC', n_setanchor);
    cl->AddCmd("fff_getanchor_v", 'JANG', n_getanchor);
    cl->EndCmds();
}

static void n_setanchor(void* vpObj, nCmd* pCmd)
{
	nOdeBallJoint* pSelf = static_cast<nOdeBallJoint*>(vpObj);
	
	pSelf->setAnchor( &vector3( pCmd->In()->GetF(), pCmd->In()->GetF(), pCmd->In()->GetF() ));
}

static void n_getanchor(void* vpObj, nCmd* pCmd)
{
	nOdeBallJoint* pSelf = static_cast<nOdeBallJoint*>(vpObj);
	
	vector3 v(pSelf->getAnchor());
	pCmd->Out()->SetF( v.x );
	pCmd->Out()->SetF( v.y );
	pCmd->Out()->SetF( v.z );
}