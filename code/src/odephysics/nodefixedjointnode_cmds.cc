#define N_IMPLEMENTS nOdeFixedJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeFixedJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodefixedjointnode.h"
#include "kernel/npersistserver.h"
#include "odephysics/nodejoint.h"

static void n_fix( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodefixedjointnode

  @superclass
  nodejointnode

  @classinfo
  Provides script access to ODE fixed joints.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_fix_v", 'FIX_', n_fix );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  fix

  @input
  v

  @output
  v

  @info
  Call this on the fixed joint after it has been attached to remember 
  the current desired relative offset between the bodies it is attached to.
*/
static
void
n_fix( void* slf, nCmd* cmd )
{
  nOdeFixedJointNode* self = (nOdeFixedJointNode*)slf;
  ((nOdeFixedJoint*)self->GetJoint())->Fix();
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeFixedJointNode::SaveCmds( nPersistServer* ps )
{
  if ( nOdeJointNode::SaveCmds( ps ) )
  {
    nCmd* cmd = ps->GetCmd( this, 'FIX_' );
    ps->PutCmd( cmd );
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
