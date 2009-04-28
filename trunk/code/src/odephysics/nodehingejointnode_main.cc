#define N_IMPLEMENTS nOdeHingeJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeHingeJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodehingejointnode.h"

nNebulaScriptClass(nOdeHingeJointNode, "nodejointnode")

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodejoint.h"

//------------------------------------------------------------------------------
/**
*/
nOdeHingeJointNode::nOdeHingeJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeHingeJointNode::~nOdeHingeJointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Create the underlying nOdeHingeJoint instance.
  This must be called before trying to do anything else with the joint.
*/
void nOdeHingeJointNode::InitJoint( const char* physContext )
{
  n_assert( !this->joint );
  this->ref_PhysContext = physContext;
  n_assert( this->ref_PhysContext.isvalid() );
  this->joint = this->ref_PhysContext->NewHingeJoint();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
