#define N_IMPLEMENTS nOdeQuadTreeSpace
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//------------------------------------------------------------------------------
#include "odephysics/nodequadtreespace.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"

static void n_setparams( void* slf, nCmd* cmd );
static void n_getparams( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodequadtreespace

  @superclass
  nodecollidespace

  @classinfo
  Encapsulates an ODE quad tree space.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setparams_ffffffi", 'SPAR', n_setparams );
  clazz->AddCmd( "ffffffi_getparams_v", 'GPAR', n_getparams );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setparams

  @input
  f(center.x), f(center.y), f(center.z), 
  f(extents.x), f(extents.y), f(extents.z), 
  i(depth)

  @output
  v

  @info
  Creates a quadtree space. 
  center and extents define the size of the root block. 
  depth sets the depth of the tree - the number of blocks that are created 
                                     is 4^depth.
*/
static
void
n_setparams( void* slf, nCmd* cmd )
{
  nOdeQuadTreeSpace* self = (nOdeQuadTreeSpace*)slf;
  vector3 center, extents;
  center.x = cmd->In()->GetF();
  center.y = cmd->In()->GetF();
  center.z = cmd->In()->GetF();
  extents.x = cmd->In()->GetF();
  extents.x = cmd->In()->GetF();
  extents.x = cmd->In()->GetF();
  int depth = cmd->In()->GetI();
  self->SetParams( center, extents, depth );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getparams

  @input
  v

  @output
  f(center.x), f(center.y), f(center.z), 
  f(extents.x), f(extents.y), f(extents.z), 
  i(depth)

  @info
  See getparams for info.
*/
static
void
n_getparams( void* slf, nCmd* cmd )
{
  nOdeQuadTreeSpace* self = (nOdeQuadTreeSpace*)slf;
  vector3 center, extents;
  int depth;
  self->GetParams( &center, &extents, &depth );
  cmd->Out()->SetF( center.x );
  cmd->Out()->SetF( center.y );
  cmd->Out()->SetF( center.z );
  cmd->Out()->SetF( extents.x );
  cmd->Out()->SetF( extents.y );
  cmd->Out()->SetF( extents.z );
  cmd->Out()->SetI( depth );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeQuadTreeSpace::SaveCmds( nPersistServer* ps )
{
  if ( nOdeCollideSpace::SaveCmds( ps ) )
  {
    // setparams
    nCmd* cmd = ps->GetCmd( this, 'SPAR' );
    vector3 center, extents;
    int depth;
    this->GetParams( &center, &extents, &depth );
    cmd->In()->SetF( center.x );
    cmd->In()->SetF( center.y );
    cmd->In()->SetF( center.z );
    cmd->In()->SetF( extents.x );
    cmd->In()->SetF( extents.y );
    cmd->In()->SetF( extents.z );
    cmd->In()->SetI( depth );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
