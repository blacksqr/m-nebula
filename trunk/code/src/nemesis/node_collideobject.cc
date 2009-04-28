#define N_IMPLEMENTS nOdeCollideObject
//---------------------------------------------------------
//  nOdeCollideObject.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//---------------------------------------------------------
#include "nemesis/node_collideobject.h"
// DEFINES FOR ODE
#define dSINGLE 1

//  includes
#include "ode/ode.h"

#include "nemesis/node_physics.h"
#include "kernel/ntimeserver.h"

nOdeCollideObject::nOdeCollideObject()
    : nCollideObject(),
	  myOdeGeometryId( NULL ),
	  isStatic( false )
{

	mySurfaceParameters.mode = dContactBounce ;
	
	// SUPER friction.  See ODE docs
	mySurfaceParameters.mu = dInfinity;

	// A little bouncy
	mySurfaceParameters.bounce = dReal(0.4);
	mySurfaceParameters.bounce_vel = dReal(0.1);

	/* only defined if the corresponding flag is set in mode */
	/* See ODE docs for all modes */
	mySurfaceParameters.mu2 = dReal(0.0);
	mySurfaceParameters.soft_erp = dReal(0.0);
	mySurfaceParameters.soft_cfm = dReal(0.0);
	mySurfaceParameters.motion1 = dReal(0.0);
	mySurfaceParameters.motion2 = dReal(0.0);
	mySurfaceParameters.slip1 = dReal(0.0);
	mySurfaceParameters.slip2 = dReal(0.0);
};

//---------------------------------------------------------
//  ~nCollideObject()
//  16-Jun-00   floh    created
//---------------------------------------------------------
nOdeCollideObject::~nOdeCollideObject() 
{ 
}

void nOdeCollideObject::SetContactParam( int param, dReal value )
{
 	switch ( param )
	{
	case dCParamMu:
		mySurfaceParameters.mu = value;
		break;
	case dCParamMu2:
		mySurfaceParameters.mu2 = value;
		( value > 0.0 ) ? mySurfaceParameters.mode |= dContactMu2 : mySurfaceParameters.mode &= ~dContactMu2;
		break;
	case dCParamBounce:
		mySurfaceParameters.bounce = value;
		( value > 0.0 ) ? mySurfaceParameters.mode |= dContactBounce : mySurfaceParameters.mode &= ~dContactBounce;
		break;
	case dCParamBounceVel:
		mySurfaceParameters.bounce_vel = value;
		break;
	case dCParamSoftErp:
		mySurfaceParameters.soft_erp = value;
		( value > 0.0 ) ? mySurfaceParameters.mode |= dContactSoftERP : mySurfaceParameters.mode &= ~dContactSoftERP;
		break;
	case dCParamSoftCfm:
		mySurfaceParameters.soft_cfm = value;
		( value > 0.0 ) ? mySurfaceParameters.mode |= dContactSoftCFM : mySurfaceParameters.mode &= ~dContactSoftCFM;
		break;
	case dCParamMotion1:
		mySurfaceParameters.motion1 = value;
		( value != 0.0 ) ? mySurfaceParameters.mode |= dContactMotion1 : mySurfaceParameters.mode &= ~dContactMotion1;
		break;
	case dCParamMotion2:
		mySurfaceParameters.motion2 = value;
		( value != 0.0 ) ? mySurfaceParameters.mode |= dContactMotion2 : mySurfaceParameters.mode &= ~dContactMotion2;
		break;
	case dCParamSlip1:
		mySurfaceParameters.slip1 = value;
		( value > 0.0 ) ? mySurfaceParameters.mode |= dContactSlip1 : mySurfaceParameters.mode &= ~dContactSlip1;
		break;
	case dCParamSlip2:
		mySurfaceParameters.slip2 = value;
		( value > 0.0 ) ? mySurfaceParameters.mode |= dContactSlip2 : mySurfaceParameters.mode &= ~dContactSlip2;
		break;
	}
}


/**
    detailed description.
*/
dReal nOdeCollideObject::GetContactParam( int param )
{
	float value = 0.0;

 	switch ( param )
	{
	case dCParamMu:
		value = mySurfaceParameters.mu;
		break;
	case dCParamMu2:
		value = mySurfaceParameters.mu2;
		break;
	case dCParamBounce:
		value = mySurfaceParameters.bounce;
		break;
	case dCParamBounceVel:
		value = mySurfaceParameters.bounce_vel;
		break;
	case dCParamSoftErp:
		value = mySurfaceParameters.soft_erp;
		break;
	case dCParamSoftCfm:
		value = mySurfaceParameters.soft_cfm;
		break;
	case dCParamMotion1:
		value = mySurfaceParameters.motion1;
		break;
	case dCParamMotion2:
		value = mySurfaceParameters.motion2;
		break;
	case dCParamSlip1:
		value = mySurfaceParameters.slip1;
		break;
	case dCParamSlip2:
		value = mySurfaceParameters.slip2;
		break;
	}

	return value;
}

void nOdeCollideObject::setStatic( bool doStatic )
{
	if (isStatic == doStatic )
		return;

	if (!doStatic)
	{
		// Create a body, which will give the object life
		dBodyID body = dBodyCreate( ((nOdePhysics *)GetContext()->GetCollideServer())->getWorldID() );
		dGeomSetBody( myOdeGeometryId, body );
	} else {
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		if (body)
		{
			const dReal *pos = dBodyGetPosition( body );
			const dReal *rot = dBodyGetRotation( body );

			vector3 pos1(pos[0], pos[1], pos[2] );
			float rot1[] = { rot[0], rot[1], rot[2], rot[3], 
						   rot[4], rot[5], rot[6], rot[7], 
					       rot[8], rot[9], rot[10], rot[11], 
						   rot[12], rot[13], rot[14], rot[15]};
			
			dBodyDestroy( body );
			// This clears out the position!?  DUH!
			dGeomSetBody( myOdeGeometryId, NULL );
			dGeomSetPosition( myOdeGeometryId, pos1.x, pos1.y, pos1.z );
			dGeomSetRotation( myOdeGeometryId, rot1 );
		}
	}
	this->isStatic = doStatic;
};

void nOdeCollideObject::SetShape(nCollideShape *s) {
    n_assert(s);
    this->shape = s;
    SetRadius(s->GetRadius());
	
	// Get the ODE object information HERE.  Let the shape describe to ODE
	// what it is, but track it through this object so we can have many objects
	// still sharing the same shape information
	myOdeGeometryId = ((nOdeSphereCollideShape *)shape)->buildOdeObject();
	dGeomSetData( myOdeGeometryId, this );
	
	if (!isStatic)
	{
		dBodyID body = dBodyCreate( ((nOdePhysics *)collide_server)->getWorldID() );
		dGeomSetBody( myOdeGeometryId, body );

		dMass mass;
		((nOdeSphereCollideShape *)shape)->buildOdeMass( &mass );
	    dBodySetMass (body, &mass);
	}
};

// Forces (Accumulated for each frame, emptied at the end of every frame.)
void nOdeCollideObject::addForce( vector3 *force )
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyAddForce( body, force->x, force->y, force->z );
	}
};

// Forces (Accumulated for each frame, emptied at the end of every frame.)
void nOdeCollideObject::addTorque( vector3 *torque )
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyAddTorque( body, torque->x, torque->y, torque->z );
	}
};

vector3 nOdeCollideObject::getLinearVelocity()
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dReal *vel = (dReal *)dBodyGetLinearVel( body );
		return( vector3( vel[0], vel[1], vel[2]) );
	}
	return( vector3( 0, 0, 0 ) );
};

vector3 nOdeCollideObject::getAngularVelocity()
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );

		dReal *vel = (dReal *)dBodyGetAngularVel( body );

		return( vector3( vel[0], vel[1], vel[2]) );
	}
	return( vector3( 0, 0, 0 ) );
};

// Now will cue a repositioning of the object in ODE space
void nOdeCollideObject::Reset(matrix44& m) {
	n_assert(is_attached);
	new_matrix = m;

	// set ODE object's position here
	vector3 pos( m.pos_component() );

	dGeomSetPosition( myOdeGeometryId, pos.x, pos.y, pos.z );
	
	matrix33 rot( m.x_component(), m.y_component(), m.z_component() );
	dMatrix3 dRot;
	dRot[0] = rot.M11;
	dRot[1] = rot.M21;
	dRot[2] = rot.M31;
	dRot[3] = 0.0f;

	dRot[4] = rot.M12;
	dRot[5] = rot.M22;
	dRot[6] = rot.M32;
	dRot[7] = 0.0f;

	dRot[8] = rot.M13;
	dRot[9] = rot.M23;
	dRot[10] = rot.M33;
	dRot[11] = 0.0f;

	dGeomSetRotation( myOdeGeometryId, dRot );
};

void nOdeCollideObject::Transform(double t, matrix44& m) {
	n_assert(is_attached);

//	if ( t == 0.0 )
//	{
		// set ODE object's position here
		vector3 pos( m.pos_component() );

		dGeomSetPosition( myOdeGeometryId, pos.x, pos.y, pos.z );
		
		matrix33 rot( m.x_component(), m.y_component(), m.z_component() );
		dMatrix3 dRot;

		dRot[0] = rot.M11;
		dRot[1] = rot.M21;
		dRot[2] = rot.M31;
		dRot[3] = 0.0f;

		dRot[4] = rot.M12;
		dRot[5] = rot.M22;
		dRot[6] = rot.M32;
		dRot[7] = 0.0f;

		dRot[8] = rot.M13;
		dRot[9] = rot.M23;
		dRot[10] = rot.M33;
		dRot[11] = 0.0f;
/*		dRot[0] = rot.M11;
		dRot[1] = rot.M12;
		dRot[2] = rot.M13;
		dRot[3] = 0.0f;

		dRot[4] = rot.M21;
		dRot[5] = rot.M22;
		dRot[6] = rot.M23;
		dRot[7] = 0.0f;

		dRot[8] = rot.M31;
		dRot[9] = rot.M32;
		dRot[10] = rot.M33;
		dRot[11] = 0.0f;
*/

		dGeomSetRotation( myOdeGeometryId, dRot );
//	}
	//	n_printf("nOdeCollideObject::Transform - Are you sure you wanted to do this?");

	nCollideObject::Transform( t, m );
};

const matrix44& nOdeCollideObject::GetTransform(void) {
	
	dBodyID body = dGeomGetBody( myOdeGeometryId );
	
	const dReal *pos = dGeomGetPosition( myOdeGeometryId );
	const dReal *rot = (body ? 
								dBodyGetRotation( body ) :  // 4x4 matrix, ignoring the 4th column
  								dGeomGetRotation( myOdeGeometryId ));
	 
	if (body)
	{
		const dReal *dQuat = dBodyGetQuaternion( body) ;
	}

	this->old_matrix = this->new_matrix;
	this->old_tstamp = this->new_tstamp;

	new_matrix.set( vector4( rot[0], rot[1],  rot[2], rot[3] ),
  				    vector4( rot[4], rot[5], rot[6], rot[7] ),
				    vector4( rot[8], rot[9], rot[10], rot[11]),
				    vector4( pos[0], pos[1], pos[2], 0.0f ) );

	// overcome -- every time our matrix is updated from ODE, let 
	// the parent know.
	//
	// BUG: If I set the transform here, the shape doesn't render.
	//      If I set it in node_node::Compute it does... And I don't
	//      know ... why.... the matrices look identicle
	// 
//	nCollideObject::Transform( nOdePhysics::pKernelServer->ts->GetTime(), new2_matrix);
    return new_matrix;
}

bool nOdeCollideObject::Contact(nCollideObject *other,     // the other object
	         nCollType ct,
		     nCollideReport& cr)
{
	dContact contact;
	memcpy(&contact.surface, &mySurfaceParameters, sizeof(mySurfaceParameters) );
	
	nOdeCollideObject *o2 = (nOdeCollideObject *)other;

	if (int numc = dCollide ( myOdeGeometryId, o2->myOdeGeometryId,1,&contact.geom,sizeof(dContact)))
	{
	  // Now create the nCollideReport

	  cr.co1 = this;
	  cr.co2 = other;
	  cr.contact = vector3( contact.geom.pos[0], contact.geom.pos[1], contact.geom.pos[1] );
	  cr.co1_normal = vector3( contact.geom.normal[0], contact.geom.normal[1], contact.geom.normal[2]);
	  cr.co2_normal = -cr.co1_normal;
	  
	  // Hmm.. What to do, if anything, with these values.
	  cr.tstamp = nOdePhysics::pKernelServer->ts->GetFrameTime();

	  return( true );
	}
	return( false );
};

vector3 nOdeCollideObject::getTorque()
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		const dReal *f = dBodyGetTorque( body );
		return( vector3( f[0], f[1], f[2]) );
	}
	return( vector3( 0.0f, 0.0f, 0.0f ) );
}

vector3 nOdeCollideObject::getForce()
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		const dReal *f = dBodyGetTorque( body );
		return( vector3( f[0], f[1], f[2]) );
	}
	return( vector3(0.0f, 0.0f, 0.0f) );
}

void nOdeCollideObject::addRelForce(vector3 *force)
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyAddRelForce( body, force->x, force->y, force->z );
	}
}

void nOdeCollideObject::addRelTorque(vector3 *torque)
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyAddRelTorque( body,torque->x, torque->y, torque->z );
	}
}

void nOdeCollideObject::addForceAtPos(vector3 *force, vector3 *pos)
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyAddForceAtPos( body, force->x, force->y, force->z,
								  pos->x,   pos->y,   pos->z );
	}
}

void nOdeCollideObject::addForceAtRelPos(vector3 *force, vector3 *pos)
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyAddForceAtRelPos( body, force->x, force->y, force->z,
								     pos->x,   pos->y,   pos->z );
	}
}

void nOdeCollideObject::addRelForceAtPos(vector3 *force, vector3 *pos)
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyAddRelForceAtPos( body, force->x, force->y, force->z,
								     pos->x,   pos->y,   pos->z );
	}
}


void nOdeCollideObject::addRelForceAtRelPos(vector3 *force, vector3 *pos)
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyAddRelForceAtRelPos( body, force->x, force->y, force->z,
				                        pos->x,   pos->y,   pos->z );
	}
}


vector3 nOdeCollideObject::getRelPointPos(vector3 *pos)
{
	dVector3 force = {0.0, 0.0, 0.0};
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyGetRelPointPos( body, pos->x, pos->y, pos->z, force );
	}
	return( vector3( force[0], force[1], force[2] ) );
}

vector3 nOdeCollideObject::getRelPointVel(vector3 *pos)
{
	dVector3 vel = {0.0, 0.0, 0.0};
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyGetRelPointVel( body, pos->x, pos->y, pos->z, vel);
	}
	return( vector3( vel[0], vel[1], vel[2] ) );
}


void nOdeCollideObject::setFiniteRotationMode(int mode)
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodySetFiniteRotationMode( body, mode );
	}
}

void nOdeCollideObject::setFiniteRotationAxis(vector3 *axis)
{
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodySetFiniteRotationAxis( body, axis->x, axis->y, axis->z);
	}
}

int nOdeCollideObject::getFiniteRotationMode()
{
	int ret = -1;
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		ret = dBodyGetFiniteRotationMode( body );
	}
	return( ret );
}

vector3 nOdeCollideObject::getFiniteRotationAxis()
{
	dVector3 axis = {0.0, 0.0, 0.0};
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		dBodyGetFiniteRotationAxis( body, axis );
	}
	return( vector3( axis[0], axis[1], axis[2] ) );
}

int nOdeCollideObject::getNumJoints()
{
	int ret = 0;
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		ret = dBodyGetNumJoints( body );
	}
	return( ret );
}

dJointID nOdeCollideObject::getJoint(int index)
{
	dJointID ret = 0;
	if (!isStatic)
	{
		dBodyID body = dGeomGetBody( myOdeGeometryId );
		n_assert( body );	

		ret = dBodyGetJoint( body, index );
	}
	return( ret );
}

//---------------------------------------------------------
//  EOF
//---------------------------------------------------------
