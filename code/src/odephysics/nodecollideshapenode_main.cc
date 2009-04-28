#define N_IMPLEMENTS nOdeCollideShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeCollideShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodecollideshapenode.h"

nNebulaScriptClass(nOdeCollideShapeNode, "nroot")

#include "odephysics/nodebodynode.h"
#include "odephysics/nodecollideshape.h"

//------------------------------------------------------------------------------
/**
*/
nOdeCollideShapeNode::nOdeCollideShapeNode()
  : ref_OdeServer(kernelServer, this),
    shape(0),
    autoClean(true)
{
  this->ref_OdeServer = "/sys/servers/ode";
  n_assert( this->ref_OdeServer.isvalid() );
}

//------------------------------------------------------------------------------
/**
*/
nOdeCollideShapeNode::~nOdeCollideShapeNode()
{
  if ( this->autoClean )
  {
    n_assert( this->shape );
    n_assert( this->ref_OdeServer.isvalid() );
    this->ref_OdeServer->ReleaseOdeShape( this->shape );
    this->shape = 0;
  }
}

//------------------------------------------------------------------------------
/**
  @brief Enable/disable auto cleanup mode (enabled by default).
  If auto cleanup is enabled the underlying nOdeCollideShape instance will 
  be released when the node is destroyed, otherwise it is up to you to 
  release the nOdeCollideShape instance.
*/
void
nOdeCollideShapeNode::SetAutoCleanup( bool cleanup )
{
  this->autoClean = cleanup;
}

//------------------------------------------------------------------------------
/**
*/
bool
nOdeCollideShapeNode::GetAutoCleanup() const
{
  return this->autoClean;
}

//------------------------------------------------------------------------------
/**
*/
nOdeCollideShape* 
nOdeCollideShapeNode::GetShape() const
{
  return this->shape;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShapeNode::SetCollClass( const char* collClass )
{
  n_assert( this->ref_OdeServer.isvalid() );
  n_assert( this->shape );
  this->collClass = collClass;
  nOdeCollClass cc = this->ref_OdeServer->QueryCollClass( collClass );
  this->shape->SetCollClass( cc );
}

//------------------------------------------------------------------------------
/**
*/
const char* nOdeCollideShapeNode::GetCollClass()
{
  return this->collClass.c_str();
}
  
//------------------------------------------------------------------------------
/**
*/  
void nOdeCollideShapeNode::AttachTo( const char* bodyPath )
{
  n_assert( this->shape );
  this->bodyPath = bodyPath;
  if ( strcmp( bodyPath, "none" ) == 0 ) // dettach
    this->shape->AttachTo( 0 );
  else
  {
    nOdeBodyNode* node = (nOdeBodyNode*)this->kernelServer->Lookup( bodyPath );
    n_assert( node && "Can't attach shape to non-existant body!" );
    this->shape->AttachTo( node->GetBody() );
  } 
}

//------------------------------------------------------------------------------
/**
*/
const char* nOdeCollideShapeNode::GetBody()
{
  n_assert( this->shape );
  if ( 0 == this->shape->GetBody() )
    return "none";
  return this->bodyPath.c_str();
}

//------------------------------------------------------------------------------
/**
*/
const char* nOdeCollideShapeNode::GetShapeType()
{
  n_assert( this->shape );
  
  switch ( this->shape->GetShapeType() )
  {
    case nOdeCollideShape::OST_SPHERE:
      return "sphere";
    case nOdeCollideShape::OST_BOX:
      return "box";
    case nOdeCollideShape::OST_CAPSULE:
      return "capsule";
    case nOdeCollideShape::OST_MESH:
      return "mesh";
    case nOdeCollideShape::OST_PLANE:
      return "plane";
  }
  
  return "uknown";
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
