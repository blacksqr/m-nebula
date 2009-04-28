#define N_IMPLEMENTS nOdeCollideShapeNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeCollideShapeNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodecollideshapenode.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"
#include "odephysics/nodecollideshape.h"

static void n_setdata( void* slf, nCmd* cmd );
static void n_getdata( void* slf, nCmd* cmd );
static void n_getshapetype( void* slf, nCmd* cmd );
static void n_getid( void* slf, nCmd* cmd );
static void n_setid( void* slf, nCmd* cmd );
static void n_attachto( void* slf, nCmd* cmd );
static void n_getbody( void* slf, nCmd* cmd );
static void n_setrotation( void* slf, nCmd* cmd );
static void n_getrotation( void* slf, nCmd* cmd );
static void n_setposition( void* slf, nCmd* cmd );
static void n_getposition( void* slf, nCmd* cmd );
static void n_setcollclass( void* slf, nCmd* cmd );
static void n_getcollclass( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodecollideshapenode

  @superclass
  nroot

  @classinfo
  Provides script access to collide shapes.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_setdata_o", 'SDAT', n_setdata );
  clazz->AddCmd( "o_getdata_v", 'GDAT', n_getdata );
  clazz->AddCmd( "s_getshapetype_v", 'GST_', n_getshapetype );
  clazz->AddCmd( "i_getid_v", 'GID_', n_getid );
  clazz->AddCmd( "v_setid_i", 'SID_', n_setid );
  clazz->AddCmd( "v_attachto_s", 'AT__', n_attachto );
  clazz->AddCmd( "s_getbody_v", 'GBOD', n_getbody );
  clazz->AddCmd( "v_setrotation_ffff", 'SROT', n_setrotation );
  clazz->AddCmd( "ffff_getrotation_v", 'GROT', n_getrotation );
  clazz->AddCmd( "v_setposition_fff", 'SPOS', n_setposition );
  clazz->AddCmd( "fff_getposition_v", 'GPOS', n_getposition );
  clazz->AddCmd( "v_setcollclass_s", 'SCC_', n_setcollclass );
  clazz->AddCmd( "s_getcollclass_v", 'GCC_', n_getcollclass );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setdata

  @input
  o(UserData)

  @output
  v

  @info
  Associate some user data with the shape.
*/
static
void
n_setdata( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  self->GetShape()->SetData( cmd->In()->GetO() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getdata

  @input
  v

  @output
  o(UserData)

  @info
  Get the user data associated with the shape.
*/
static
void
n_getdata( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  cmd->Out()->SetO( self->GetShape()->GetData() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getshapetype

  @input
  v

  @output
  s(ShapeType)

  @info
  Get the shape type. ShapeType will be one of these:
    - sphere
    - box
    - capsule
    - mesh
    - plane
    - unknown
*/
static
void
n_getshapetype( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  cmd->Out()->SetS( self->GetShapeType() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getid

  @input
  v

  @output
  i(ShapeId)

  @info
  Get the shape's unique identifier.
*/
static
void
n_getid( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  cmd->Out()->SetI( self->GetShape()->GetId() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setid

  @input
  i(ShapeId)

  @output
  v

  @info
  Set the shape's unique identifier. The identifier is typically provided by
  the nOdeServer, so you shouldn't mess with this unless you know what you're
  doing.
*/
static
void
n_setid( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  self->GetShape()->SetId( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  attachto

  @input
  s(BodyNodeName)

  @output
  v

  @info
  Attach the shape to a body. If BodyNodeName is "none" the shape will be
  dettached if it is currently attached to a body.
  For more info lookup dGeomSetBody in the ODE manual.
*/
static
void
n_attachto( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  self->AttachTo( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getbody

  @input
  v

  @output
  s(BodyNodeName)

  @info
  Get the body the shape is currently attached to. BodyNodeName will be "none"
  if the shape is not attached to anything.
  For more info lookup dGeomGetBody in the ODE manual.
*/
static
void
n_getbody( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  cmd->Out()->SetS( self->GetBody() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setrotation

  @input
  f(q.x), f(q.y), f(q.z), f(q.w)

  @output
  v

  @info
  Set the rotation of the shape to the provided quaternion.
*/
static
void
n_setrotation( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  quaternion q;
  q.x = cmd->In()->GetF();
  q.y = cmd->In()->GetF();
  q.z = cmd->In()->GetF();
  q.w = cmd->In()->GetF();
  self->GetShape()->SetRotation( q );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getrotation

  @input
  v

  @output
  f(q.x), f(q.y), f(q.z), f(q.w)

  @info
  Get rotation of the shape as a quaternion.
*/
static
void
n_getrotation( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  quaternion q;
  self->GetShape()->GetRotation( &q );
  cmd->Out()->SetF( q.x );
  cmd->Out()->SetF( q.y );
  cmd->Out()->SetF( q.z );
  cmd->Out()->SetF( q.w );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setposition

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set the position of the shape.
*/
static
void
n_setposition( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->GetShape()->SetPosition( v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getposition

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the position of the shape.
*/
static
void
n_getposition( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  vector3 v;
  self->GetShape()->GetPosition( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setcollclass

  @input
  s(CollisionClass)

  @output
  v

  @info
  Set the collision class of the shape.
*/
static
void
n_setcollclass( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  self->SetCollClass( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getcollclass

  @input
  v

  @output
  s(CollisionClass)

  @info
  Get the collision class of the shape.
*/
static
void
n_getcollclass( void* slf, nCmd* cmd )
{
  nOdeCollideShapeNode* self = (nOdeCollideShapeNode*)slf;
  cmd->Out()->SetS( self->GetCollClass() );
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeCollideShapeNode::SaveCmds( nPersistServer* ps )
{
  if ( nRoot::SaveCmds( ps ) )
  {
    nCmd* cmd;
    
    // Only position/rotate the shape if it isn't attached to a body.
    if ( !this->GetShape()->GetBody() )
    {
      quaternion q;
      vector3 v;
      
      // setrotation
      cmd = ps->GetCmd( this, 'SROT' );
      this->GetShape()->GetRotation( &q );
      cmd->In()->SetF( q.x );
      cmd->In()->SetF( q.y );
      cmd->In()->SetF( q.z );
      cmd->In()->SetF( q.w );
      ps->PutCmd( cmd );  
      
      // setposition
      cmd = ps->GetCmd( this, 'SPOS' );
      this->GetShape()->GetPosition( &v );
      cmd->In()->SetF( v.x );
      cmd->In()->SetF( v.y );
      cmd->In()->SetF( v.z );
      ps->PutCmd( cmd );
    }
    else
    {
      // attachto
      cmd = ps->GetCmd( this, 'AT__' );
      cmd->In()->SetS( this->GetBody() );
      ps->PutCmd( cmd );
    }
    
    // setcollclass
    cmd = ps->GetCmd( this, 'SCC_' );
    cmd->In()->SetS( this->GetCollClass() );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
