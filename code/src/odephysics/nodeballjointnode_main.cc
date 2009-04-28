#define N_IMPLEMENTS nOdeBallJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeBallJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeballjointnode.h"

nNebulaScriptClass(nOdeBallJointNode, "nodejointnode")

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodejoint.h"

//------------------------------------------------------------------------------
/**
*/
nOdeBallJointNode::nOdeBallJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeBallJointNode::~nOdeBallJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Create the underlying nOdeBallJoint instance.
  This obviously must be called before trying to do anything else with
  the joint.
*/
void nOdeBallJointNode::InitJoint( const char* physContext )
{
  n_assert( !this->joint );
  this->ref_PhysContext = physContext;
  n_assert( this->ref_PhysContext.isvalid() );
  this->joint = this->ref_PhysContext->NewBallJoint();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
