#define N_IMPLEMENTS nOdeTriMeshShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeTriMeshShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodetrimeshshapenode.h"
#include "kernel/npersistserver.h"

static void n_initshape( void* slf, nCmd* cmd );
static void n_initshape2( void* slf, nCmd* cmd );
static void n_enabletempcoherence( void* slf, nCmd* cmd );
static void n_istempcoherenceenabled( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodetrimeshshapenode

  @superclass
  nodecollideshapenode

  @classinfo
  Provides script access to ODE tri-mesh shapes.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_initshape_s", 'IS__', n_initshape );
  clazz->AddCmd( "v_initshape2_ss", 'IS2_', n_initshape2 );
  clazz->AddCmd( "v_enabletempcoherence_sb", 'ETC_', n_enabletempcoherence );
  clazz->AddCmd( "b_istempcoherenceenabled_s", 'ITCE', n_istempcoherenceenabled );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  initshape

  @input
  s(ShapeId)

  @output
  v

  @info
  Semi-initialise the shape. 
  If a shape has been previously created with the provided ShapeId the 
  tri-mesh will be shared, otherwise you will have to load the tri-mesh 
  before using the shape.
*/
static
void
n_initshape( void* slf, nCmd* cmd )
{
  nOdeTriMeshShapeNode* self = (nOdeTriMeshShapeNode*)slf;
  self->InitShape( cmd->In()->GetS(), 0 );
}

//------------------------------------------------------------------------------
/**
  @cmd
  initshape2

  @input
  s(ShapeId), s(TriMeshFileName)

  @output
  v

  @info
  Initialise the shape. 
  If a shape has been previously created with the provided ShapeId the 
  tri-mesh will be shared.
*/
static
void
n_initshape2( void* slf, nCmd* cmd )
{
  nOdeTriMeshShapeNode* self = (nOdeTriMeshShapeNode*)slf;
  const char* id = cmd->In()->GetS();
  const char* filename = cmd->In()->GetS();
  self->InitShape( id, filename );
}

//------------------------------------------------------------------------------
/**
  @cmd
  enabletempcoherence

  @input
  s(ShapeType), b(True/False)

  @output
  v

  @info
  Enable/disable temporal coherence for shapes of type ShapeType.
  ShapeType can be one of:
    - sphere
    - box
    - capsule
*/
static
void
n_enabletempcoherence( void* slf, nCmd* cmd )
{
  nOdeTriMeshShapeNode* self = (nOdeTriMeshShapeNode*)slf;
  const char* shapetype = cmd->In()->GetS();
  bool enable = cmd->In()->GetB();
  self->EnableTemporalCoherence( shapetype, enable );
}

//------------------------------------------------------------------------------
/**
  @cmd
  istempcoherenceenabled

  @input
  s(ShapeType)

  @output
  b(True/False)

  @info
  Check if temporal coherence is enabled for shapes of type ShapeType.
  ShapeType can be one of:
    - sphere
    - box
    - capsule
*/
static
void
n_istempcoherenceenabled( void* slf, nCmd* cmd )
{
  nOdeTriMeshShapeNode* self = (nOdeTriMeshShapeNode*)slf;
  const char* shapetype = cmd->In()->GetS();
  cmd->Out()->SetB( self->IsTemporalCoherenceEnabled( shapetype ) );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeTriMeshShapeNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeCollideShapeNode::SaveCmds( ps ) )
  {
    nCmd* cmd;
    
    // initshape
    if ( this->filename.c_str() )
    {
      cmd = ps->GetCmd( this, 'IS2_' );
      cmd->In()->SetS( this->id.c_str() );
      cmd->In()->SetS( this->filename.c_str() );
    }
    else
    {
      cmd = ps->GetCmd( this, 'IS__' );
      cmd->In()->SetS( this->id.c_str() );
    }
    ps->PutCmd( cmd );
    
    // enabletempcoherence
    
    cmd = ps->GetCmd( this, 'ETC_' );
    cmd->In()->SetS( "sphere" );
    cmd->In()->SetB( this->IsTemporalCoherenceEnabled( "sphere" ) );
    ps->PutCmd( cmd );
    
    cmd = ps->GetCmd( this, 'ETC_' );
    cmd->In()->SetS( "box" );
    cmd->In()->SetB( this->IsTemporalCoherenceEnabled( "box" ) );
    ps->PutCmd( cmd );
    
    cmd = ps->GetCmd( this, 'ETC_' );
    cmd->In()->SetS( "capsule" );
    cmd->In()->SetB( this->IsTemporalCoherenceEnabled( "capsule" ) );
    ps->PutCmd( cmd );
  
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
