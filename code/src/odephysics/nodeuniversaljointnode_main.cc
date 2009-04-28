#define N_IMPLEMENTS nOdeUniversalJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeUniversalJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeuniversaljointnode.h"

nNebulaScriptClass(nOdeUniversalJointNode, "nodejointnode")

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodejoint.h"

//------------------------------------------------------------------------------
/**
*/
nOdeUniversalJointNode::nOdeUniversalJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeUniversalJointNode::~nOdeUniversalJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Create the underlying nOdeUniversalJoint instance.
  This must be called before trying to do anything else with the joint.
*/
void nOdeUniversalJointNode::InitJoint( const char* physContext )
{
  n_assert( !this->joint );
  this->ref_PhysContext = physContext;
  n_assert( this->ref_PhysContext.isvalid() );
  this->joint = this->ref_PhysContext->NewUniversalJoint();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
