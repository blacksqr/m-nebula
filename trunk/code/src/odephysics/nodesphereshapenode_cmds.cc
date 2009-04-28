#define N_IMPLEMENTS nOdeSphereShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeSphereShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodesphereshapenode.h"
#include "kernel/npersistserver.h"
#include "odephysics/nodesphereshape.h"

static void n_setradius( void* slf, nCmd* cmd );
static void n_getradius( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodesphereshapenode

  @superclass
  nodecollideshapenode

  @classinfo
  Provides script access to ODE sphere shapes.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setradius_f", 'SRAD', n_setradius );
  clazz->AddCmd( "f_getradius_v", 'GRAD', n_getradius );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setradius

  @input
  f(Radius)

  @output
  v

  @info
  Set the radius of the sphere.
*/
static
void
n_setradius( void* slf, nCmd* cmd )
{
  nOdeSphereShapeNode* self = (nOdeSphereShapeNode*)slf;
  ((nOdeSphereShape*)self->GetShape())->SetRadius( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getradius

  @input
  v

  @output
  f(Radius)

  @info
  Get the radius of the sphere.
*/
static
void
n_getradius( void* slf, nCmd* cmd )
{
  nOdeSphereShapeNode* self = (nOdeSphereShapeNode*)slf;
  cmd->Out()->SetF( ((nOdeSphereShape*)self->GetShape())->GetRadius() );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeSphereShapeNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeCollideShapeNode::SaveCmds( ps ) )
  {
    // setradius
    nCmd* cmd = ps->GetCmd( this, 'SRAD' );
    cmd->In()->SetF( ((nOdeSphereShape*)this->GetShape())->GetRadius() );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
