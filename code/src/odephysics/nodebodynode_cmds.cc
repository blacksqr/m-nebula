#define N_IMPLEMENTS nOdeBodyNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeBodyNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodebodynode.h"
#include "kernel/npersistserver.h"
#include "mathlib/vector.h"

static void n_initbody( void* slf, nCmd* cmd );
static void n_setautocleanup( void* slf, nCmd* cmd );
static void n_getautocleanup( void* slf, nCmd* cmd );
static void n_setrotation( void* slf, nCmd* cmd );
static void n_getrotation( void* slf, nCmd* cmd );
static void n_setposition( void* slf, nCmd* cmd );
static void n_getposition( void* slf, nCmd* cmd );
static void n_setlinearvel( void* slf, nCmd* cmd );
static void n_getlinearvel( void* slf, nCmd* cmd );
static void n_setangularvel( void* slf, nCmd* cmd );
static void n_getangularvel( void* slf, nCmd* cmd );
static void n_addforce( void* slf, nCmd* cmd );
static void n_addtorque( void* slf, nCmd* cmd );
static void n_addrelforce( void* slf, nCmd* cmd );
static void n_addreltorque( void* slf, nCmd* cmd );
static void n_addforceatpos( void* slf, nCmd* cmd );
static void n_addforceatrelpos( void* slf, nCmd* cmd );
static void n_addrelforceatpos( void* slf, nCmd* cmd );
static void n_addrelforceatrelpos( void* slf, nCmd* cmd );
static void n_getforce( void* slf, nCmd* cmd );
static void n_gettorque( void* slf, nCmd* cmd );
static void n_setforce( void* slf, nCmd* cmd );
static void n_settorque( void* slf, nCmd* cmd );
static void n_getrelpointpos( void* slf, nCmd* cmd );
static void n_getrelpointvel( void* slf, nCmd* cmd );
static void n_getpointvel( void* slf, nCmd* cmd );
static void n_getposrelpoint( void* slf, nCmd* cmd );
static void n_vectortoworld( void* slf, nCmd* cmd );
static void n_vectorfromworld( void* slf, nCmd* cmd );
static void n_enable( void* slf, nCmd* cmd );
static void n_isenabled( void* slf, nCmd* cmd );
static void n_setfiniterotationmode( void* slf, nCmd* cmd );
static void n_getfiniterotationmode( void* slf, nCmd* cmd );
static void n_setfiniterotationaxis( void* slf, nCmd* cmd );
static void n_getfiniterotationaxis( void* slf, nCmd* cmd );
static void n_getnumjoints( void* slf, nCmd* cmd );
static void n_enablegravity( void* slf, nCmd* cmd );
static void n_isgravityenabled( void* slf, nCmd* cmd );
static void n_connectedto( void* slf, nCmd* cmd );
static void n_connectedtoex( void* slf, nCmd* cmd );
static void n_setmasstotal( void* slf, nCmd* cmd );
static void n_getmasstotal( void* slf, nCmd* cmd );
static void n_setmasscog( void* slf, nCmd* cmd );
static void n_getmasscog( void* slf, nCmd* cmd );
static void n_setmassinertia( void* slf, nCmd* cmd );
static void n_getmassinertia( void* slf, nCmd* cmd );
static void n_copymass( void* slf, nCmd* cmd );

#ifdef ODE_STEPFAST_AUTO_DISABLE
static void n_setautodisablethreshold( void* slf, nCmd* cmd );
static void n_getautodisablethreshold( void* slf, nCmd* cmd );
static void n_setautodisablesteps( void* slf, nCmd* cmd );
static void n_getautodisablesteps( void* slf, nCmd* cmd );
static void n_setautodisable( void* slf, nCmd* cmd );
static void n_getautodisable( void* slf, nCmd* cmd );
#endif // ODE_STEPFAST_AUTO_DISABLE

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodebodynode

  @superclass
  nroot

  @classinfo
  Provides script access to ODE bodies.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_initbody_s", 'IB__', n_initbody );
  clazz->AddCmd( "v_setautocleanup_b", 'SAC_', n_setautocleanup );
  clazz->AddCmd( "b_getautocleanup_v", 'GAC_', n_getautocleanup );
  clazz->AddCmd( "v_setrotation_ffff", 'SRQ_', n_setrotation );
  clazz->AddCmd( "ffff_getrotation_v", 'GRQ_', n_getrotation );
  clazz->AddCmd( "v_setposition_fff", 'SP__', n_setposition );
  clazz->AddCmd( "fff_getposition_v", 'GP__', n_getposition );
  clazz->AddCmd( "v_setlinearvel_fff", 'SLV_', n_setlinearvel );
  clazz->AddCmd( "fff_getlinearvel_v", 'GLV_', n_getlinearvel );
  clazz->AddCmd( "v_setangularvel_fff", 'SAV_', n_setangularvel );
  clazz->AddCmd( "fff_getangularvel_v", 'GAV_', n_getangularvel );
  clazz->AddCmd( "v_addforce_fff", 'AF__', n_addforce );
  clazz->AddCmd( "v_addtorque_fff", 'AT__', n_addtorque );
  clazz->AddCmd( "v_addrelforce_fff", 'ARF_', n_addrelforce );
  clazz->AddCmd( "v_addreltorque_fff", 'ART_', n_addreltorque );
  clazz->AddCmd( "v_addforceatpos_ffffff", 'AFAP', n_addforceatpos );
  clazz->AddCmd( "v_addforceatrelpos_ffffff", 'AFRP', n_addforceatrelpos );
  clazz->AddCmd( "v_addrelforceatpos_ffffff", 'ARFP', n_addrelforceatpos );
  clazz->AddCmd( "v_addrelforceatrelpos_ffffff", 'RFRP', n_addrelforceatrelpos );
  clazz->AddCmd( "fff_getforce_v", 'GF__', n_getforce );
  clazz->AddCmd( "fff_gettorque_v", 'GT__', n_gettorque );
  clazz->AddCmd( "v_setforce_fff", 'SF__', n_setforce );
  clazz->AddCmd( "v_settorque_fff", 'ST__', n_settorque );
  clazz->AddCmd( "fff_getrelpointpos_fff", 'GRPP', n_getrelpointpos );
  clazz->AddCmd( "fff_getrelpointvel_fff", 'GRPV', n_getrelpointvel );
  clazz->AddCmd( "fff_getpointvel_fff", 'GPV_', n_getpointvel );
  clazz->AddCmd( "fff_getposrelpoint_fff", 'GPRP', n_getposrelpoint );
  clazz->AddCmd( "fff_vectortoworld_fff", 'VTW_', n_vectortoworld );
  clazz->AddCmd( "fff_vectorfromworld_fff", 'VFW_', n_vectorfromworld );
  clazz->AddCmd( "v_enable_b", 'E___', n_enable );
  clazz->AddCmd( "b_isenabled_v", 'IE__', n_isenabled );
  clazz->AddCmd( "v_setfiniterotationmode_i", 'SFRM', n_setfiniterotationmode );
  clazz->AddCmd( "i_getfiniterotationmode_v", 'GFRM', n_getfiniterotationmode );
  clazz->AddCmd( "v_setfiniterotationaxis_fff", 'SFRA', n_setfiniterotationaxis );
  clazz->AddCmd( "fff_getfiniterotationaxis_v", 'GFRA', n_getfiniterotationaxis );
  clazz->AddCmd( "i_getnumjoints_v", 'GNJ_', n_getnumjoints );
  clazz->AddCmd( "v_enablegravity_b", 'EG__', n_enablegravity );
  clazz->AddCmd( "b_isgravityenabled_v", 'IGE_', n_isgravityenabled );
  clazz->AddCmd( "b_connectedto_s", 'CT__', n_connectedto );
  clazz->AddCmd( "b_connectedtoex_si", 'CTE_', n_connectedtoex );
  clazz->AddCmd( "v_setmasstotal_f", 'SMT_', n_setmasstotal );
  clazz->AddCmd( "f_getmasstotal_v", 'GMT_', n_getmasstotal );
  clazz->AddCmd( "v_setmasscog_fff", 'SMCG', n_setmasscog );
  clazz->AddCmd( "fff_getmasscog_v", 'GMCG', n_getmasscog );
  clazz->AddCmd( "v_setmassinertia_ffffff", 'SMI_', n_setmassinertia );
  clazz->AddCmd( "ffffff_getmassinertia_v", 'GMI_', n_getmassinertia );
  clazz->AddCmd( "v_copymass_s", 'CPYM', n_copymass );
#ifdef ODE_STEPFAST_AUTO_DISABLE
  clazz->AddCmd( "v_setautodisablethreshold_f", 'SADT', n_setautodisablethreshold );
  clazz->AddCmd( "f_getautodisablethreshold_v", 'GADT', n_getautodisablethreshold );
  clazz->AddCmd( "v_setautodisablesteps_i", 'SADS', n_setautodisablesteps );
  clazz->AddCmd( "i_getautodisablesteps_v", 'GADS', n_getautodisablesteps );
  clazz->AddCmd( "v_setautodisable_b", 'SAD_', n_setautodisable );
  clazz->AddCmd( "b_getautodisable_v", 'GAD_', n_getautodisable );
#endif // ODE_STEPFAST_AUTO_DISABLE
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  initbody

  @input
  s(PhysicsContext)

  @output
  v 

  @info
  Initialise the body, must be called before trying to use any of the other 
  commands.
*/
static
void
n_initbody( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->InitBody( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setautocleanup

  @input
  b(True/False)

  @output
  v

  @info
  If enabled the lightweight C++ body will be destroyed along with the node.
  This is enabled by default.
*/
static
void
n_setautocleanup( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->SetAutoCleanup( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getautocleanup

  @input
  v

  @output
  b(True/False) 

  @info
  Check if the lightweight C++ instance will be destroyed along with the node.
*/
static
void
n_getautocleanup( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetB( self->GetAutoCleanup() );
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
  Set the rotation of the body to the provided quaternion.
*/
static
void
n_setrotation( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  float w = cmd->In()->GetF();
  self->GetBody()->SetRotation( quaternion(x, y, z, w) );
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
  Get rotation of the body as a quaternion.
*/
static
void
n_getrotation( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  quaternion q;
  self->GetBody()->GetRotation( &q );
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
  Set the position of the body.
*/
static
void
n_setposition( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->GetBody()->SetPosition( vector3(x, y, z) );
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
  Get the position of the body.
*/
static
void
n_getposition( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  vector3 pos;
  self->GetBody()->GetPosition( &pos );
  cmd->Out()->SetF( pos.x );
  cmd->Out()->SetF( pos.y );
  cmd->Out()->SetF( pos.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setlinearvel

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set linear velocity of the body.
*/
static
void
n_setlinearvel( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->GetBody()->SetLinearVel( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getlinearvel

  @input
  v

  @output
  f(x), f(y), f(z) 

  @info
  Get the linear velocity of the body.
*/
static
void
n_getlinearvel( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  vector3 vel;
  self->GetBody()->GetLinearVel( &vel );
  cmd->Out()->SetF( vel.x );
  cmd->Out()->SetF( vel.y );
  cmd->Out()->SetF( vel.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setangularvel

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set the angular velocity of the body.
*/
static
void
n_setangularvel( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->GetBody()->SetAngularVel( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getangularvel

  @input
  v

  @output
  f(x), f(y), f(z) 

  @info
  Get the angular velocity of the body.
*/
static
void
n_getangularvel( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  vector3 vel;
  self->GetBody()->GetAngularVel( &vel );
  cmd->Out()->SetF( vel.x );
  cmd->Out()->SetF( vel.y );
  cmd->Out()->SetF( vel.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  addforce

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  See ODE manual. (dBodyAddForce)
*/
static
void
n_addforce( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->GetBody()->AddForce( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  addtorque

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  See ODE manual. (dBodyAddTorque)
*/
static
void
n_addtorque( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->GetBody()->AddTorque( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  addrelforce

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  See ODE manual. (dBodyAddRelForce)
*/
static
void
n_addrelforce( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->GetBody()->AddRelForce( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  addreltorque

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  See ODE manual. (dBodyAddRelTorque)
*/
static
void
n_addreltorque( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->GetBody()->AddRelTorque( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  addforceatpos

  @input
  f(fx), f(fy), f(fz), f(px), f(py), f(pz)

  @output
  v

  @info
  See ODE manual. (dBodyAddForceAtPos)
*/
static
void
n_addforceatpos( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  float px = cmd->In()->GetF();
  float py = cmd->In()->GetF();
  float pz = cmd->In()->GetF();
  self->GetBody()->AddForceAtPos( vector3(fx, fy, fz), vector3( px, py, pz ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  addforceatrelpos

  @input
  f(fx), f(fy), f(fz), f(px), f(py), f(pz)

  @output
  v

  @info
  See ODE manual. (dBodyAddForceAtRelPos)
*/
static
void
n_addforceatrelpos( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  float px = cmd->In()->GetF();
  float py = cmd->In()->GetF();
  float pz = cmd->In()->GetF();
  self->GetBody()->AddForceAtRelPos( vector3(fx, fy, fz), vector3(px, py, pz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  addrelforceatpos

  @input
  f(fx), f(fy), f(fz), f(px), f(py), f(pz)

  @output
  v

  @info
  See ODE manual. (dBodyAddRelForceAtPos)
*/
static
void
n_addrelforceatpos( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  float px = cmd->In()->GetF();
  float py = cmd->In()->GetF();
  float pz = cmd->In()->GetF();
  self->GetBody()->AddRelForceAtPos( vector3(fx, fy, fz), vector3(px, py, pz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  addrelforceatrelpos

  @input
  f(fx), f(fy), f(fz), f(px), f(py), f(pz)

  @output
  v

  @info
  See ODE manual. (dBodyAddRelForceAtRelPos)
*/
static
void
n_addrelforceatrelpos( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  float px = cmd->In()->GetF();
  float py = cmd->In()->GetF();
  float pz = cmd->In()->GetF();
  self->GetBody()->AddRelForceAtRelPos( vector3(fx, fy, fz), vector3(px, py, pz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getforce

  @input
  v

  @output
  f(fx), f(fy), f(fz) 

  @info
  Get the accumulated force vector for the body.
*/
static
void
n_getforce( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  vector3 f;
  self->GetBody()->GetForce( &f );
  cmd->Out()->SetF( f.x );
  cmd->Out()->SetF( f.y );
  cmd->Out()->SetF( f.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  gettorque

  @input
  v

  @output
  f(fx), f(fy), f(fz) 

  @info
  Get the accumulated torque vector for the body.
*/
static
void
n_gettorque( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  vector3 f;
  self->GetBody()->GetTorque( &f );
  cmd->Out()->SetF( f.x );
  cmd->Out()->SetF( f.y );
  cmd->Out()->SetF( f.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setforce

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  Set the accumulated force vector for the body.
*/
static
void
n_setforce( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  self->GetBody()->SetForce( vector3(fx, fy, fz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  settorque

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  Set the accumulated torque vector for the body.
*/
static
void
n_settorque( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  self->GetBody()->SetTorque( vector3(fx, fy, fz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getrelpointpos

  @input
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @output
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @info
  See ODE manual. (dBodyGetRelPointPos)
*/
static
void
n_getrelpointpos( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float brx = cmd->In()->GetF();
  float bry = cmd->In()->GetF();
  float brz = cmd->In()->GetF();
  vector3 res;
  self->GetBody()->GetRelPointPos( vector3(brx, bry, brz), &res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getrelpointvel

  @input
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @output
  f(GlobalVelX), f(GlobalVelY), f(GlobalVelZ) 

  @info
  See ODE manual. (dBodyGetRelPointVel)
*/
static
void
n_getrelpointvel( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float brx = cmd->In()->GetF();
  float bry = cmd->In()->GetF();
  float brz = cmd->In()->GetF();
  vector3 res;
  self->GetBody()->GetRelPointVel( vector3(brx, bry, brz), &res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getpointvel

  @input
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @output
  f(GlobalVelX), f(GlobalVelY), f(GlobalVelZ)

  @info
  See ODE manual. (dBodyGetPointVel)
*/
static
void
n_getpointvel( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float gx = cmd->In()->GetF();
  float gy = cmd->In()->GetF();
  float gz = cmd->In()->GetF();
  vector3 res;
  self->GetBody()->GetPointVel( vector3(gx, gy, gz), &res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getposrelpoint

  @input
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @output
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @info
  See ODE manual. (dBodyGetPosRelPoint)
*/
static
void
n_getposrelpoint( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float gx = cmd->In()->GetF();
  float gy = cmd->In()->GetF();
  float gz = cmd->In()->GetF();
  vector3 res;
  self->GetBody()->GetPosRelPoint( vector3(gx, gy, gz), &res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  vectortoworld

  @input
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @output
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @info
  Transform a vector expressed in the body coordinate system to 
  the global/world coordinate system.
*/
static
void
n_vectortoworld( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float brx = cmd->In()->GetF();
  float bry = cmd->In()->GetF();
  float brz = cmd->In()->GetF();
  vector3 res;
  self->GetBody()->VectorToWorld( vector3(brx, bry, brz), &res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  vectorfromworld

  @input
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @output
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @info
  Transform a vector expressed in the global/world coordinate system
  to the body coordinate system.
*/
static
void
n_vectorfromworld( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float gx = cmd->In()->GetF();
  float gy = cmd->In()->GetF();
  float gz = cmd->In()->GetF();
  vector3 res;
  self->GetBody()->VectorFromWorld( vector3(gx, gy, gz), &res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  enable

  @input
  b(True/False)

  @output
  v

  @info
  Enable or disable the body. Disabled bodies are effectively "turned off" and 
  are not updated during a simulation step. However, if a disabled body is 
  connected to island containing one or more enabled bodies then it will be 
  re-enabled at the next simulation step.
*/
static
void
n_enable( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  if ( cmd->In()->GetB() )
    self->GetBody()->Enable();
  else
    self->GetBody()->Disable();
}

//------------------------------------------------------------------------------
/**
  @cmd
  isenabled

  @input
  v

  @output
  b(True/False) 

  @info
  Check if the body is enabled.
*/
static
void
n_isenabled( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetB( self->GetBody()->IsEnabled() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setfiniterotationmode

  @input
  i(Mode)

  @output
  v

  @info
  See ODE manual. (dBodySetFiniteRotationMode)
*/
static
void
n_setfiniterotationmode( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->GetBody()->SetFiniteRotationMode( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getfiniterotationmode

  @input
  v

  @output
  i(Mode) 

  @info
  Get the finite rotation mode for the body.
*/
static
void
n_getfiniterotationmode( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetI( self->GetBody()->GetFiniteRotationMode() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setfiniterotationaxis

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  See ODE manual. (dBodySetFiniteRotationAxis)
*/
static
void
n_setfiniterotationaxis( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->GetBody()->SetFiniteRotationAxis( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getfiniterotationaxis

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the finite rotation axis of the body.
*/
static
void
n_getfiniterotationaxis( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  vector3 res;
  self->GetBody()->GetFiniteRotationAxis( &res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getnumjoints

  @input
  v

  @output
  i(Number of Joints)

  @info
  Get the number of joints that are attached to the body.
*/
static
void
n_getnumjoints( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetI( self->GetBody()->GetNumJoints() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  enablegravity

  @input
  b(True/False)

  @output
  v

  @info
  Specify whether the body should be influenced by gravity.
*/
static
void
n_enablegravity( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->GetBody()->EnableGravity( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  isgravityenabled

  @input
  v

  @output
  b(True/False) 

  @info
  Check if the body is influenced by gravity.
*/
static
void
n_isgravityenabled( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetB( self->GetBody()->IsGravityEnabled() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  connectedto

  @input
  s(OdeBodyNode)

  @output
  b(True/False)

  @info
  Check if the bodies are connected by a joint.
*/
static
void
n_connectedto( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  nOdeBodyNode* other = (nOdeBodyNode*)self->kernelServer->Lookup( cmd->In()->GetS() );
  n_assert( other && "Body not found!" );
  cmd->Out()->SetB( self->GetBody()->ConnectedTo( other->GetBody() ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  connectedtoex

  @input
  s(OdeBodyNode), i(JointType)

  @output
  b(True/False)

  @info
  Check if the bodies are connected by a joint that is not of the type JointType.
*/
static
void
n_connectedtoex( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  nOdeBodyNode* other = (nOdeBodyNode*)self->kernelServer->Lookup( cmd->In()->GetS() );
  n_assert( other && "Body not found!" );
  int jointType = cmd->In()->GetI();
  cmd->Out()->SetB( self->GetBody()->ConnectedToEx( other->GetBody(), jointType ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setmasstotal

  @input
  f(TotalMass)

  @output
  v

  @info
  Adjust the total mass of the body to TotalMass. 
  For more info lookup dMassAdjust in the ODE manual.
*/
static
void
n_setmasstotal( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->SetMassTotal( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getmasstotal

  @input
  v

  @output
  f(TotalMass)

  @info
  Get the total mass of the body.
*/
static
void
n_getmasstotal( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetF( self->GetMassTotal() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setmasscog

  @input
  f(cgx), f(cgy), f(cgz)

  @output
  v

  @info
  Set the center of gravity position in the body frame.
  For more info lookup dMass in the ODE manual.
*/
static
void
n_setmasscog( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float cgx = cmd->In()->GetF();
  float cgy = cmd->In()->GetF();
  float cgz = cmd->In()->GetF();
  self->SetMassCOG( vector3(cgx, cgy, cgz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getmasscog

  @input
  v

  @output
  f(cgx), f(cgy), f(cgz)

  @info
  Get the center of gravity position in the body frame.
  For more info lookup dMass in the ODE manual.
*/
static
void
n_getmasscog( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  vector3 cog;
  self->GetMassCOG( &cog );
  cmd->Out()->SetF( cog.x );
  cmd->Out()->SetF( cog.y );
  cmd->Out()->SetF( cog.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setmassinertia

  @input
  f(I11), f(I22), f(I33), f(I12), f(I13), f(I23)

  @output
  v

  @info
  Set the inertia tensor.
  For more info lookup dMass in the ODE manual.
*/
static
void
n_setmassinertia( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  float i11 = cmd->In()->GetF();
  float i22 = cmd->In()->GetF();
  float i33 = cmd->In()->GetF();
  float i12 = cmd->In()->GetF();
  float i13 = cmd->In()->GetF();
  float i23 = cmd->In()->GetF();
  self->SetMassInertia( matrix33(i11, i12, i13, i12, i22, i23, i13, i23, i33) );  
}

//------------------------------------------------------------------------------
/**
  @cmd
  getmassinertia

  @input
  v

  @output
  f(I11), f(I22), f(I33), f(I12), f(I13), f(I23)

  @info
  Get the inertia tensor.
  For more info lookup dMass in the ODE manual.
*/
static
void
n_getmassinertia( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  matrix33 m;
  self->GetMassInertia( &m );
  cmd->Out()->SetF( m.M11 );
  cmd->Out()->SetF( m.M22 );
  cmd->Out()->SetF( m.M33 );
  cmd->Out()->SetF( m.M12 );
  cmd->Out()->SetF( m.M13 );
  cmd->Out()->SetF( m.M23 );
}

//------------------------------------------------------------------------------
/**
  @cmd
  copymass

  @input
  s(OdeMassNode)

  @output
  v

  @info
  Copies the mass parameters from a mass node.
*/
static
void
n_copymass( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->CopyMass( cmd->In()->GetS() );
}

#ifdef ODE_STEPFAST_AUTO_DISABLE
//------------------------------------------------------------------------------
/**
  @cmd
  setautodisablethreshold

  @input
  f(Threshold)

  @output
  v

  @info
  For more info lookup dBodySetAutoDisableThresholdSF1 in the ODE manual.
*/
static
void
n_setautodisablethreshold( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->GetBody()->SetAutoDisableThreshold( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getautodisablethreshold

  @input
  v

  @output
  f(Threshold)

  @info
  For more info lookup dBodyGetAutoDisableThresholdSF1 in the ODE manual.
*/
static
void
n_getautodisablethreshold( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetF( self->GetBody()->GetAutoDisableThreshold() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setautodisablesteps

  @input
  i(NumSteps)

  @output
  v

  @info
  For more info lookup dBodySetAutoDisableStepsSF1 in the ODE manual.
*/
static
void
n_setautodisablesteps( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->GetBody()->SetAutoDisableSteps( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getautodisablesteps

  @input
  v

  @output
  i(NumSteps)

  @info
  For more info lookup dBodyGetAutoDisableStepsSF1 in the ODE manual.
*/
static
void
n_getautodisablesteps( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetI( self->GetBody()->GetAutoDisableSteps() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setautodisable

  @input
  b(True/False)

  @output
  v

  @info
  Enable/disable auto disabling for this body.
  For more info lookup dBodySetAutoDisableSF1 in the ODE manual.
*/
static
void
n_setautodisable( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  self->GetBody()->SetAutoDisable( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getautodisable

  @input
  v

  @output
  b(True/False)

  @info
  Check if auto disabling is enabled for this body.
  For more info lookup dBodyGetAutoDisableSF1 in the ODE manual.
*/
static
void
n_getautodisable( void* slf, nCmd* cmd )
{
  nOdeBodyNode* self = (nOdeBodyNode*)slf;
  cmd->Out()->SetB( self->GetBody()->GetAutoDisable() );
}
#endif // ODE_STEPFAST_AUTO_DISABLE

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeBodyNode::SaveCmds( nPersistServer* ps )
{
  if ( nRoot::SaveCmds( ps ) )
  {
    nCmd* cmd;
    quaternion q;
    vector3 vec;
    nOdeBody* b = this->GetBody();
    
    // initbody
    cmd = ps->GetCmd( this, 'IB__' );
    cmd->In()->SetS( this->ref_PhysContext.getname() );
    ps->PutCmd( cmd );
    
    // setautocleanup
    cmd = ps->GetCmd( this, 'SAC_' );
    cmd->In()->SetB( this->GetAutoCleanup() );
    ps->PutCmd( cmd );

    // handle mass
    nOdeMass mass;
    matrix33 inertia;
    float totalMass;
    b->GetMass( &mass );
    mass.GetParameters( &totalMass, &vec, &inertia );

    // setmasstotal
    cmd = ps->GetCmd( this, 'SMT_' );
    cmd->In()->SetF( totalMass );
    ps->PutCmd( cmd );
    
    // setmasscog
    cmd = ps->GetCmd( this, 'SMCG' );
    cmd->In()->SetF( vec.x );
    cmd->In()->SetF( vec.y );
    cmd->In()->SetF( vec.z );
    ps->PutCmd( cmd );
    
    // setmassinertia
    cmd = ps->GetCmd( this, 'SMI_' );
    cmd->In()->SetF( inertia.M11 );
    cmd->In()->SetF( inertia.M22 );
    cmd->In()->SetF( inertia.M33 );
    cmd->In()->SetF( inertia.M12 );
    cmd->In()->SetF( inertia.M13 );
    cmd->In()->SetF( inertia.M23 );
    ps->PutCmd( cmd );
    
    // setrotation
    cmd = ps->GetCmd( this, 'SRQ_' );
    b->GetRotation( &q );
    cmd->In()->SetF( q.x );
    cmd->In()->SetF( q.y );
    cmd->In()->SetF( q.z );
    cmd->In()->SetF( q.w );
    ps->PutCmd( cmd );
    
    // setposition
    cmd = ps->GetCmd( this, 'SP__' );
    b->GetPosition( &vec );
    cmd->In()->SetF( vec.x );
    cmd->In()->SetF( vec.y );
    cmd->In()->SetF( vec.z );
    ps->PutCmd( cmd );
    
    // setlinearvel
    cmd = ps->GetCmd( this, 'SLV_' );
    b->GetLinearVel( &vec );
    cmd->In()->SetF( vec.x );
    cmd->In()->SetF( vec.y );
    cmd->In()->SetF( vec.z );
    ps->PutCmd( cmd );
    
    // setangularvel
    cmd = ps->GetCmd( this, 'SAV_' );
    b->GetAngularVel( &vec );
    cmd->In()->SetF( vec.x );
    cmd->In()->SetF( vec.y );
    cmd->In()->SetF( vec.z );
    ps->PutCmd( cmd );
    
    // setforce
    cmd = ps->GetCmd( this, 'SF__' );
    b->GetForce( &vec );
    cmd->In()->SetF( vec.x );
    cmd->In()->SetF( vec.y );
    cmd->In()->SetF( vec.z );
    ps->PutCmd( cmd );
    
    // settorque
    cmd = ps->GetCmd( this, 'ST__' );
    b->GetTorque( &vec );
    cmd->In()->SetF( vec.x );
    cmd->In()->SetF( vec.y );
    cmd->In()->SetF( vec.z );
    ps->PutCmd( cmd );
    
    // enable
    cmd = ps->GetCmd( this, 'E___' );
    cmd->In()->SetB( b->IsEnabled() );
    ps->PutCmd( cmd );

    // setfiniterotationmode
    cmd = ps->GetCmd( this, 'SFRM' );
    cmd->In()->SetI( b->GetFiniteRotationMode() );
    ps->PutCmd( cmd );
    
    // setfiniterotationaxis
    cmd = ps->GetCmd( this, 'SFRA' );
    b->GetFiniteRotationAxis( &vec );
    cmd->In()->SetF( vec.x );
    cmd->In()->SetF( vec.y );
    cmd->In()->SetF( vec.z );
    ps->PutCmd( cmd );
    
    // enablegravity
    cmd = ps->GetCmd( this, 'EG__' );
    cmd->In()->SetB( b->IsGravityEnabled() );
    ps->PutCmd( cmd );
    
#ifdef ODE_STEPFAST_AUTO_DISABLE
    
    // setautodisablethreshold
    cmd = ps->GetCmd( this, 'SADT' );
    cmd->In()->SetF( b->GetAutoDisableThreshold() );
    ps->PutCmd( cmd );
    
    // setautodisablesteps
    cmd = ps->GetCmd( this, 'SADS' );
    cmd->In()->SetI( b->GetAutoDisableSteps() );
    ps->PutCmd( cmd );
    
    // setautodisable
    cmd = ps->GetCmd( this, 'SAD_' );
    cmd->In()->SetB( b->GetAutoDisable() );
    ps->PutCmd( cmd );
    
#endif // ODE_STEPFAST_AUTO_DISABLE

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
