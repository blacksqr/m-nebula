#define N_IMPLEMENTS nOdePhysics
//==============================================================================
//  subdir/nRenderableObject_init.cc
//  author: Your Name
//  (C) 2001 Forged Reality
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_physics.h"

//  global declarations
static void n_setgravity(void*, nCmd*);
static void n_getgravity(void*, nCmd*);
static void n_seterp(void*, nCmd*);
static void n_setcfm(void*, nCmd*);
static void n_geterp(void*, nCmd*);
static void n_getcfm(void*, nCmd*);
static void n_setmaxsteptime(void*, nCmd*);
static void n_getmaxsteptime(void*, nCmd*);


//==============================================================================
//  CLASS
//  nOdePhysics
//  SUPERCLASS
//  nsuperclassname
//  INCLUDE
//  subdir/nOdePhysics.h
//  INFO
//
//  Wraps and encompasses Russel Smith's ODE physics engine, as well as 
//	his collision detection and response model.
//
//	Please refer to www.q12.org for further documentation.
//  
//------------------------------------------------------------------------------
void
n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setgravity_fff", 'PSGR', n_setgravity);
    cl->AddCmd("fff_getgravity_v", 'PGGR', n_getgravity);
    cl->AddCmd("v_seterp_f",       'PSER', n_seterp);
    cl->AddCmd("v_setcfm_f",       'PSCF', n_setcfm);
    cl->AddCmd("f_geterp_v",       'PGER', n_geterp);
    cl->AddCmd("f_getcfm_v",       'PGCF', n_getcfm);
    cl->AddCmd("v_setmaxsteptime_f",'PSMS', n_setmaxsteptime);
    cl->AddCmd("f_getmaxsteptime_v",'PGMS', n_getmaxsteptime);
    cl->EndCmds();
}

//==============================================================================
//  CMD
//  setgravity
//  INPUT
//  fff
//  OUTPUT
//  v
//  INFO
//  Sets the gravity vector for the world.  Described in X/Y/Z float values as
//  the vector of force to influence ALL non-static objects each frame.
//------------------------------------------------------------------------------
static void n_setgravity(void* vpObj, nCmd* pCmd)
{
    nOdePhysics* pSelf = static_cast<nOdePhysics*>(vpObj);
    pSelf->setGravity( pCmd->In()->GetF(), pCmd->In()->GetF(), pCmd->In()->GetF() );
}

//==============================================================================
//  CMD
//  getgravity
//  INPUT
//  v
//  OUTPUT
//  fff
//  INFO
//  Gets the gravity vector for the world.  Described in X/Y/Z float values as
//  the vector of force to influence ALL non-static objects each frame.
//------------------------------------------------------------------------------
static void n_getgravity(void* vpObj, nCmd* pCmd)
{
    nOdePhysics* pSelf = static_cast<nOdePhysics*>(vpObj);
    vector3 gravity( pSelf->getGravity() );
	pCmd->Out()->SetF( gravity.x );
	pCmd->Out()->SetF( gravity.y );
	pCmd->Out()->SetF( gravity.z );
}

//==============================================================================
//  CMD
//  seterp
//  INPUT
//  f
//  OUTPUT
//  v
//  INFO
//  Sets the ODE "ERP" value, described inthe documentation as:
//
//  Set the global ERP value, that controls how much error correction is
//  performed in each time step.
//  Typical values are in the range 0.1--0.8.
//  The default is 0.2.
//
//  Please see the ODE documentation for further information on how the ERP 
//	effects stability
//------------------------------------------------------------------------------
static void n_seterp(void* vpObj, nCmd* pCmd)
{
    nOdePhysics* pSelf = static_cast<nOdePhysics*>(vpObj);
    pSelf->setWorldERP( pCmd->In()->GetF() );
}

//==============================================================================
//  CMD
//  setcfm
//  INPUT
//  f
//  OUTPUT
//  v
//  INFO
//  Sets the ODE "CFM" (constraint force mixing) value, described in 
//	the ODE documentation as:
//
//  Set the global CFM (constraint force mixing) value.
//  Typical values are in the range 1e-9 -- 1.
//  The default is 1e-5 
//
//  Please see the ODE documentation for further information on how the CFM
//	effects stability
//------------------------------------------------------------------------------
static void n_setcfm(void* vpObj, nCmd* pCmd)
{
    nOdePhysics* pSelf = static_cast<nOdePhysics*>(vpObj);
    pSelf->setWorldCFM( pCmd->In()->GetF() );
}

//==============================================================================
//  CMD
//  geterp
//  INPUT
//  v
//  OUTPUT
//  f
//  INFO
//  Gets the ODE "ERP" value, described inthe documentation as:
//
//  Get the global ERP value, that controls how much error correction is
//  performed in each time step.
//  Typical values are in the range 0.1--0.8.
//  The default is 0.2.
//
//  Please see the ODE documentation for further information on how the ERP 
//	effects stability
//------------------------------------------------------------------------------
static void n_geterp(void* vpObj, nCmd* pCmd)
{
    nOdePhysics* pSelf = static_cast<nOdePhysics*>(vpObj);
    pCmd->Out()->SetF( pSelf->getWorldERP() );
}

//==============================================================================
//  CMD
//  getcfm
//  INPUT
//  v
//  OUTPUT
//  f
//  INFO
//  Gets the ODE "CFM" (constraint force mixing) value, described in 
//	the ODE documentation as:
//
//  Get the global CFM (constraint force mixing) value.
//  Typical values are in the range 1e-9 -- 1.
//  The default is 1e-5 
//
//  Please see the ODE documentation for further information on how the CFM
//	effects stability
//------------------------------------------------------------------------------
static void n_getcfm(void* vpObj, nCmd* pCmd)
{
    nOdePhysics* pSelf = static_cast<nOdePhysics*>(vpObj);
    pCmd->Out()->SetF( pSelf->getWorldCFM() );
}

//==============================================================================
//  CMD
//  setmaxsteptime
//  INPUT
//  f
//  OUTPUT
//  v
//  INFO
//  
//  Sets the maximum time allowed for a physics step of the world.  Any frame 
//  time value below this mark will step normally, any frame time above this
//  value will slice the physics step into steps of at most this value
//
//------------------------------------------------------------------------------
static void n_setmaxsteptime(void* vpObj, nCmd* pCmd)
{
    nOdePhysics* pSelf = static_cast<nOdePhysics*>(vpObj);
    pSelf->setWorldMaxStepTime( pCmd->In()->GetF() );
}

//==============================================================================
//  CMD
//  getmaxsteptime
//  INPUT
//  v
//  OUTPUT
//  f
//  INFO
//  
//  Gets the maximum time allowed for a physics step of the world.  Any frame 
//  time value below this mark will step normally, any frame time above this
//  value will slice the physics step into steps of at most this value
//
//------------------------------------------------------------------------------
static void n_getmaxsteptime(void* vpObj, nCmd* pCmd)
{
    nOdePhysics* pSelf = static_cast<nOdePhysics*>(vpObj);
    pCmd->Out()->SetF( pSelf->getWorldMaxStepTime() );
}
