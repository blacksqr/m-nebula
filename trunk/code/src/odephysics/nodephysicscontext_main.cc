#define N_IMPLEMENTS nOdePhysicsContext
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//
//  nOdePhysicsContext is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "odephysics/nodephysicscontext.h"
nNebulaScriptClass(nOdePhysicsContext, "nroot");

#ifndef N_ODEBODY_H
#include "odephysics/nodebody.h"
#endif

#ifndef N_ODECOLLIDEREPORT_H
#include "odephysics/nodecollidereport.h"
#endif

#ifndef N_ODEJOINT_H
#include "odephysics/nodejoint.h"
#endif

//------------------------------------------------------------------------------
/**
*/
nOdePhysicsContext::nOdePhysicsContext()
{
  this->worldId = dWorldCreate();
  this->contactJointGroup = dJointGroupCreate( 0 );
}

//------------------------------------------------------------------------------
/**
*/
nOdePhysicsContext::~nOdePhysicsContext()
{
  dJointGroupDestroy( this->contactJointGroup );
  dWorldDestroy( this->worldId );
}

//------------------------------------------------------------------------------
/**
*/
void nOdePhysicsContext::SetAutoEnableDepth( int depth )
{
  dWorldSetAutoEnableDepthSF1( this->worldId, depth );
}

//------------------------------------------------------------------------------
/**
*/
int nOdePhysicsContext::GetAutoEnableDepth()
{
  return dWorldGetAutoEnableDepthSF1( this->worldId );
}

//------------------------------------------------------------------------------
nOdeBody* nOdePhysicsContext::NewOdeBody()
{
  return new nOdeBody( this );
}

//------------------------------------------------------------------------------
void nOdePhysicsContext::ReleaseOdeBody( nOdeBody* body )
{
  delete body;
}

//------------------------------------------------------------------------------
/**
  @brief Generates a new contact joint.
  @param b1 First body involved in contact, use NULL to indicate static environment.
  @param b2 Second body involved in contact, use NULL for static environment.
  @param collContact Collision contact obtained from a collision report.
  @param surface Surface parameters for the contact.
  @param friction Optional friction direction.
*/
void nOdePhysicsContext::CreateContact( nOdeBody* b1, nOdeBody* b2,
                                        const nOdeCollideContact& collContact, 
                                        const nOdeSurface& surface,
                                        const vector3& friction )
{
  dContact contact;
  contact.fdir1[0] = friction.x;
  contact.fdir1[1] = friction.y;
  contact.fdir1[2] = friction.z;
  contact.surface = surface.surface;
  contact.geom.depth = collContact.depth;
  contact.geom.normal[0] = collContact.normal.x;
  contact.geom.normal[1] = collContact.normal.y;
  contact.geom.normal[2] = collContact.normal.z;
  contact.geom.pos[0] = collContact.contact.x;
  contact.geom.pos[1] = collContact.contact.y;
  contact.geom.pos[2] = collContact.contact.z;
  // in theory the physics shouldn't care about the geoms
  // so we don't really need to supply them
  //contact.geom.g1 = 0;
  //contact.geom.g2 = 0;
  dBodyID body1 = 0;
  dBodyID body2 = 0;
  if ( b1 )
    body1 = b1->bodyId;
  if ( b2 )
    body2 = b2->bodyId;
    
  dJointID joint = dJointCreateContact( this->worldId, this->contactJointGroup,
                                        &contact );
  dJointAttach( joint, body1, body2 );
}

//------------------------------------------------------------------------------
nOdeBallJoint* nOdePhysicsContext::NewBallJoint( /*nOdeJointGroup* group*/ )
{
  return new nOdeBallJoint( this->worldId, 0 );
}

//------------------------------------------------------------------------------
nOdeHingeJoint* nOdePhysicsContext::NewHingeJoint( /*nOdeJointGroup* group*/ )
{
  return new nOdeHingeJoint( this->worldId, 0 );
}
    
//------------------------------------------------------------------------------
nOdeSliderJoint* nOdePhysicsContext::NewSliderJoint( /*nOdeJointGroup* group*/ )
{
  return new nOdeSliderJoint( this->worldId, 0 );
}

//------------------------------------------------------------------------------
nOdeUniversalJoint* nOdePhysicsContext::NewUniversalJoint( /*nOdeJointGroup* group*/ )
{
  return new nOdeUniversalJoint( this->worldId, 0 );
}

//------------------------------------------------------------------------------
nOdeHinge2Joint* nOdePhysicsContext::NewHinge2Joint( /*nOdeJointGroup* group*/ )
{
  return new nOdeHinge2Joint( this->worldId, 0 );
}

//------------------------------------------------------------------------------
nOdeFixedJoint* nOdePhysicsContext::NewFixedJoint( /*nOdeJointGroup* group*/ )
{
  return new nOdeFixedJoint( this->worldId, 0 );
}

//------------------------------------------------------------------------------
nOdeAMotorJoint* nOdePhysicsContext::NewAMotorJoint( /*nOdeJointGroup* group*/ )
{
  return new nOdeAMotorJoint( this->worldId, 0 );
}

//------------------------------------------------------------------------------
void nOdePhysicsContext::ReleaseOdeJoint( nOdeJoint* joint )
{
  delete joint;
}
