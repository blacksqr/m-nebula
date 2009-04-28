#define N_IMPLEMENTS nOdeNode
//==============================================================================
//  subdir/nOdeNode_init.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "kernel/npersistserver.h"
#include "nemesis/nodenode.h"
#include "nemesis/node_physics.h"
#include "nemesis/node_collideobject.h"

//  global declarations
static void n_getrelpointvel(void *, nCmd*);
static void n_getrelpointpos(void *, nCmd*);
static void n_gettorque(void *, nCmd*);
static void n_getforce(void *, nCmd*);

// Setters
static void n_addforce(void *, nCmd *);
static void n_addtorque(void *, nCmd *);
static void n_addforceatrelpos(void *, nCmd*);
static void n_addforceatpos(void *, nCmd*);
static void n_addrelforceatpos(void *, nCmd*);
static void n_setfiniterotationmode(void *, nCmd*);
static void n_setfiniterotationaxis(void *, nCmd*);

// Getters
static void n_getforce(void *, nCmd*);
static void n_gettorque(void *, nCmd*);
static void n_getrelpointpos(void *, nCmd*);
static void n_getrelpointvel(void *, nCmd*);
static void n_getfiniterotationmode(void *, nCmd*);
static void n_getfiniterotationaxis(void *, nCmd*);
static void n_getnumjoints(void *, nCmd*);
static void n_getjoint(void *, nCmd*);

static void n_setvisualize(void*, nCmd*);
static void n_setcollisionclass(void*, nCmd*);

static void n_connectwithball(void*, nCmd*);
static void n_connectfixed(void*, nCmd*);
static void n_connectwithhinge(void*, nCmd*);
static void n_connectwithhinge2(void*, nCmd*);
static void n_connectwithslider(void*, nCmd*);

static void n_setshapestyle(void *, nCmd *);
static void n_setradius(void *, nCmd *);
static void n_setheight(void *, nCmd *);
static void n_setbox(void *, nCmd *);
static void n_makesphere(void *, nCmd *);
static void n_makecylinder(void *, nCmd *);
static void n_makebox(void *, nCmd *);
static void n_makeplane(void *, nCmd *);

static void n_usecollision(void *, nCmd *);
static void n_usephysics(void *, nCmd *);
static void n_setcontactparam(void *, nCmd *);
static void n_getcontactparam(void *, nCmd *);

static void n_setcollisionlink(void *, nCmd *);
static void n_setcollisionfile(void *, nCmd *);
static void n_maketrilist(void *, nCmd *);

struct str2param {
    const char *str;
	nOdePhysics::nOdeTypes param;
};

static struct str2param str2param_table[] =
{
	{ "sphere",			nOdePhysics::ODE_SPHERE },
	{ "cylinder",		nOdePhysics::ODE_CYLINDER },
	{ "plane",			nOdePhysics::ODE_PLANE },
	{ "box",	        nOdePhysics::ODE_BOX },
	{ "trilist",        nOdePhysics::ODE_TRILIST },

	{ NULL,             nOdePhysics::ODE_UNKNOWN }
};

static nOdePhysics::nOdeTypes str2param( const char *str )
{
    int i=0;
    struct str2param *p = NULL;
    while ( p = &(str2param_table[i++]), p->str )
	{
        if ( strcmp( p->str, str ) == 0 )
			return p->param;
    }

    return nOdePhysics::ODE_UNKNOWN;
};

static const char *param2str( nOdePhysics::nOdeTypes type )
{
    int i=0;
    struct str2param *p = NULL;
    while ( p = &(str2param_table[i++]), p->str )
	{
        if (type == p->param)
			return p->str;
    }

    return NULL;
};

struct str2param2 {
    const char *str;
	int param;
};

static struct str2param2 str2param_table2[] =
{
	{ "mu",				dCParamMu },
	{ "mu2",			dCParamMu2 },
	{ "bounce",			dCParamBounce },
	{ "bouncevel",		dCParamBounceVel },
	{ "softerp",		dCParamSoftErp },
	{ "softcfm",		dCParamSoftCfm },
	{ "motion1",		dCParamMotion1 },
	{ "motion2",		dCParamMotion2 },
	{ "slip1",			dCParamSlip1 },
	{ "slip2",			dCParamSlip2 },

	{ NULL,             -1 }
};

static int str2param2( const char *str )
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

struct str2param3 {
    const char *str;
    int param;
};

static struct str2param3 str2param3_table[] =
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

static int str2param3( const char *str )
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

//==============================================================================
//  CLASS
//  nOdeNode
//  SUPERCLASS
//  nsuperclassname
//  INCLUDE
//  subdir/nOdeNode.h
//  INFO
//  Yeah right.. Where's the better documentation? (see methods below)
//------------------------------------------------------------------------------
void
n_initcmds(nClass *cl)
{
    cl->BeginCmds();

    cl->AddCmd("v_setvisualize_b", 'SMNO', n_setvisualize);
    cl->AddCmd("v_setcollisionclass_s", 'SCOC', n_setcollisionclass);

	// Hand-build a collision shape
    cl->AddCmd("v_setshapetyle_s", 'SCOA', n_setshapestyle);
    cl->AddCmd("v_setradius_f", 'SCOB', n_setradius);
    cl->AddCmd("v_setheight_f", 'SCOD', n_setheight);
    cl->AddCmd("v_setbox_ffffff", 'SCOE', n_setbox);

    cl->AddCmd("v_setcollisionlink_ss", 'SCLK', n_setcollisionlink);
    cl->AddCmd("v_setcollisionfile_ss", 'SCLF', n_setcollisionfile);
    
	// Use built-in shapes
	cl->AddCmd("v_makesphere_f", 'SCOG', n_makesphere);
    cl->AddCmd("v_makecylinder_ff", 'SCOH', n_makecylinder);
    cl->AddCmd("v_makebox_ffffff", 'SCOI', n_makebox);
    cl->AddCmd("v_makeplane_ffff", 'SCOJ', n_makeplane);
    cl->AddCmd("v_maketrilist_s", 'SCTL', n_maketrilist);
	
	// To collide or not to collide
    cl->AddCmd("v_usecollision_b", 'SCOK', n_usecollision);

	// To physics or not to physics
    cl->AddCmd("v_usephysics_b", 'SCOL', n_usephysics);

	// Set this object's surface params (See ODE docs)
    cl->AddCmd("v_setcontactparam_sf",	'SCPR', n_setcontactparam);
    cl->AddCmd("f_getcontactparam_s",	'GCPR', n_getcontactparam);
	
	// Add some forces and axes to the system
    cl->AddCmd("v_addforce_fff",	'ADDF', n_addforce);
    cl->AddCmd("v_addtorque_fff",	'ADDT', n_addtorque);
    cl->AddCmd("v_addforceatpos_ffffff",	'AFAP', n_addforceatpos);
    cl->AddCmd("v_addforceatrelpos_ffffff",	'AFRP', n_addforceatrelpos);
    cl->AddCmd("v_addrelforceatpos_ffffff",	'ARFP', n_addrelforceatpos);
    cl->AddCmd("v_setfiniterotationmode_i",'SFRM', n_setfiniterotationmode);
    cl->AddCmd("v_setfiniterotationaxis_fff",'SFRA', n_setfiniterotationaxis);

	// Get the data back out of the system
    cl->AddCmd("fff_getforce_v",	'GETF', n_getforce);
    cl->AddCmd("fff_gettorque_v",	'GTOQ', n_gettorque);
    cl->AddCmd("fff_getrelpointpos_v",	'GRPP', n_getrelpointpos);
    cl->AddCmd("fff_getrelpointvel_v",	'GRPV', n_getrelpointvel);
    cl->AddCmd("i_getfiniterotationmode_v",'GFRM', n_getfiniterotationmode);
    cl->AddCmd("fff_getfiniterotationaxis_v",'GFRA', n_getfiniterotationaxis);
    cl->AddCmd("i_getnumjoints_v",'GNJO', n_getnumjoints);
    cl->AddCmd("i_getjoints_i",'GJOI', n_getjoint);

//	cl->AddCmd("v_connectwithmotor_ssfff", 'CHIN', n_connectwithslider );
	cl->AddCmd("s_connectwithslider_sfff", 'CSLI', n_connectwithslider );
	cl->AddCmd("s_connectwithhinge2_sfffffffff", 'CHIN', n_connectwithhinge2 );
	cl->AddCmd("s_connectwithhinge_sffffff", 'CHGE', n_connectwithhinge );
	cl->AddCmd("s_connectwithball_sfff", 'CBAL', n_connectwithball );
	cl->AddCmd("s_connectfixed_s", 'CFIX', n_connectfixed );
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    connectwithhinge

    @input
    s ( other nodenode ), 
	f ( anchor x value )
	f ( anchor y value )
	f ( anchor z value )

	f ( axis x value )
	f ( axis y value )
	f ( axis z value )

    @output
    s The name of the joint created (use this to set parameters)

    @info
	Connect this odenode with another odenode via a ODE 
	HINGE joint.  See ODE docs for more details

*/
static void n_connectwithhinge(void* vpObj, nCmd* pCmd)
{
    nOdeNode* pSelf = static_cast<nOdeNode*>(vpObj);
	const char* other = pCmd->In()->GetS();
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	float x2 = pCmd->In()->GetF();
	float y2 = pCmd->In()->GetF();
	float z2 = pCmd->In()->GetF();
	
	pCmd->Out()->SetS(pSelf->connectWithHingeJoint( other, &vector3(x,y,z), &vector3(x2,y2,z2)));
}

//------------------------------------------------------------------------------
/**
    @cmd
    connectwithhinge2

    @input
    s ( other nodenode ), 
	f ( anchor x value )
	f ( anchor y value )
	f ( anchor z value )

	f ( axis x value )
	f ( axis y value )
	f ( axis z value )

	f ( axis2 x value )
	f ( axis2 y value )
	f ( axis2 z value )

    @output
    s The name of the joint created

    @info
	Connect this odenode with another odenode via a ODE 
	HINGE joint.  See ODE docs for more details

*/
static void n_connectwithhinge2(void* vpObj, nCmd* pCmd)
{
    nOdeNode* pSelf = static_cast<nOdeNode*>(vpObj);
	const char* other = pCmd->In()->GetS();
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	float ax = pCmd->In()->GetF();
	float ay = pCmd->In()->GetF();
	float az = pCmd->In()->GetF();
	
	float ax2 = pCmd->In()->GetF();
	float ay2 = pCmd->In()->GetF();
	float az2 = pCmd->In()->GetF();

	pCmd->Out()->SetS(pSelf->connectWithHinge2Joint( other, &vector3(x,y,z), &vector3(ax,ay,az), &vector3(ax2,ay2,az2)));
}
//------------------------------------------------------------------------------
/**
    @cmd
    connectwithball

    @input
    s ( other nodenode ), 
	f ( anchor x value )
	f ( anchor y value )
	f ( anchor z value )

    @output
    s name of the joint created

    @info
	Connect this odenode with another odenode via a ODE 
	BALL joint.  See ODE docs for more details

*/
static void n_connectwithball(void* vpObj, nCmd* pCmd)
{
    nOdeNode* pSelf = static_cast<nOdeNode*>(vpObj);
	const char* other = pCmd->In()->GetS();
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	pCmd->Out()->SetS(pSelf->connectWithBallJoint( other, &vector3(x,y,z)));
}

//------------------------------------------------------------------------------
/**
    @cmd
    connectwithslider

    @input
    s ( other nodenode ), 
	f ( axis x value )
	f ( axis y value )
	f ( axis z value )

    @output
    s Name of the joint 

    @info
	Connect this odenode with another odenode via a ODE 
	SLIDER joint.  See ODE docs for more details

*/
static void n_connectwithslider(void* vpObj, nCmd* pCmd)
{
    nOdeNode* pSelf = static_cast<nOdeNode*>(vpObj);
	const char* other = pCmd->In()->GetS();
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	pCmd->Out()->SetS(pSelf->connectWithSliderJoint( other,&vector3(x,y,z)));
}
//------------------------------------------------------------------------------
/**
    @cmd
    connectfixed

    @input
    s ( other nodenode ), 
	f ( anchor x value )
	f ( anchor y value )
	f ( anchor z value )

	f ( axis x value )
	f ( axis y value )
	f ( axis z value )

    @output
    s The name of the joint created

    @info
	Connect this odenode with another odenode via a ODE 
	FIXED (immovable) joint.  See ODE docs for more details

*/
static void n_connectfixed(void* vpObj, nCmd* pCmd)
{
    nOdeNode* pSelf = static_cast<nOdeNode*>(vpObj);
	pCmd->Out()->SetS(pSelf->connectWithFixedJoint( pCmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcontactparam

    @input
    s ( parameter ), f ( value )

    @output
    v

    @info
    set optional surface parameters for contacts resulting
	from collisions in the space.
	
	see ODE documentation for complete descriptions.
	valid parameters are:

	mu
	mu2
	bounce
	bouncevel
	softerp
	softcfm
	motion1
	motion2
	slip1
	slip2

*/
static
void
n_setcontactparam(void* slf, nCmd* cmd)
{
	nOdeNode* self = static_cast<nOdeNode*>(slf);

    const char* strParam = cmd->In()->GetS();
	float value = cmd->In()->GetF();

	((nOdeCollideObject *)self->getCollideObject())->SetContactParam( str2param2( strParam ), value );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcontactparam

    @input
	s ( parameter )

    @output
    f ( value )

    @info
    get value of an optional parameter for contacts
	resulting from collisions in the space.
	see setcontactparam for valid parameters.
*/
static
void
n_getcontactparam(void* slf, nCmd* cmd)
{
	nOdeNode* self = static_cast<nOdeNode*>(slf);

    const char* strParam = cmd->In()->GetS();

	float value = 0.0;
	value = ((nOdeCollideObject *)self->getCollideObject())->GetContactParam(str2param2( strParam ) );

	cmd->Out()->SetF( value );
}


//==============================================================================
//  CMD
//  setmeshnode
//  INPUT
//  s
//  OUTPUT
//  v
//  INFO
//  Sets the visual nMeshNode to use for this object at render time
//------------------------------------------------------------------------------
static
void
n_setvisualize(void* vpObj, nCmd* pCmd)
{
    nOdeNode* pSelf = static_cast<nOdeNode*>(vpObj);
    pSelf->setVisualize( pCmd->In()->GetB() );
}

//==============================================================================
//  CMD
//  setcollisionclass
//  INPUT
//  s
//  OUTPUT
//  v
//  INFO
//  Sets the class of collisions -- see Startup for list
//------------------------------------------------------------------------------
static
void
n_setcollisionclass(void* vpObj, nCmd* pCmd)
{
    nOdeNode* pSelf = static_cast<nOdeNode*>(vpObj);
    pSelf->setCollisionClass( pCmd->In()->GetS() );
}

//------------------------------------------------------------------------------
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
bool
nOdeNode::SaveCmds(nPersistServer* ps)
{
    if (nVisNode::SaveCmds(ps))
    {
        nCmd* pCmd = 0;

        if ((pCmd = ps->GetCmd(this, 'SMNO')))
        {
            ps->PutCmd(pCmd);
        }
        if ((pCmd = ps->GetCmd(this, 'SCOC')))
        {
            ps->PutCmd(pCmd);
        }
        if ((pCmd = ps->GetCmd(this, 'CVAL')))
        {
            ps->PutCmd(pCmd);
        }
        if ((pCmd = ps->GetCmd(this, 'CFIX')))
        {
            ps->PutCmd(pCmd);
        }
        return true;
    }
    else
	{
        return false;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Manually set the shape of this collision object.  This 
//  shape will then need to have it's radius and possibly
//  its height and other parameters tweaked
//
//  This must be called before Attach() / Compute() is reached.
//  so be sure to fully initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_setshapestyle( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	nOdePhysics::nOdeTypes shape = (nOdePhysics::nOdeTypes) str2param(pCmd->In()->GetS());
	pSelf->setBoundingShapeStyle( shape );	
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Manually set the radius of this collision object.
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized  o be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_setradius( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float radius = pCmd->In()->GetF();
	pSelf->setRadius( radius );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Manually set the height of this collision object. (Cylinders only)
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized  o be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_setheight( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float height = pCmd->In()->GetF();
	pSelf->setHeight( height );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Manually set the min/max bounding box of this collision object.
//
//  INPUT: 
//    fff - The min x/y/z of the bounding box
//    fff - The max x/y/z of the bounding box
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized  o be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_setbox( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float minx = pCmd->In()->GetF();
	float miny = pCmd->In()->GetF();
	float minz = pCmd->In()->GetF();

	float maxx = pCmd->In()->GetF();
	float maxy = pCmd->In()->GetF();
	float maxz = pCmd->In()->GetF();
	
	bbox3 box;
	box.vmin.set( minx, miny, minz );
	box.vmax.set( maxx, maxy, maxz );

	pSelf->setBoundingBox( &box );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Automatically sense the values this collision object will need,
//  given the nVisNode pointer, and the shape style passed in.
//
//  INPUT: 
//    s - The name of the nVisNode to link to
//    s - The shape to syle after (sphere, box, cylinder, plane)
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized  o be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_setcollisionlink( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	const char* linkTo = pCmd->In()->GetS();
	nOdePhysics::nOdeTypes shape = str2param( pCmd->In()->GetS() );	
	
	nClass *visClass = pSelf->pKernelServer->FindClass("nvisnode");
	nRoot  *test     = pSelf->pKernelServer->Lookup( linkTo );

	if (test->IsA( visClass ))
		pSelf->setCollisionLink( (nVisNode*)test, shape );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Automatically sense the values this collision object will need,
//  given the .N3D filename pointer, and the shape style passed in.
//
//  INPUT: 
//    s - The name of the .N3D file to link to
//    s - The shape to style after (sphere, box, cylinder, plane)
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized  o be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_setcollisionfile( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	const char* linkTo = pCmd->In()->GetS();
	nOdePhysics::nOdeTypes shape = str2param( pCmd->In()->GetS() );	
	
	pSelf->setCollisionFile( linkTo, shape );
}


//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Automatically create a sphere with the given radius.
//  No other methods need to be called once this is called.
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized  o be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_makesphere( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float radius = pCmd->In()->GetF();

	pSelf->MakeSphere( radius );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Automatically create a cylinder with the given radius/height.
//  No other methods need to be called once this is called.
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized so be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_makecylinder( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float radius = pCmd->In()->GetF();
	float height = pCmd->In()->GetF();

	pSelf->MakeCylinder( radius, height );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Automatically create a box with the given min/max values.
//  No other methods need to be called once this is called.
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized so be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_makebox( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float minx = pCmd->In()->GetF();
	float miny = pCmd->In()->GetF();
	float minz = pCmd->In()->GetF();

	float maxx = pCmd->In()->GetF();
	float maxy = pCmd->In()->GetF();
	float maxz = pCmd->In()->GetF();
	
	pSelf->MakeBox( minx, miny, minz, maxx, maxy, maxz );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Automatically create a plane with the given radius on the given axis.
//  No other methods need to be called once this is called.
//
//  INPUT:
//    f - the 'a' vector of the plane equation
//    f - the 'b' vector of the plane equation
//    f - the 'c' vector of the plane equation
//    f - the 'd' vector of the plane equation
//
//  From ODE docs: 
//    Create a plane geometry object of the given parameters The plane equation is 
//         a*x+b*y+c*z = d
//
//    The plane's normal vector is (a,b,c), and it must have length 1. Unlike other 
//    geometry objects, planes disregard their assigned position and rotation, i.e. 
//    the parameters are always in global coordinates. In other words it is assumed 
//    that the plane is always part of the static environment and not tied to any 
//    movable object. 
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized so be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_makeplane( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float a = pCmd->In()->GetF();
	float b = pCmd->In()->GetF();
	float c = pCmd->In()->GetF();
	float d = pCmd->In()->GetF();
	
	pSelf->MakePlane( a, b, c, d);
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Automatically create a plane with the given radius on the given axis.
//  No other methods need to be called once this is called.
//
//  INPUT:
//    f - the 'a' vector of the plane equation
//    f - the 'b' vector of the plane equation
//    f - the 'c' vector of the plane equation
//    f - the 'd' vector of the plane equation
//
//  From ODE docs: 
//    Create a plane geometry object of the given parameters The plane equation is 
//         a*x+b*y+c*z = d
//
//    The plane's normal vector is (a,b,c), and it must have length 1. Unlike other 
//    geometry objects, planes disregard their assigned position and rotation, i.e. 
//    the parameters are always in global coordinates. In other words it is assumed 
//    that the plane is always part of the static environment and not tied to any 
//    movable object. 
//
//  This must be called before Attach() / Compute() is reached,
//  as this is where the shape is fully realized so be sure to fully 
//  initialize the object in one pass.
//------------------------------------------------------------------------------
static void n_maketrilist( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	const char *a = pCmd->In()->GetS();
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Add a force to this object represented by a vector3
//
//  INPUT:
//    f - the 'x' vector of the force
//    f - the 'y' vector of the force 
//    f - the 'z' vector of the force 
//
//  From ODE docs: 
//    3.4. Force accumulators
//
//    Between each integrator step the user can call functions to apply 
//    forces to the rigid body. These forces are added to "force accumulators" 
//    in the rigid body object. When the next integrator step happens, the sum 
//    of all the applied forces will be used to push the body around. The forces 
//    accumulators are set to zero after each integrator step.
//
//------------------------------------------------------------------------------
static void n_addforce( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();
	
	pSelf->addForce( &vector3(x,y,z) );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Add a torque to this object represented by a vector3
//
//  INPUT:
//    f - the 'x' vector of the torque
//    f - the 'y' vector of the torque
//    f - the 'z' vector of the torque
//
//  From ODE docs: 
//    3.4. Force accumulators
//
//    Between each integrator step the user can call functions to apply 
//    forces to the rigid body. These forces are added to "force accumulators" 
//    in the rigid body object. When the next integrator step happens, the sum 
//    of all the applied forces will be used to push the body around. The forces 
//    accumulators are set to zero after each integrator step.
//
//------------------------------------------------------------------------------
static void n_addtorque( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();
	
	pSelf->addTorque( &vector3(x,y,z) );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Toggle this object's collidability.  If false, It will still be effected by 
//  gravity, just not as a collide target anymore.
//
//  // TODO: Implement this
//
//------------------------------------------------------------------------------
static void n_usecollision( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Toggle this object's phyiscs.  If false, It will still be effected by 
//  collisions, just not modified by gravity anymore.
//
//------------------------------------------------------------------------------
static void n_usephysics( void* pObj, nCmd* pCmd )
{
	nOdeNode *pSelf = static_cast<nOdeNode*>(pObj);
	pSelf->SetPhysicsable( !pCmd->In()->GetB() );
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Add a force to this object represented by a vector3
//
//  INPUT:
//    f - the 'x' vector of the force
//    f - the 'y' vector of the force 
//    f - the 'z' vector of the force 
//
//    f - the 'x' vector of the position
//    f - the 'y' vector of the position
//    f - the 'z' vector of the position
//
//  From ODE docs: 
//    3.4. Force accumulators
//
//    Between each integrator step the user can call functions to apply 
//    forces to the rigid body. These forces are added to "force accumulators" 
//    in the rigid body object. When the next integrator step happens, the sum 
//    of all the applied forces will be used to push the body around. The forces 
//    accumulators are set to zero after each integrator step.
//
//------------------------------------------------------------------------------
static void n_addforceatpos(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);

	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	float px = pCmd->In()->GetF();
	float py = pCmd->In()->GetF();
	float pz = pCmd->In()->GetF();

	nOdeCollideObject *co = pSelf->getCollideObject();
	if (co)
	{
		co->addForceAtPos( &vector3( x, y, z), &vector3(px, py, pz)) ;
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Add a force to this object represented by a vector3
//
//  INPUT:
//    f - the 'x' vector of the force
//    f - the 'y' vector of the force 
//    f - the 'z' vector of the force 
//
//    f - the 'x' vector of the position
//    f - the 'y' vector of the position
//    f - the 'z' vector of the position
//
//  From ODE docs: 
//    3.4. Force accumulators
//
//    Between each integrator step the user can call functions to apply 
//    forces to the rigid body. These forces are added to "force accumulators" 
//    in the rigid body object. When the next integrator step happens, the sum 
//    of all the applied forces will be used to push the body around. The forces 
//    accumulators are set to zero after each integrator step.
//
//------------------------------------------------------------------------------
static void n_addforceatrelpos(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);
	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	float px = pCmd->In()->GetF();
	float py = pCmd->In()->GetF();
	float pz = pCmd->In()->GetF();

	nOdeCollideObject *co = pSelf->getCollideObject();
	if (co)
	{
		co->addForceAtRelPos( &vector3( x, y, z), &vector3(px, py, pz)) ;
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Add a force to this object represented by a vector3
//
//  INPUT:
//    f - the 'x' vector of the relative force
//    f - the 'y' vector of the relative force 
//    f - the 'z' vector of the relative force 
//
//    f - the 'x' vector of the position
//    f - the 'y' vector of the position
//    f - the 'z' vector of the position
//
//  From ODE docs: 
//    3.4. Force accumulators
//
//    Between each integrator step the user can call functions to apply 
//    forces to the rigid body. These forces are added to "force accumulators" 
//    in the rigid body object. When the next integrator step happens, the sum 
//    of all the applied forces will be used to push the body around. The forces 
//    accumulators are set to zero after each integrator step.
//
//------------------------------------------------------------------------------
static void n_addrelforceatpos(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);

	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	float px = pCmd->In()->GetF();
	float py = pCmd->In()->GetF();
	float pz = pCmd->In()->GetF();

	nOdeCollideObject *co = pSelf->getCollideObject();
	if (co)
	{
		co->addRelForceAtPos( &vector3( x, y, z), &vector3(px, py, pz)) ;
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Set the finite rotation mode for this object
//
//  INPUT:
//
//    i - the mode of rotation
//
//  From ODE docs: 
//     This function controls the way a body's orientation is 
//     updated at each time step. The mode argument can be: 
//        0: An ``infitesimal'' orientation update is used. 
//           This is fast to compute, but it can occasionally cause 
//           inaccuracies for bodies that are rotating at high speed, 
//           especially when those bodies are joined to other bodies. 
//           This is the default for every new body that is created. 
//        1: A ``finite'' orientation update is used. This is more costly 
//           to compute, but will be more accurate for high speed rotations. 
//           Note however that high speed rotations can result in many types 
//           of error in a simulation, and this mode will only fix one of 
//           those sources of error. 
//
//------------------------------------------------------------------------------
static void n_setfiniterotationmode(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);

	int mode = pCmd->In()->GetI();

	nOdeCollideObject *co = pSelf->getCollideObject();
	if (co)
	{
		co->setFiniteRotationMode( mode );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//  Set the finite rotation mode for this object
//
//  INPUT:
//
//    i - the mode of rotation
//
//  From ODE docs: 
//       This sets the finite rotation axis for a body. 
//       This is axis only has meaning when the finite rotation mode is set 
//       (see setFiniteRotationMode()).
//
//       If this axis is zero (0,0,0), full finite rotations are performed on 
//       the body.
//
//       If this axis is nonzero, the body is rotated by performing a partial 
//       finite rotation along the axis direction followed by an infitesimal 
//       rotation along an orthogonal direction.
//
//       This can be useful to alleviate certain sources of error caused by 
//       quickly spinning bodies. For example, if a car wheel is rotating at 
//       high speed you can call this function with the wheel's hinge axis as 
//       the argument to try and improve its behavior. 
//
//------------------------------------------------------------------------------
static void n_setfiniterotationaxis(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);

	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	nOdeCollideObject *co = pSelf->getCollideObject();
	if (co)
	{
		co->setFiniteRotationAxis( &vector3(x, y, z) );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//------------------------------------------------------------------------------
static void n_getforce(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);

	nOdeCollideObject *co = pSelf->getCollideObject();
	if (co)
	{
		vector3 force(co->getForce());
		pCmd->Out()->SetF( force.x );
		pCmd->Out()->SetF( force.y );
		pCmd->Out()->SetF( force.z );
	} else {
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//------------------------------------------------------------------------------
static void n_gettorque(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);

	nOdeCollideObject *co = pSelf->getCollideObject();
	if (co)
	{
		vector3 torque(co->getTorque());
		pCmd->Out()->SetF( torque.x );
		pCmd->Out()->SetF( torque.y );
		pCmd->Out()->SetF( torque.z );
	} else {
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//------------------------------------------------------------------------------
static void n_getrelpointpos(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);
	nOdeCollideObject *co = pSelf->getCollideObject();

	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	if (co)
	{
		vector3 pos(co->getRelPointPos( &vector3( x, y, z) ));
		pCmd->Out()->SetF( pos.x );
		pCmd->Out()->SetF( pos.y );
		pCmd->Out()->SetF( pos.z );
	} else {
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//------------------------------------------------------------------------------
static void n_getrelpointvel(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);
	nOdeCollideObject *co = pSelf->getCollideObject();

	float x = pCmd->In()->GetF();
	float y = pCmd->In()->GetF();
	float z = pCmd->In()->GetF();

	if (co)
	{
		vector3 vel(co->getRelPointVel( &vector3( x, y, z) ));
		pCmd->Out()->SetF( vel.x );
		pCmd->Out()->SetF( vel.y );
		pCmd->Out()->SetF( vel.z );
	} else {
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//------------------------------------------------------------------------------
static void n_getfiniterotationmode(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);
	nOdeCollideObject *co = pSelf->getCollideObject();

	if (co)
	{
		int mode  = co->getFiniteRotationMode();
		pCmd->Out()->SetI( mode );
	} else {
		pCmd->Out()->SetI( -1 );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//------------------------------------------------------------------------------
static void n_getfiniterotationaxis(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);
	nOdeCollideObject *co = pSelf->getCollideObject();

	if (co)
	{
		vector3 axis( co->getFiniteRotationAxis() );
		pCmd->Out()->SetF( axis.x );
		pCmd->Out()->SetF( axis.y );
		pCmd->Out()->SetF( axis.z );
	} else {
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
		pCmd->Out()->SetF( 0.0f );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
//------------------------------------------------------------------------------
static void n_getnumjoints(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);

	nOdeCollideObject *co = pSelf->getCollideObject();

	if (co)
	{
		int num = co->getNumJoints();
		pCmd->Out()->SetI( num );
	} else {
		pCmd->Out()->SetI( 0 );
	}
}

//------------------------------------------------------------------------------
//  2002.02.06  whitegold       created
//
// TODO: Passing a struct?
//------------------------------------------------------------------------------
static void n_getjoint(void * pObj, nCmd* pCmd)
{
	nOdeNode *pSelf = static_cast<nOdeNode *>(pObj);

	nOdeCollideObject *co = pSelf->getCollideObject();
	int index = pCmd->In()->GetI();

	if (0)
	{
		//co->getJoint( index );
		//pCmd->Out()->SetI( num );
	} else {
		pCmd->Out()->SetI( -1 );
	}
}
