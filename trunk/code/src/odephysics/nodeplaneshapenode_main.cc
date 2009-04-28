#define N_IMPLEMENTS nOdePlaneShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdePlaneShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeplaneshapenode.h"

nNebulaScriptClass(nOdePlaneShapeNode, "nodecollideshapenode")

//------------------------------------------------------------------------------
/**
*/
nOdePlaneShapeNode::nOdePlaneShapeNode()
{
  this->shape = (nOdeCollideShape*)this->ref_OdeServer->NewPlaneShape();
}

//------------------------------------------------------------------------------
/**
*/
nOdePlaneShapeNode::~nOdePlaneShapeNode()
{
  // empty
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
