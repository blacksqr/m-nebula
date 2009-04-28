#define N_IMPLEMENTS nOdeHashSpace
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//------------------------------------------------------------------------------
#include "odephysics/nodehashspace.h"
#include "kernel/npersistserver.h"

static void n_setlevels( void* slf, nCmd* cmd );
static void n_getlevels( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodehashspace

  @superclass
  nodecollidespace

  @classinfo
  Encapsulates an ODE hash space.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setlevels_ii", 'SLVL', n_setlevels );
  clazz->AddCmd( "ii_getlevels_v", 'GLVL', n_getlevels );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setlevels

  @input
  i(MinLevel), i(MaxLevel)

  @output
  v

  @info
  Set the smallest and largest cell sizes to be used in the hash space.
  For more information lookup dHashSpaceSetLevels in the ODE manual.
*/
static
void
n_setlevels( void* slf, nCmd* cmd )
{
  nOdeHashSpace* self = (nOdeHashSpace*)slf;
  int minLevel = cmd->In()->GetI();
  int maxLevel = cmd->In()->GetI();
  self->SetLevels( minLevel, maxLevel );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getlevels

  @input
  v

  @output
  i(MinLevel), i(MaxLevel)

  @info
  Get the smallest and largest cell sizes to be used in the hash space.
  For more information lookup dHashSpaceSetLevels in the ODE manual.
*/
static
void
n_getlevels( void* slf, nCmd* cmd )
{
  nOdeHashSpace* self = (nOdeHashSpace*)slf;
  int minLevel, maxLevel;
  self->GetLevels( &minLevel, &maxLevel );
  cmd->Out()->SetI( minLevel );
  cmd->Out()->SetI( maxLevel );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeHashSpace::SaveCmds( nPersistServer* ps )
{
  if ( nOdeCollideSpace::SaveCmds( ps ) )
  {
    // setlevels
    int minLevel, maxLevel;
    this->GetLevels( &minLevel, &maxLevel );
    nCmd* cmd = ps->GetCmd( this, 'SLVL' );
    cmd->In()->SetI( minLevel );
    cmd->In()->SetI( maxLevel );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
