#define N_IMPLEMENTS nOdeHinge2JointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeHinge2JointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodehinge2jointnode.h"

nNebulaScriptClass(nOdeHinge2JointNode, "nodejointnode")

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodejoint.h"

//------------------------------------------------------------------------------
/**
*/
nOdeHinge2JointNode::nOdeHinge2JointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeHinge2JointNode::~nOdeHinge2JointNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Create the underlying nOdeHinge2Joint instance.
  This must be called before trying to do anything else with the joint.
*/
void nOdeHinge2JointNode::InitJoint( const char* physContext )
{
  n_assert( !this->joint );
  this->ref_PhysContext = physContext;
  n_assert( this->ref_PhysContext.isvalid() );
  this->joint = this->ref_PhysContext->NewHinge2Joint();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
