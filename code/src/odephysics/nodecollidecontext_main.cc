#define N_IMPLEMENTS nOdeCollideContext
//------------------------------------------------------------------------------
//  nodecollidecontext.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeCollideContext is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "kernel/ntimeserver.h"

#include "odephysics/nodecollidecontext.h"
nNebulaClass( nOdeCollideContext, "nroot" );

#include "odephysics/nodecollideshape.h"
//------------------------------------------------------------------------------
/**
@brief Constructor
*/
nOdeCollideContext::nOdeCollideContext()
: ref_OdeServer(kernelServer, this)
{
	this->ref_OdeServer = "/sys/servers/ode";
}

//------------------------------------------------------------------------------
/**
@brief Destructor
*/
nOdeCollideContext::~nOdeCollideContext()
{
}

//------------------------------------------------------------------------------
/**
@brief Check for collision between potentialy colliding geoms.
*/
void nOdeCollideContext::NearCallback( void* data, dGeomID o1, dGeomID o2 )
{
	if ( dGeomIsSpace( o1 ) || dGeomIsSpace( o2 ) ) 
	{
		// colliding a space with something
		dSpaceCollide2( o1, o2, data, &NearCallback );
		// collide all geoms internal to the space(s)
		if ( dGeomIsSpace( o1 ) ) 
			dSpaceCollide( (dSpaceID)o1, data, &NearCallback );
		if ( dGeomIsSpace( o2 ) ) 
			dSpaceCollide( (dSpaceID)o2, data, &NearCallback );
	}
	else
	{
		// colliding two non-space geoms
		nOdeCollideContext* context = (nOdeCollideContext*)data;
		nOdeCollideShape* firstShape = (nOdeCollideShape*)dGeomGetData( o1 );
		nOdeCollideShape* secondShape = (nOdeCollideShape*)dGeomGetData( o2 );
		n_assert( firstShape && secondShape );
		context->Collide( firstShape, secondShape );
	}
}

//------------------------------------------------------------------------------
/**
@brief Detect collision between shapes in the specified space.
@return Number of collision reports recorded.
*/
int nOdeCollideContext::Collide( dSpaceID spaceId, bool clearCache )
{
	n_assert( spaceId );

	if ( clearCache ) // wipe any existing records
		this->collideReportHandler.Clear();

	dSpaceCollide( spaceId, this, &NearCallback );

	return this->collideReportHandler.GetNumCollissions();
}

//------------------------------------------------------------------------------
/**
@brief Check for collision between two shapes.

Check wether the pair of shapes has already been previously handled,
if not get ODE to determine collision info then store it in collide
reports. If any of the shapes are composite shapes multiple collide
reports will be generated, one report for each pair (a,b) where
a is part of s1 and b is part of s2. No report will be generated
for (s1,s2) - only the collisions between the parts will be stored.
*/
void nOdeCollideContext::Collide( nOdeCollideShape* s1, nOdeCollideShape* s2 )
{
	n_assert( this->ref_OdeServer.isvalid() );

	nOdeCollideReportHandler& handler = this->collideReportHandler;

	// query the collision type defined for those two shapes
	nOdeCollType ct = this->ref_OdeServer->QueryCollType( s1->GetCollClass(), 
		s2->GetCollClass() );

	// ignore collision?
	if ( ODE_COLLTYPE_IGNORE == ct ) 
		return;

	// has this collision already been detected by the other object?
	if ( !handler.CollissionExists( s1->GetId(), s2->GetId() ) ) 
	{
		// no, we're first...
		// ask objects whether they collide...

		nOdeCollideReport cr;
		int numContacts = dCollide( s1->geomId, 
			s2->geomId,
			this->N_MAX_CONTACT_GEOMS, 
			&(this->contactGeoms[0]), 
			sizeof(dContactGeom) );

		nOdeCollideContact* cc;
		const int invalidContact = -1;
		int contactIndex = invalidContact;

		// store all contacts in collision reports

		if ( numContacts > 0 )
		{
			// start a report
			cr.firstContact = invalidContact;
			cr.cs1 = (nOdeCollideShape*)dGeomGetData( this->contactGeoms[0].g1 );
			cr.cs2 = (nOdeCollideShape*)dGeomGetData( this->contactGeoms[0].g2 );
			cr.tstamp = this->kernelServer->ts->GetFrameTime();
		}

		for ( int i = 0; i < numContacts; i++ )
		{
			if ( i > 0 )
			{
				// check if we need to create a new report
				if ( (this->contactGeoms[i-1].g1 != this->contactGeoms[i].g1) ||
					(this->contactGeoms[i-1].g2 != this->contactGeoms[i].g2) )
				{
					// store last report
					handler.AddCollission( cr, cr.cs1->GetId(), cr.cs2->GetId() );
					// start new report
					cr.firstContact = invalidContact;
					cr.cs1 = (nOdeCollideShape*)dGeomGetData( this->contactGeoms[i].g1 );
					cr.cs2 = (nOdeCollideShape*)dGeomGetData( this->contactGeoms[i].g2 );
				}
			}
			cr.cs1->numColls++;
			cr.cs2->numColls++;

			cc = handler.NewCollideContact( contactIndex );
			cc->contact.set( this->contactGeoms[i].pos[0], 
				this->contactGeoms[i].pos[1],
				this->contactGeoms[i].pos[2] );
			cc->normal.set( this->contactGeoms[i].normal[0],
				this->contactGeoms[i].normal[1],
				this->contactGeoms[i].normal[2] );
			cc->depth = this->contactGeoms[i].depth;

			if ( invalidContact == cr.firstContact )
				cr.firstContact = contactIndex;

			cr.lastContact = contactIndex;
		}

		if ( numContacts > 0 ) // store the very last report
			handler.AddCollission( cr, cr.cs1->GetId(), cr.cs2->GetId() );
	}
}

//------------------------------------------------------------------------------
/**
@brief Get all collision reports a shape is involved in.
@param cs A collide shape (that is part of this context).
@param cr [out] Pointer to an internal collision array and the number of 
collision reports.
@return Number of reports for the shape cs.
*/
int nOdeCollideContext::GetCollissionReports( nOdeCollideShape* cs, 
											 nOdeCollideReport**& cr )
{
	n_assert( cs && "Not a valid collide shape!" );

	if ( cs->GetNumCollissions() > 0 )
		return this->collideReportHandler.GetCollissions( cs, cr );
	else 
	{
		cr = NULL;
		return 0;
	}
}

//------------------------------------------------------------------------------
/**
@brief Check for collision between a ray/line and another geom.
*/
void nOdeCollideContext::RayCallback( void* data, dGeomID g1, dGeomID g2 )
{
	if ( dGeomGetClass( g1 ) == dRayClass )
		((nOdeCollideContext*)data)->RayCollide( g1, g2 );
	else
		((nOdeCollideContext*)data)->RayCollide( g2, g1 );
}

//------------------------------------------------------------------------------
/**
@brief Check for collision between a ray and a shape.
*/
void nOdeCollideContext::RayCollide( dGeomID ray, dGeomID g2 )
{
	n_assert( this->ref_OdeServer.isvalid() );

	nOdeCollideReportHandler& handler = this->checkReportHandler;
	nOdeCollideShape* shape = (nOdeCollideShape*)dGeomGetData( g2 );
	n_assert( shape );
	dContactGeom contactGeom;

	if ( ODE_COLLCLASS_ALWAYS_EXACT != this->checkClass )
	{
		// query the collision type defined for those two objects
		nOdeCollType ct = this->ref_OdeServer->QueryCollType( this->checkClass,
			shape->GetCollClass() );
		// ignore collision?
		if ( ODE_COLLTYPE_IGNORE == ct ) 
			return;
	}

	// generate at most 1 contact
	int numContacts = dCollide( ray, g2, 1, &contactGeom, sizeof(dContactGeom) );

	// store contact in a collision report
	if ( numContacts > 0 )
	{
		nOdeCollideReport cr; 
		nOdeCollideContact* cc;
		int contactIndex;
		const int ownId = 0xffff;

		cc = handler.NewCollideContact( contactIndex );
		cc->contact.set( contactGeom.pos[0], 
			contactGeom.pos[1], 
			contactGeom.pos[2] );
		cc->normal.set( contactGeom.normal[0], 
			contactGeom.normal[1], 
			contactGeom.normal[2] );
		cc->depth = contactGeom.depth;
		cr.firstContact = contactIndex;
		cr.lastContact = contactIndex;
		cr.cs1 = shape;
		cr.cs2 = shape;
		cr.tstamp = this->kernelServer->ts->GetFrameTime();
		handler.AddCollission( cr, ownId, shape->GetId() );
	}
}

//------------------------------------------------------------------------------
/**
@brief Check for intersection of a line against the shapes in the context.
@param line      [in] The line to test (in global space).
@param collType  [in] 
- ODE_COLLTYPE_IGNORE:  illegal (makes no sense)
- ODE_COLLTYPE_QUICK:   return all contacts (unsorted)
- ODE_COLLTYPE_CONTACT: return closest contact only
- ODE_COLLTYPE_EXACT:   same as ODE_COLLTYPE_QUICK
@param collClass [in] The collision class to associate with the line. Use
ODE_COLLCLASS_ALWAYS_EXACT to disable filtering (so
the line will be tested against shapes regardless of 
their collision classes).
@param crPtr [out] Pointer to contact information.
@return Number of detected contacts (<b>at most 1 per shape</b>).
*/
int
nOdeCollideContext::LineCheck( dSpaceID spaceId, const line3& line, 
							  nOdeCollType collType, 
							  nOdeCollClass collClass, 
							  nOdeCollideReport**& crPtr )
{
	n_assert( 0 != nOdeCollideShape::rayId );
	n_assert( collType != ODE_COLLTYPE_IGNORE );

	this->checkClass = collClass;
	nOdeCollideShape::rayCollType = collType;

	// setup ray geom
	dGeomRaySet( nOdeCollideShape::rayId, line.b.x, line.b.y, line.b.z,
		line.m.x, line.m.y, line.m.z );
	dGeomRaySetLength( nOdeCollideShape::rayId, line.len() );

	switch ( collType )
	{
	case ODE_COLLTYPE_EXACT:
	case ODE_COLLTYPE_QUICK:
		dGeomRaySetParams( nOdeCollideShape::rayId, 
			1/*firstContact*/, 0/*backFaceCull*/ );
		dGeomRaySetClosestHit( nOdeCollideShape::rayId, 0 );
		break;
	case ODE_COLLTYPE_CONTACT:
		dGeomRaySetParams( nOdeCollideShape::rayId,
			0/*firstContact*/, 0/*backFaceCull*/ );
		dGeomRaySetClosestHit( nOdeCollideShape::rayId, 1 );
		break;
	}

	// FIXME: I'm not sure if there is any point in adding it to the space or not.
	//dSpaceAdd( this->spaceId, this->rayId );

	// initialize collision report handler
	this->checkReportHandler.Clear();

	dSpaceCollide2( nOdeCollideShape::rayId, (dGeomID)spaceId, this, &RayCallback );

	//dSpaceRemove( this->spaceId, this->rayId );

	if ( ODE_COLLTYPE_CONTACT == collType )
	{
		// get closest contact only
		return this->checkReportHandler.GetClosestCollission( line.start(), crPtr );
	}
	else
	{
		// get all contacts (unsorted)
		return this->checkReportHandler.GetAllCollissions( crPtr );
	}

	return 0;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
