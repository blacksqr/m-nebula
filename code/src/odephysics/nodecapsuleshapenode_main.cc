#define N_IMPLEMENTS nOdeCapsuleShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeCapsuleShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodecapsuleshapenode.h"

nNebulaScriptClass(nOdeCapsuleShapeNode, "nodecollideshapenode")

//------------------------------------------------------------------------------
/**
*/
nOdeCapsuleShapeNode::nOdeCapsuleShapeNode()
{
  this->shape = (nOdeCollideShape*)this->ref_OdeServer->NewCapsuleShape();
}

//------------------------------------------------------------------------------
/**
*/
nOdeCapsuleShapeNode::~nOdeCapsuleShapeNode()
{
  // empty
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
