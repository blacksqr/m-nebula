#define N_IMPLEMENTS nOdeSliderJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeSliderJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodesliderjointnode.h"

nNebulaScriptClass(nOdeSliderJointNode, "nodejointnode")

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodejoint.h"

//------------------------------------------------------------------------------
/**
*/
nOdeSliderJointNode::nOdeSliderJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeSliderJointNode::~nOdeSliderJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Create the underlying nOdeSliderJoint instance.
  This must be called before trying to do anything else with the joint.
*/
void nOdeSliderJointNode::InitJoint( const char* physContext )
{
  n_assert( !this->joint );
  this->ref_PhysContext = physContext;
  n_assert( this->ref_PhysContext.isvalid() );
  this->joint = this->ref_PhysContext->NewSliderJoint();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
