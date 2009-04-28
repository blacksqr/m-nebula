#define N_IMPLEMENTS nOdeCapsuleShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeCapsuleShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodecapsuleshapenode.h"
#include "kernel/npersistserver.h"
#include "odephysics/nodecapsuleshape.h"

static void n_setsize( void* slf, nCmd* cmd );
static void n_getsize( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodecapsuleshapenode

  @superclass
  nodecollideshapenode

  @classinfo
  Provides script access to ODE capsule shapes.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setsize_ff", 'SS__', n_setsize );
  clazz->AddCmd( "ff_getsize_v", 'GS__', n_getsize );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setsize

  @input
  f(Radius), f(Length)

  @output
  v

  @info
  Set the size of the capsule.
  For more info lookup dGeomCCylinderSetParams in the ODE manual.
*/
static
void
n_setsize( void* slf, nCmd* cmd )
{
  nOdeCapsuleShapeNode* self = (nOdeCapsuleShapeNode*)slf;
  float radius = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  ((nOdeCapsuleShape*)self->GetShape())->SetSize( radius, length );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getsize

  @input
  v

  @output
  f(Radius), f(Length)

  @info
  Get the size of the capsule.
  For more info lookup dGeomCCylinderGetParams in the ODE manual.
*/
static
void
n_getsize( void* slf, nCmd* cmd )
{
  nOdeCapsuleShapeNode* self = (nOdeCapsuleShapeNode*)slf;
  float radius, length;
  ((nOdeCapsuleShape*)self->GetShape())->GetSize( &radius, &length );
  cmd->Out()->SetF( radius );
  cmd->Out()->SetF( length );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeCapsuleShapeNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeCollideShapeNode::SaveCmds( ps ) )
  {
    // setsize
    nCmd* cmd = ps->GetCmd( this, 'SS__' );
    float radius, length;
    ((nOdeCapsuleShape*)this->GetShape())->GetSize( &radius, &length );
    cmd->In()->SetF( radius );
    cmd->In()->SetF( length );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
