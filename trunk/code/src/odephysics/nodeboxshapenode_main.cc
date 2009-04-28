#define N_IMPLEMENTS nOdeBoxShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeBoxShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeboxshapenode.h"

nNebulaScriptClass(nOdeBoxShapeNode, "nodecollideshapenode")

//------------------------------------------------------------------------------
/**
*/
nOdeBoxShapeNode::nOdeBoxShapeNode()
{
  this->shape = (nOdeCollideShape*)this->ref_OdeServer->NewBoxShape();
}

//------------------------------------------------------------------------------
/**
*/
nOdeBoxShapeNode::~nOdeBoxShapeNode()
{
  // empty
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
