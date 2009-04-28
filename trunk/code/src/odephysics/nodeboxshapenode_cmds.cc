#define N_IMPLEMENTS nOdeBoxShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeBoxShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeboxshapenode.h"
#include "kernel/npersistserver.h"
#include "odephysics/nodeboxshape.h"

static void n_setsize( void* slf, nCmd* cmd );
static void n_getsize( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodeboxshapenode

  @superclass
  nodecollideshapenode

  @classinfo
  Provides script access to ODE box shapes.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setsize_fff", 'SS__', n_setsize );
  clazz->AddCmd( "fff_getsize_v", 'GS__', n_getsize );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setsize

  @input
  f(lx), f(ly), f(lz)

  @output
  v

  @info
  Set the size of the box.
  For more info lookup dGeomBoxSetLengths in the ODE manual.
*/
static
void
n_setsize( void* slf, nCmd* cmd )
{
  nOdeBoxShapeNode* self = (nOdeBoxShapeNode*)slf;
  float lx = cmd->In()->GetF();
  float ly = cmd->In()->GetF();
  float lz = cmd->In()->GetF();
  ((nOdeBoxShape*)self->GetShape())->SetSize( lx, ly, lz );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getsize

  @input
  v

  @output
  f(lx), f(ly), f(lz)

  @info
  Get the size of the box.
  For more info lookup dGeomBoxGetLengths in the ODE manual.
*/
static
void
n_getsize( void* slf, nCmd* cmd )
{
  nOdeBoxShapeNode* self = (nOdeBoxShapeNode*)slf;
  float lx, ly, lz;
  ((nOdeBoxShape*)self->GetShape())->GetSize( &lx, &ly, &lz );
  cmd->Out()->SetF( lx );
  cmd->Out()->SetF( ly );
  cmd->Out()->SetF( lz );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeBoxShapeNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeCollideShapeNode::SaveCmds( ps ) )
  {
    // setsize
    nCmd* cmd = ps->GetCmd( this, 'SS__' );
    float lx, ly, lz;
    ((nOdeBoxShape*)this->GetShape())->GetSize( &lx, &ly, &lz );
    cmd->In()->SetF( lx );
    cmd->In()->SetF( ly );
    cmd->In()->SetF( lz );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
