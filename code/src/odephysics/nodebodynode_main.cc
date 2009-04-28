#define N_IMPLEMENTS nOdeBodyNode
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//
//  nOdeBodyNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodebodynode.h"

nNebulaScriptClass(nOdeBodyNode, "nroot");

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodemassnode.h"

//------------------------------------------------------------------------------
/**
*/
nOdeBodyNode::nOdeBodyNode()
  : body(0), autoClean(true), ref_PhysContext(kernelServer, this)
{
  // empty
}

//------------------------------------------------------------------------------
/**
*/
nOdeBodyNode::~nOdeBodyNode()
{
  if ( this->autoClean )
  {
    n_assert( this->body );
    n_assert( this->ref_PhysContext.isvalid() );
    this->ref_PhysContext->ReleaseOdeBody( this->body );
    this->body = 0;
  }
}

//------------------------------------------------------------------------------
/**
  @brief Create the underlying nOdeBody instance.
  This obviously must be called before trying to do anything else with
  the body.
*/
void nOdeBodyNode::InitBody( const char* physContextPath )
{
  n_assert( !this->body && "nOdeBodyNode:InitBody(): body already initialised!" );
  this->ref_PhysContext = physContextPath;
  n_assert( this->ref_PhysContext.isvalid() );
  this->body = this->ref_PhysContext->NewOdeBody();
}

//------------------------------------------------------------------------------
/**
*/
void nOdeBodyNode::CopyMass( const char* massPath )
{
  nRoot* node = this->kernelServer->Lookup( massPath );
  n_assert( node );
  n_assert( node->IsA( this->kernelServer->FindClass( "nodemassnode" ) ) );
  this->GetBody()->SetMass( ((nOdeMassNode*)node)->GetMass() );
}
  
//------------------------------------------------------------------------------
/**
*/  
void nOdeBodyNode::SetMassTotal( float totalMass )
{
  nOdeMass mass;
  this->GetBody()->GetMass( &mass );
  mass.AdjustTo( totalMass );
  this->GetBody()->SetMass( mass );
}

//------------------------------------------------------------------------------
/**
*/
float nOdeBodyNode::GetMassTotal()
{
  nOdeMass mass;
  this->GetBody()->GetMass( &mass );
  return mass.GetTotalMass();
}

//------------------------------------------------------------------------------
/**
*/
void nOdeBodyNode::SetMassCOG( const vector3& cog )
{
  nOdeMass mass;
  this->GetBody()->GetMass( &mass );
  mass.SetCenterOfGravity( cog );
  this->GetBody()->SetMass( mass );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeBodyNode::GetMassCOG( vector3* cog )
{
  nOdeMass mass;
  this->GetBody()->GetMass( &mass );
  mass.GetCenterOfGravity( cog );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeBodyNode::SetMassInertia( const matrix33& inertia )
{
  nOdeMass mass;
  this->GetBody()->GetMass( &mass );
  mass.SetInertia( inertia );
  this->GetBody()->SetMass( mass );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeBodyNode::GetMassInertia( matrix33* inertia )
{
  nOdeMass mass;
  this->GetBody()->GetMass( &mass );
  mass.GetInertia( inertia );
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
