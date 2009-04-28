#define N_IMPLEMENTS nOdeAMotorJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeAMotorJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeamotorjointnode.h"

nNebulaScriptClass(nOdeAMotorJointNode, "nodejointnode")

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodejoint.h"

//------------------------------------------------------------------------------
/**
*/
nOdeAMotorJointNode::nOdeAMotorJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeAMotorJointNode::~nOdeAMotorJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Create the underlying nOdeAMotorJoint instance.
  This obviously must be called before trying to do anything else with
  the joint.
*/
void nOdeAMotorJointNode::InitJoint( const char* physContext )
{
  n_assert( !this->joint );
  this->ref_PhysContext = physContext;
  n_assert( this->ref_PhysContext.isvalid() );
  this->joint = this->ref_PhysContext->NewAMotorJoint();
}

//------------------------------------------------------------------------------
/**
*/
void nOdeAMotorJointNode::SetMode( const char* mode )
{
  nOdeAMotorJoint* j = (nOdeAMotorJoint*)this->GetJoint();
  if ( strcmp( mode, "user" ) == 0 )
    j->SetMode( dAMotorUser );
  else if ( strcmp( mode, "euler" ) == 0 )
    j->SetMode( dAMotorEuler );
  else
    n_warn( "nOdeAMotorJointNode::SetMode(): Uknown mode!" );
}

//------------------------------------------------------------------------------
/**
*/
const char* nOdeAMotorJointNode::GetMode()
{
  nOdeAMotorJoint* j = (nOdeAMotorJoint*)this->GetJoint();
  if ( j->GetMode() == dAMotorUser )
    return "user";
  else if ( j->GetMode() == dAMotorEuler )
    return "euler";
  else
    n_warn( "nOdeAMotorJointNode::GetMode(): Unknown mode!" );
  return "uknown";
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
