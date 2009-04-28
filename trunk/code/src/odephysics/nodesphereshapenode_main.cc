#define N_IMPLEMENTS nOdeSphereShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeSphereShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodesphereshapenode.h"

nNebulaScriptClass(nOdeSphereShapeNode, "nodecollideshapenode")

//------------------------------------------------------------------------------
/**
*/
nOdeSphereShapeNode::nOdeSphereShapeNode()
{
  this->shape = (nOdeCollideShape*)this->ref_OdeServer->NewSphereShape();
}

//------------------------------------------------------------------------------
/**
*/
nOdeSphereShapeNode::~nOdeSphereShapeNode()
{
  // empty
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
