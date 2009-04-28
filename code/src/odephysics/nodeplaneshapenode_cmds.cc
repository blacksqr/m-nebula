#define N_IMPLEMENTS nOdePlaneShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdePlaneShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodeplaneshapenode.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"
#include "odephysics/nodeplaneshape.h"

static void n_setparams( void* slf, nCmd* cmd );
static void n_getparams( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodeplaneshapenode

  @superclass
  nodecollideshapenode

  @classinfo
  Provides script access to ODE plane shapes.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setparams_ffff", 'SPAR', n_setparams );
  clazz->AddCmd( "ffff_getparams_v", 'GPAR', n_getparams );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setparams

  @input
  f(a), f(b), f(c), f(d)

  @output
  v

  @info
  Set the plane equation.
  For more info lookup dGeomPlaneSetParams in the ODE manual.
*/
static
void
n_setparams( void* slf, nCmd* cmd )
{
  nOdePlaneShapeNode* self = (nOdePlaneShapeNode*)slf;
  vector3 n;
  float d;
  n.x = cmd->In()->GetF();
  n.y = cmd->In()->GetF();
  n.z = cmd->In()->GetF();
  d = cmd->In()->GetF();
  ((nOdePlaneShape*)self->GetShape())->SetParams( n, d );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getparams

  @input
  v

  @output
  f(a), f(b), f(c), f(d)

  @info
  Get the plane equation.
*/
static
void
n_getparams( void* slf, nCmd* cmd )
{
  nOdePlaneShapeNode* self = (nOdePlaneShapeNode*)slf;
  vector3 n;
  float d;
  ((nOdePlaneShape*)self->GetShape())->GetParams( &n, &d );
  cmd->Out()->SetF( n.x );
  cmd->Out()->SetF( n.y );
  cmd->Out()->SetF( n.z );
  cmd->Out()->SetF( d );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdePlaneShapeNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeCollideShapeNode::SaveCmds( ps ) )
  {
    // setparams
    nCmd* cmd = ps->GetCmd( this, 'SPAR' );
    vector3 n;
    float d;
    ((nOdePlaneShape*)this->GetShape())->GetParams( &n, &d );
    cmd->In()->SetF( n.x );
    cmd->In()->SetF( n.y );
    cmd->In()->SetF( n.z );
    cmd->In()->SetF( d );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
