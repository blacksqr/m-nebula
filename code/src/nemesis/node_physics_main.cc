#define N_IMPLEMENTS nOdePhysics
//==============================================================================
//  misc/nOdePhysics_main.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_physics.h"

#include "collide/ncollideserver.h"
#include "collide/ncollideobject.h"
#include "collide/ncollideshape.h"
#include "collide/ncollidereporthandler.h"

#include "nemesis/node_collidecontext.h"
#include "nemesis/node_collideshapes.h"

#include "kernel/ntimeserver.h"

// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/ode.h"


static void odeError(int errnum, const char *msg, va_list ap)
{
	n_printf("ODE_ERR: [%d] %s\n", errnum, msg );
}

static void odeDebug(int errnum, const char *msg, va_list ap)
{
	n_printf("ODE_DBG: [%d] %s\n", errnum, msg );
}

static void odeMessage(int errnum, const char *msg, va_list ap)
{
	n_printf("ODE_MSG: [%d] %s\n", errnum, msg );
}

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
nOdePhysics::nOdePhysics() :
	myRefCs(pKernelServer, this),
	myMaxStepTime( 0.1f )
{
	unique = 0;

    myCollisionJoints = dJointGroupCreate(0);

	myRefCs = "/sys/servers/physics";

	// We need to be called directly after collide is called, 
	// since collide is priority 65, if we're 66, we're married 
	// to its pulse.. uhmm.. or sumthin
//	this->trig_prio	= 66;

	myWorld = dWorldCreate();
	vector3 gravity( 0.0f, -9.8f, 0.0f );
	setGravity( gravity );
	dWorldSetCFM( myWorld, .0001f );

	dSetErrorHandler( &odeError );
	dSetDebugHandler( &odeDebug );
	dSetMessageHandler( &odeMessage );


}

//==============================================================================
//  2001.9.27  WhiteGold       created
//------------------------------------------------------------------------------
nOdePhysics::~nOdePhysics()
{
	dJointGroupDestroy( myCollisionJoints );
	dWorldDestroy( myWorld );
}

//==============================================================================
//  2001.9.27  WhiteGold       created
//------------------------------------------------------------------------------
bool nOdePhysics::Trigger()
{
	if (myLastTime == 0.0)
		myLastTime = pKernelServer->ts->GetFrameTime() - .05;
	double tNow = pKernelServer->ts->GetFrameTime();
	double dt = tNow - myLastTime; 

	
	// TODO:: Expose these as performance tweak variables.

	// Take it in no less than XXX chunks.
	int catchup = 0;
	while ( dt > myMaxStepTime )
	{
		dWorldStep(myWorld, myMaxStepTime);
		dJointGroupEmpty (myCollisionJoints);
		GetDefaultContext()->Collide();
		
		dt -= myMaxStepTime;
		catchup ++;
	}

	if (catchup)
		n_printf("DEBUG: Physics sim had to catch up %d frames\n", catchup );

	dWorldStep(myWorld, dt);
	dJointGroupEmpty (myCollisionJoints);
	GetDefaultContext()->Collide();

	myLastTime = tNow;
	return( true );
}

nCollideContext *nOdePhysics::NewContext(void)
{
    nOdeCollideContext *cc = new nOdeCollideContext(this);
    this->context_list.AddHead(cc);
    return cc;
}

nCollideShape   *nOdePhysics::NewShape(const char *id)
{
	return( NewShape( id, ODE_SPHERE ) );
}

nCollideShape   *nOdePhysics::NewShape(const char *id, nOdeTypes type)
{
    n_assert(id);

    char buf[N_MAXPATH];
    getResourceID(id,buf,sizeof(buf));

    // shape already existing?
    nCollideShape *cs = (nCollideShape *) shape_list.Find(buf);
    if (!cs) {
		switch( type )
		{
			case ODE_SPHERE:
			    cs = new nOdeSphereCollideShape(buf);
				break;
			case ODE_BOX:
				cs = new nOdeBoxCollideShape(buf);
				break;
			case ODE_CYLINDER:
				cs = new nOdeCylinderCollideShape(buf);
				break;
			case ODE_PLANE:
				cs = new nOdePlaneCollideShape(buf);
                break;
			case ODE_TRILIST:
				cs = new nOdeTriListCollideShape(buf);
		}
        this->shape_list.AddTail(cs);
    }
    cs->AddRef();

    return cs;
}

void nOdePhysics::setGravity( float x, float y, float z )
{
	dWorldSetGravity( myWorld, x, y, z );
}

void nOdePhysics::setGravity( vector3 &gravity )
{
	dWorldSetGravity( myWorld, gravity.x, gravity.y, gravity.z );
}

vector3 nOdePhysics::getGravity()
{
	dVector3 grav;
	dWorldGetGravity( myWorld, grav );
	return( vector3( grav[0], grav[1], grav[2] ));
}

// Error Reduction Parameter
void nOdePhysics::setWorldERP( float erp )
{
	dWorldSetERP( myWorld, erp );
}

float nOdePhysics::getWorldERP()
{
	return( dWorldGetERP( myWorld ) );
}

// Constraint Force Mixing
void nOdePhysics::setWorldCFM( float cfm )
{
	dWorldSetCFM( myWorld, cfm );
}

float nOdePhysics::getWorldCFM()
{
	return( dWorldGetCFM( myWorld ) );
}
