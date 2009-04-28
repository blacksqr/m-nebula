#define N_IMPLEMENTS nOdeSliderJoint
//==============================================================================
//  subdir/nOdeSliderJoint_cmds.cc
//  author: Your Name
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_slider_joint.h"

#include "kernel/nclass.h"

static void n_setaxis(void* vpObj, nCmd* pCmd);
static void n_getaxis(void* vpObj, nCmd* pCmd);
static void n_getsliderpos(void* vpObj, nCmd* pCmd);
static void n_getsliderrate(void* vpObj, nCmd* pCmd);

//==============================================================================
//  CLASS
//  nOdeSliderJoint
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
    cl->AddCmd("v_setaxis_fff", 'JAXS', n_setaxis);
    cl->AddCmd("fff_getaxis_v", 'JAGX', n_getaxis);
    cl->AddCmd("fff_getsliderpos_v", 'JASP', n_getsliderpos);
    cl->AddCmd("fff_getsliderrate_v", 'JSPR', n_getsliderrate);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
	setaxis

    @input
	f ( axis x value )
	f ( axis y value )
	f ( axis z value )

    @output
    v

    @info
	set the axis of this slider joint

*/
static void n_setaxis(void* vpObj, nCmd* pCmd)
{
	nOdeSliderJoint* pSelf = static_cast<nOdeSliderJoint*>(vpObj);
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	pSelf->setAxis(&vector3( x, y, z));
}

//------------------------------------------------------------------------------
/**
    @cmd
	getaxis

    @input
	v

    @output
	f ( axis x value )
	f ( axis y value )
	f ( axis z value )

    @info
	set the axis of this slider joint

*/
static void n_getaxis(void* vpObj, nCmd* pCmd)
{
	nOdeSliderJoint* pSelf = static_cast<nOdeSliderJoint*>(vpObj);
	
	vector3 v(pSelf->getAxis());
	pCmd->Out()->SetF( v.x );
	pCmd->Out()->SetF( v.y );
	pCmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
	getsliderpos

    @input
	v

    @output
	f ( value )

    @info
	Get the slider position of this joint

*/
static void n_getsliderpos(void* vpObj, nCmd* pCmd)
{
	nOdeSliderJoint* pSelf = static_cast<nOdeSliderJoint*>(vpObj);
	
	pCmd->Out()->SetF( pSelf->getSliderPosition());
}

//------------------------------------------------------------------------------
/**
    @cmd
	getsliderrate

    @input
	v

    @output
	f ( value )

    @info
	Get the slider rate of this joint

*/
static void n_getsliderrate(void* vpObj, nCmd* pCmd)
{
	nOdeSliderJoint* pSelf = static_cast<nOdeSliderJoint*>(vpObj);
	
	pCmd->Out()->SetF( pSelf->getSliderPositionRate());
}