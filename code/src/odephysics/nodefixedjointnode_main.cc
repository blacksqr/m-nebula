#define N_IMPLEMENTS nOdeFixedJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeFixedJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodefixedjointnode.h"

nNebulaScriptClass(nOdeFixedJointNode, "nodejointnode")

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodejoint.h"

//------------------------------------------------------------------------------
/**
*/
nOdeFixedJointNode::nOdeFixedJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeFixedJointNode::~nOdeFixedJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Create the underlying nOdeFixedJoint instance.
  This obviously must be called before trying to do anything else with
  the joint.
*/
void nOdeFixedJointNode::InitJoint( const char* physContext )
{
  n_assert( !this->joint );
  this->ref_PhysContext = physContext;
  n_assert( this->ref_PhysContext.isvalid() );
  this->joint = this->ref_PhysContext->NewFixedJoint();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
