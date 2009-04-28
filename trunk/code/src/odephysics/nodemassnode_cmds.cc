#define N_IMPLEMENTS nOdeMassNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeMassNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodemassnode.h"
#include "kernel/npersistserver.h"

static void n_reset( void* slf, nCmd* cmd );
static void n_setparameters( void* slf, nCmd* cmd );
static void n_getparameters( void* slf, nCmd* cmd );
static void n_gettotalmass( void* slf, nCmd* cmd );
static void n_adjustto( void* slf, nCmd* cmd );
static void n_setcog( void* slf, nCmd* cmd );
static void n_getcog( void* slf, nCmd* cmd );
static void n_setinertia( void* slf, nCmd* cmd );
static void n_getinertia( void* slf, nCmd* cmd );
static void n_setsphere( void* slf, nCmd* cmd );
static void n_setspheretotal( void* slf, nCmd* cmd );
static void n_setcapsule( void* slf, nCmd* cmd );
static void n_setcapsuletotal( void* slf, nCmd* cmd );
static void n_setcylinder( void* slf, nCmd* cmd );
static void n_setcylindertotal( void* slf, nCmd* cmd );
static void n_setbox( void* slf, nCmd* cmd );
static void n_setboxtotal( void* slf, nCmd* cmd );
static void n_translate( void* slf, nCmd* cmd );
static void n_rotate( void* slf, nCmd* cmd );
static void n_add( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
  @scriptclass
  nodemassnode

  @superclass
  nroot

  @classinfo
  Provides script access to ODE mass objects.
*/
void
n_initcmds( nClass* clazz )
{
  clazz->BeginCmds();
  clazz->AddCmd( "v_reset_v", 'RST_', n_reset );
  clazz->AddCmd( "v_setparameters_ffffffffff", 'SP__', n_setparameters );
  clazz->AddCmd( "ffffffffff_getparameters_v", 'GP__', n_getparameters );
  clazz->AddCmd( "f_gettotalmass_v", 'GTM_', n_gettotalmass );
  clazz->AddCmd( "v_adjustto_f", 'AT__', n_adjustto );
  clazz->AddCmd( "v_setcog_fff", 'SCOG', n_setcog );
  clazz->AddCmd( "fff_getcog_v", 'GCOG', n_getcog );
  clazz->AddCmd( "v_setinertia_ffffff", 'SI__', n_setinertia );
  clazz->AddCmd( "ffffff_getinertia_v", 'GI__', n_getinertia );
  clazz->AddCmd( "v_setsphere_ff", 'SS__', n_setsphere );
  clazz->AddCmd( "v_setspheretotal_ff", 'SST_', n_setspheretotal );
  clazz->AddCmd( "v_setcapsule_fffi", 'SCP_', n_setcapsule );
  clazz->AddCmd( "v_setcapsuletotal_fffi", 'SCPT', n_setcapsuletotal );
  clazz->AddCmd( "v_setcylinder_fffi", 'SCY_', n_setcylinder );
  clazz->AddCmd( "v_setcylindertotal_fffi", 'SCYT', n_setcylindertotal );
  clazz->AddCmd( "v_setbox_ffff", 'SB__', n_setbox );
  clazz->AddCmd( "v_setboxtotal_ffff", 'SBT_', n_setboxtotal );
  clazz->AddCmd( "v_translate_fff", 'T___', n_translate );
  clazz->AddCmd( "v_rotate_ffff", 'RQ__', n_rotate );
  clazz->AddCmd( "v_add_s", 'A___', n_add );
  clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
  @cmd
  reset

  @input
  v

  @output
  v

  @info
  Set all the mass parameters to zero.
*/
static
void
n_reset( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  self->GetMass().Reset();
}

//------------------------------------------------------------------------------
/**
  @cmd
  setparameters

  @input
  f(TheMass), 
  f(cgx), f(cgy), f(cgz),
  f(I11), f(I22), f(I33), 
  f(I12), f(I13), f(I23)

  @output
  v

  @info
  Set the mass parameters. 
  For more info lookup dMassSetParameters in the ODE manual.
*/
static
void
n_setparameters( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float theMass = cmd->In()->GetF();
  vector3 cog;
  cog.x = cmd->In()->GetF();
  cog.y = cmd->In()->GetF();
  cog.z = cmd->In()->GetF();
  float i11 = cmd->In()->GetF();
  float i22 = cmd->In()->GetF();
  float i33 = cmd->In()->GetF();
  float i12 = cmd->In()->GetF();
  float i13 = cmd->In()->GetF();
  float i23 = cmd->In()->GetF();
  matrix33 inertia(i11, i12, i13, i12, i22, i23, i13, i23, i33);
  self->GetMass().SetParameters( theMass, cog, inertia );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getparameters

  @input
  v

  @output
  f(TheMass), 
  f(cgx), f(cgy), f(cgz),
  f(I11), f(I22), f(I33), 
  f(I12), f(I13), f(I23)

  @info
  Get the mass parameters.
*/
static
void
n_getparameters( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float theMass;
  vector3 cog;
  matrix33 m;
  self->GetMass().GetParameters( &theMass, &cog, &m );
  cmd->Out()->SetF( theMass );
  cmd->Out()->SetF( cog.x );
  cmd->Out()->SetF( cog.y );
  cmd->Out()->SetF( cog.z );
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
  gettotalmass

  @input
  v

  @output
  f(TotalMass)

  @info
  Get the total mass.
*/
static
void
n_gettotalmass( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  cmd->Out()->SetF( self->GetMass().GetTotalMass() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  adjustto

  @input
  f(TotalMass)

  @output
  v

  @info
  Adjust the total mass to TotalMass.
  For more info lookup dMassAdjust in the ODE manual.
*/
static
void
n_adjustto( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  self->GetMass().AdjustTo( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setcog

  @input
  f(cgX), f(cgY), f(cgZ)

  @output
  v

  @info
  Set the center of gravity.
  For more info lookup dMass in the ODE manual.
*/
static
void
n_setcog( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->GetMass().SetCenterOfGravity( v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getcog

  @input
  v

  @output
  f(cgX), f(cgY), f(cgZ)

  @info
  Get the current center of gravity.
  For more info lookup dMass in the ODE manual.
*/
static
void
n_getcog( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  vector3 v;
  self->GetMass().GetCenterOfGravity( &v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setinertia

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
n_setinertia( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float i11 = cmd->In()->GetF();
  float i22 = cmd->In()->GetF();
  float i33 = cmd->In()->GetF();
  float i12 = cmd->In()->GetF();
  float i13 = cmd->In()->GetF();
  float i23 = cmd->In()->GetF();
  self->GetMass().SetInertia( matrix33(i11, i12, i13, i12, i22, i23, i13, i23, i33) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  getinertia

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
n_getinertia( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  matrix33 m;
  self->GetMass().GetInertia( &m );
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
  setsphere

  @input
  f(density), f(radius)

  @output
  v

  @info
  Set the mass parameters to represent a sphere.
  For more info lookup dMassSetSphere in the ODE manual.
*/
static
void
n_setsphere( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float density = cmd->In()->GetF();
  float radius = cmd->In()->GetF();
  self->GetMass().SetSphere( density, radius );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setspheretotal

  @input
  f(TotalMass), f(radius)

  @output
  v

  @info
  Set the mass parameters to represent a sphere.
  For more info lookup dMassSetSphereTotal in the ODE manual.
*/
static
void
n_setspheretotal( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float totalmass = cmd->In()->GetF();
  float radius = cmd->In()->GetF();
  self->GetMass().SetSphereTotal( totalmass, radius );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setcapsule

  @input
  f(density), f(radius), f(length), i(direction)

  @output
  v

  @info
  Set the mass parameters to represent a capsule of the given parameters and 
  density. The capsule's long axis is oriented along the body's x, y or z axis 
  according to the value of 'direction' (1 = x, 2 = y, 3 = z), you can set
  'direction' to zero to indicate that the default axis should be used (z-axis). 
  For more info lookup dMassSetCappedCylinder in the ODE manual.
*/
static
void
n_setcapsule( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float density = cmd->In()->GetF();
  float radius = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  int direction = cmd->In()->GetI();
  if ( 0 == direction )
    direction = 3;
  self->GetMass().SetCapsule( density, radius, length, direction );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setcapsuletotal

  @input
  f(TotalMass), f(radius), f(length), i(direction)

  @output
  v

  @info
  The capsule's long axis is oriented along the body's x, y or z axis 
  according to the value of 'direction' (1 = x, 2 = y, 3 = z), you can set
  'direction' to zero to indicate that the default axis should be used (z-axis). 
  For more info lookup dMassSetCappedCylinderTotal in the ODE manual.
*/
static
void
n_setcapsuletotal( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float totalMass = cmd->In()->GetF();
  float radius = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  int direction = cmd->In()->GetI();
  if ( 0 == direction )
    direction = 3;
  self->GetMass().SetCapsuleTotal( totalMass, radius, length, direction );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setcylinder

  @input
  f(density), f(radius), f(length), i(direction)

  @output
  v

  @info
  Set the mass parameters to represent a cylinder of the given parameters and 
  density. The cylinder's long axis is oriented along the body's x, y or z axis 
  according to the value of 'direction' (1 = x, 2 = y, 3 = z), you can set
  'direction' to zero to indicate that the default axis should be used (z-axis). 
  For more info lookup dMassSetCylinder in the ODE manual.
*/
static
void
n_setcylinder( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float density = cmd->In()->GetF();
  float radius = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  int direction = cmd->In()->GetI();
  if ( 0 == direction )
    direction = 3;
  self->GetMass().SetCapsule( density, radius, length, direction );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setcylindertotal

  @input
  f(TotalMass), f(radius), f(length), i(direction)

  @output
  v

  @info
  The cylinder's long axis is oriented along the body's x, y or z axis 
  according to the value of 'direction' (1 = x, 2 = y, 3 = z), you can set
  'direction' to zero to indicate that the default axis should be used (z-axis). 
  For more info lookup dMassSetCylinderTotal in the ODE manual.
*/
static
void
n_setcylindertotal( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float totalMass = cmd->In()->GetF();
  float radius = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  int direction = cmd->In()->GetI();
  if ( 0 == direction )
    direction = 3;
  self->GetMass().SetCylinderTotal( totalMass, radius, length, direction );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setbox

  @input
  f(density), f(lx), f(ly), f(lz)

  @output
  v

  @info
  Set the mass parameters to represent a box of the given dimensions and density.
  For more info lookup dMassSetBox in the ODE manual.
*/
static
void
n_setbox( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float density = cmd->In()->GetF();
  float lx = cmd->In()->GetF();
  float ly = cmd->In()->GetF();
  float lz = cmd->In()->GetF();
  self->GetMass().SetBox( density, lx, ly, lz );
}

//------------------------------------------------------------------------------
/**
  @cmd
  setboxtotal

  @input
  f(TotalMass), f(lx), f(ly), f(lz)

  @output
  v

  @info
  For more info lookup dMassSetBoxTotal in the ODE manual.
*/
static
void
n_setboxtotal( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  float totalMass = cmd->In()->GetF();
  float lx = cmd->In()->GetF();
  float ly = cmd->In()->GetF();
  float lz = cmd->In()->GetF();
  self->GetMass().SetBoxTotal( totalMass, lx, ly, lz );
}

//------------------------------------------------------------------------------
/**
  @cmd
  translate

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Adjust the mass parameters to represent the object displaced by (x, y, z) 
  relative to the body frame.
  For more info lookup dMassTranslate in the ODE manual.
*/
static
void
n_translate( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->GetMass().Translate( v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  rotate

  @input
  f(q.x), f(q.y), f(q.z), f(q.w)

  @output
  v

  @info
  Adjust the mass parameters to represent the object rotated by quaternion q 
  relative to the body frame.
  For more info lookup dMassRotate in the ODE manual.
*/
static
void
n_rotate( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  quaternion q;
  q.x = cmd->In()->GetF();
  q.y = cmd->In()->GetF();
  q.z = cmd->In()->GetF();
  q.w = cmd->In()->GetF();
  self->GetMass().Rotate( q );
}

//------------------------------------------------------------------------------
/**
  @cmd
  add

  @input
  s(OtherMassNode)

  @output
  v

  @info
  Add another mass to this mass.
  For more info lookup dMassAdd in the ODE manual.
*/
static
void
n_add( void* slf, nCmd* cmd )
{
  nOdeMassNode* self = (nOdeMassNode*)slf;
  nOdeMassNode* other = (nOdeMassNode*)self->kernelServer->Lookup( cmd->In()->GetS() );
  self->GetMass() += other->GetMass();
}

//------------------------------------------------------------------------------
/**
  @param  ps  Writes the nCmd object contents out to a file.
  @return     Success or failure.
*/
bool
nOdeMassNode::SaveCmds( nPersistServer* ps )
{
  if ( nRoot::SaveCmds( ps ) )
  {
    matrix33 inertia;
    float totalMass;
    vector3 vec;
    this->GetMass().GetParameters( &totalMass, &vec, &inertia );
    
    nCmd* cmd = ps->GetCmd( this, 'SP__' );
    cmd->In()->SetF( totalMass );
    cmd->In()->SetF( vec.x );
    cmd->In()->SetF( vec.y );
    cmd->In()->SetF( vec.z );
    cmd->In()->SetF( inertia.M11 );
    cmd->In()->SetF( inertia.M22 );
    cmd->In()->SetF( inertia.M33 );
    cmd->In()->SetF( inertia.M12 );
    cmd->In()->SetF( inertia.M13 );
    cmd->In()->SetF( inertia.M23 );
    ps->PutCmd( cmd );

    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
