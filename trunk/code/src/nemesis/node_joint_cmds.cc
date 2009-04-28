#define N_IMPLEMENTS nOdeJoint
//==============================================================================
//  subdir/nRenderableObject_init.cc
//  author: Your Name
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_joint.h"
#include "kernel/nclass.h"
#include "nemesis/nodenode.h"

struct str2param {
    const char *str;
    int param;
};

static struct str2param str2param_table[] =
{
	{ "lowstop",			dParamLoStop },
	{ "highstop",			dParamHiStop },
	{ "velocity",			dParamVel },
	{ "maxforce",			dParamFMax },
	{ "fudgefactor",		dParamFudgeFactor },
	{ "bounce",				dParamBounce },
	{ "stoperp",			dParamStopERP },
	{ "stopcfm",			dParamStopCFM },
	{ "suspensionerp",		dParamSuspensionERP },
	{ "suspensioncfm",		dParamSuspensionCFM },

	{ "lowstop2",			dParamLoStop2 },
	{ "highstop2",			dParamHiStop2 },
	{ "velocity2",			dParamVel2 },
	{ "maxforce2",			dParamFMax2 },
	{ "fudgefactor2",		dParamFudgeFactor2 },
	{ "bounce2",			dParamBounce2 },
	{ "stoperp2",			dParamStopERP2 },
	{ "stopcfm2",			dParamStopCFM2 },
	{ "suspensionerp2",		dParamSuspensionERP2 },
	{ "suspensioncfm2",		dParamSuspensionCFM2 },

	{ NULL,                 -1 }
};

static int str2param( const char *str )
{
    int i=0;
    struct str2param *p = NULL;
    while ( p = &(str2param_table[i++]), p->str )
	{
        if ( strcmp( p->str, str ) == 0 )
			return p->param;
    }

    return -1;
}


static void n_connect(void*, nCmd*);
static void n_setjointparam(void*, nCmd*);

//==============================================================================
//  CLASS
//  nOdeJoint
//  SUPERCLASS
//  nsuperclassname
//  INCLUDE
//  subdir/nOdeJoint.h
//  INFO
//
//  
//------------------------------------------------------------------------------
void
n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_connect_ss", 'JCNT', n_connect);
    cl->AddCmd("v_setjointparam_sf", 'JPAR', n_setjointparam);
    cl->EndCmds();
}

static void n_setjointparam(void* vpObj, nCmd* pCmd)
{
	nOdeJoint* pSelf = static_cast<nOdeJoint*>(vpObj);
	
	int param   = str2param( pCmd->In()->GetS() );
	float value = pCmd->In()->GetF();
	
	pSelf->setJointParameter( param, value );
}

static void n_connect(void* vpObj, nCmd* pCmd)
{
	nOdeJoint* pSelf = static_cast<nOdeJoint*>(vpObj);
	const char *o1, *o2;
	o1 = pCmd->In()->GetS();
	o2 = pCmd->In()->GetS();

	nRoot *temp = nOdeJoint::pKernelServer->Lookup( o1 );
	n_assert( temp );

	if (temp->IsA( nOdeNode::pClass ) )
	{
		nOdeCollideObject *co1, *co2 = NULL;
		nOdeNode *ob1 = (nOdeNode*)temp;
		
		if ( co1 = (nOdeCollideObject *)ob1->getCollideObject() )
		{
			if ( (temp = nOdeJoint::pKernelServer->Lookup( o2 )) && temp->IsA(nOdeNode::pClass) )
			{
				co2 = (nOdeCollideObject *)((nOdeNode *)temp)->getCollideObject();
			}
			// CONNECT!
			pSelf->connectJointToObjects( co1, co2 );
		} else { 
			// Error - not found!
			n_printf("nOdeJoint:: Object %s doesn't have a collision shape to attach joint to", o1);
		}
	} else {
		// Error - not a nRenderable!!
		n_printf("nOdeJoint:: Object %s doesn't exist or isn't an nOdeNode\n", o1);
	}
}
