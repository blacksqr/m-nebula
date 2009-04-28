#define N_IMPLEMENTS nOdeTriMeshShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeTriMeshShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodetrimeshshapenode.h"

nNebulaScriptClass(nOdeTriMeshShapeNode, "nodecollideshapenode")

#include "odephysics/nodecollideshape.h"
#include "odephysics/nodetrimeshshape.h"

//------------------------------------------------------------------------------
/**
*/
nOdeTriMeshShapeNode::nOdeTriMeshShapeNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeTriMeshShapeNode::~nOdeTriMeshShapeNode()
{
  // empty
}

//------------------------------------------------------------------------------
/**
  @brief Initialise the tri-mesh shape.
  Must be called before trying to use the shape.
*/
void nOdeTriMeshShapeNode::InitShape( const char* id, const char* filename )
{
  this->id = id;
  this->filename = filename;
  if ( filename )
  {
    this->shape = (nOdeCollideShape*)
                    this->ref_OdeServer->NewTriMeshShape( id, filename );
  }
  else
  {
    this->shape = (nOdeCollideShape*)
                    this->ref_OdeServer->NewTriMeshShape( id );
  }
}

//------------------------------------------------------------------------------
/**
*/
void nOdeTriMeshShapeNode::EnableTemporalCoherence( const char* shapeType, 
                                                    bool enable )
{
  nOdeTriMeshShape* shape = (nOdeTriMeshShape*)this->GetShape();
  if ( strcmp( shapeType, "sphere" ) == 0 )
    shape->EnableTemporalCoherence( nOdeCollideShape::OST_SPHERE, enable );
  else if ( strcmp( shapeType, "box" ) == 0 )
    shape->EnableTemporalCoherence( nOdeCollideShape::OST_BOX, enable );
  else if ( strcmp( shapeType, "capsule" ) == 0 )
    shape->EnableTemporalCoherence( nOdeCollideShape::OST_CAPSULE, enable );
  else
    n_error( "Uknown shape type: %s", shapeType );
}

//------------------------------------------------------------------------------
/**
*/
bool nOdeTriMeshShapeNode::IsTemporalCoherenceEnabled( const char* shapeType )
{
  nOdeTriMeshShape* shape = (nOdeTriMeshShape*)this->GetShape();
  if ( strcmp( shapeType, "sphere" ) == 0 )
    return shape->IsTemporalCoherenceEnabled( nOdeCollideShape::OST_SPHERE );
  else if ( strcmp( shapeType, "box" ) == 0 )
    return shape->IsTemporalCoherenceEnabled( nOdeCollideShape::OST_BOX );
  else if ( strcmp( shapeType, "capsule" ) == 0 )
    return shape->IsTemporalCoherenceEnabled( nOdeCollideShape::OST_CAPSULE );
  else
  {
    n_error( "Uknown shape type: %s", shapeType );
    return false;
  }
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
