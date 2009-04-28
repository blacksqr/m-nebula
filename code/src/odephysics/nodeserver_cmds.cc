#define N_IMPLEMENTS nOdeServer
//--------------------------------------------------------------------
//  nodeserver_cmds.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeServer is licensed under the terms of the Nebula License.
//--------------------------------------------------------------------
#include "odephysics/nodeserver.h"
#include "kernel/npersistserver.h"
#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodecollidecontext.h"

static void n_begincollclasses(void *, nCmd *);
static void n_addcollclass(void *, nCmd *);
static void n_endcollclasses(void *, nCmd *);
static void n_begincolltypes(void *, nCmd *);
static void n_addcolltype(void *, nCmd *);
static void n_endcolltypes(void *, nCmd *);
static void n_querycolltype(void *, nCmd *);
static void n_beginsurfaces(void *, nCmd *);
static void n_addsurface(void *, nCmd *);
static void n_enablesurfparam(void *, nCmd *);
static void n_setsurfparam(void *, nCmd *);
static void n_endsurfaces(void *, nCmd *);
static void n_beginmaterials(void *, nCmd *);
static void n_addmaterial(void *, nCmd *);
static void n_endmaterials(void *, nCmd *);
static void n_beginsurfrelations(void *, nCmd *);
static void n_setsurfrelation3s(void *, nCmd *);
static void n_setsurfrelation3i(void *, nCmd *);
static void n_endsurfrelations(void *, nCmd *);
static void n_getsurfaceindex(void *, nCmd *);
static void n_getmaterialindex(void *, nCmd *);
static void n_setphysicscontext(void *, nCmd *);
static void n_getphysicscontext(void *, nCmd *);
static void n_setcollidecontext(void *, nCmd *);
static void n_getcollidecontext(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nodeserver
    
    @superclass
    nroot

    @classinfo
    Provides facilities for collision detection and physics. 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_begincollclasses_v",    'BGCC', n_begincollclasses );
    clazz->AddCmd( "v_addcollclass_s",        'ADCC', n_addcollclass );
    clazz->AddCmd( "v_endcollclasses_v",      'ENCC', n_endcollclasses );
    clazz->AddCmd( "v_begincolltypes_v",      'BGCT', n_begincolltypes );
    clazz->AddCmd( "v_addcolltype_sss",       'ADCT', n_addcolltype );
    clazz->AddCmd( "v_endcolltypes_v",        'ENCT', n_endcolltypes );
    clazz->AddCmd( "s_querycolltype_ss",      'QRCT', n_querycolltype );
    clazz->AddCmd( "v_beginsurfaces_i",       'BSUR', n_beginsurfaces );
    clazz->AddCmd( "v_addsurface_is",         'ADSF', n_addsurface );
    clazz->AddCmd( "v_enablesurfparam_is",    'ESFP', n_enablesurfparam );
    clazz->AddCmd( "v_setsurfparam_isf",      'SSFP', n_setsurfparam );
    clazz->AddCmd( "v_endsurfaces_v",         'ESUR', n_endsurfaces );
    clazz->AddCmd( "v_beginmaterials_v",      'BMAT', n_beginmaterials );
    clazz->AddCmd( "v_addmaterial_s",         'AMAT', n_addmaterial );
    clazz->AddCmd( "v_endmaterials_v",        'EMAT', n_endmaterials );
    clazz->AddCmd( "v_beginsurfrelations_v",  'BSRL', n_beginsurfrelations );
    clazz->AddCmd( "v_setsurfrelation3s_sss", 'SRL1', n_setsurfrelation3s );
    clazz->AddCmd( "v_setsurfrelation3i_iii", 'SRL2', n_setsurfrelation3i );
    clazz->AddCmd( "v_beginsurfrelations_v",  'ESRL', n_endsurfrelations );
    clazz->AddCmd( "i_getsurfaceindex_s",     'GSFI', n_getsurfaceindex );
    clazz->AddCmd( "i_getmaterialindex_s",    'GMTI', n_getmaterialindex );
    clazz->AddCmd( "v_setphysicscontext_s",   'SDPC', n_setphysicscontext );
    clazz->AddCmd( "s_getphysicscontext_v",   'GDPC', n_getphysicscontext );
    clazz->AddCmd( "v_setcollidecontext_s",   'SDCC', n_setcollidecontext );
    clazz->AddCmd( "s_getcollidecontext_v",   'GDCC', n_getcollidecontext );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    begincollclasses

    @input
    v

    @output
    v

    @info
    Start defining collision classes. A collision class is a
    user defined abstract type name which is later used to
    define inter-class collision types (ignore, quick, or exact).
*/
static void n_begincollclasses(void* o, nCmd* )
{
  nOdeServer* self = (nOdeServer*)o;
  self->BeginCollClasses();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addcollclass

    @input
    s (CollClassName)

    @output
    v

    @info
    Add a new collision class name. Collision class names must
    be unique.
*/
static void n_addcollclass( void* o, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)o;
  self->AddCollClass( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    endcollclasses

    @input
    v

    @output
    s (CollClassName)

    @info
    End adding collision class names. After collision class names
    are defined, the collision type table should be built
    by doing a 'begincolltypes/addcolltype/endcolltypes' sequence.
*/
static void n_endcollclasses(void* o, nCmd* )
{
  nOdeServer* self = (nOdeServer*)o;
  self->EndCollClasses();
}

//------------------------------------------------------------------------------
/**
    @cmd
    begincolltypes

    @input
    v

    @output
    v

    @info
    Begin definition of collision types. A collision type defines
    how 2 collision classes should check collision (ignore, quick
    or exact).
*/
static void n_begincolltypes( void* o, nCmd* )
{
  nOdeServer* self = (nOdeServer*)o;
  self->BeginCollTypes();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addcolltype

    @input
    s (CollClass1), s (CollClass2), s (CollType=ignore|quick|contact|exact)

    @output
    v

    @info
    Define a collision type, this is how 2 objects of a given
    collision class should perform their collision test:
    ignore  -> the 2 objects will ignore any collision
    quick   -> the 2 objects will do a quick spherical check
    contact -> the 2 objects report the first contact
    exact   -> the 2 objects should do a triangle exact check

    The default is 'exact'.
*/
static void n_addcolltype( void* o, nCmd* cmd )
{
  nOdeServer *self = (nOdeServer *) o;
  const char *s0 = cmd->In()->GetS();
  const char *s1 = cmd->In()->GetS();
  const char *s2 = cmd->In()->GetS();
  nOdeCollType ct = ODE_COLLTYPE_EXACT;
  if (strcmp(s2, "ignore")       == 0) ct = ODE_COLLTYPE_IGNORE;
  else if (strcmp(s2, "quick")   == 0) ct = ODE_COLLTYPE_QUICK;
  else if (strcmp(s2, "exact")   == 0) ct = ODE_COLLTYPE_EXACT;
  else if (strcmp(s2, "contact") == 0) ct = ODE_COLLTYPE_CONTACT;
  else 
  {
    n_printf( "Unknown collision type '%s', valid is 'ignore|quick|contact|exact'!\n"
             , s2 );
  }
  self->AddCollType( s0, s1, ct );
}

//------------------------------------------------------------------------------
/**
    @cmd
    endcolltypes

    @input
    v

    @output
    v

    @info
    End defining collision types.
*/
static void n_endcolltypes( void* o, nCmd* )
{
  nOdeServer* self = (nOdeServer*)o;
  self->EndCollTypes();
}

//------------------------------------------------------------------------------
/**
    @cmd
    querycolltype

    @input
    s (CollClass1), s (CollClass2)

    @output
    s (CollType)

    @info
    Query the collision type table for the collision type
    that has been defined for the 2 given collision classes.
*/
static void n_querycolltype( void* o, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)o;
  const char* s1 = cmd->In()->GetS();
  const char* s2 = cmd->In()->GetS();
  nOdeCollType ct = self->QueryCollType( s1, s2 );
  switch ( ct ) 
  {
    case ODE_COLLTYPE_IGNORE:   cmd->Out()->SetS("ignore"); break;
    case ODE_COLLTYPE_QUICK:    cmd->Out()->SetS("quick"); break;
    case ODE_COLLTYPE_CONTACT:  cmd->Out()->SetS("contact"); break;
    case ODE_COLLTYPE_EXACT:    cmd->Out()->SetS("exact"); break;
  }
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginsurfaces

    @input
    i ( total number of surfaces )

    @output
    v

    @info
    Begin defining surfaces.
*/
void n_beginsurfaces( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  self->BeginSurfaces( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    addsurface

    @input
    i ( surface index ), s ( surface name )

    @output
    v

    @info
    Add a new surface definition.
*/
void n_addsurface( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  int i = cmd->In()->GetI();
  const char* name = cmd->In()->GetS();
  self->AddSurface( i, name );
}

//------------------------------------------------------------------------------
/**
    @cmd
    enablesurfparam

    @input
    i ( surface index ), s ( parameter name )

    @output
    v

    @info
    Enable a surface parameter. Here are the available parameters,
    see the ODE user manual for additional info.
    
    fdir1         - corresponds to dContactFDir1
    approxfdir1   - corresponds to dContactApprox1_1
    approxfdir2   - corresponds to dContactApprox1_2
    approxfboth   - corresponds to dContactApprox1
*/
void n_enablesurfparam( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  int i = cmd->In()->GetI();
  const char* param = cmd->In()->GetS();
  self->EnableSurfaceParam( i, param );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsurfparam

    @input
    i ( surface index ), s ( parameter name ), f ( parameter value )

    @output
    v

    @info
    Enable and set a surface parameter. Here are the available parameters
    and possible values, see the ODE user manual for additional info.
    
    mu        - Coulomb friction coefficient for friction direction 1
                (corresponds to dSurfaceParameters::mu).
                Value must be in range 0 to infinity, specify -1 to indicate
                infinity.
    mu2       - Coulomb friction coefficient for friction direction 2
                (corresponds to dSurfaceParameters::mu2).
                Value must be in range 0 to infinity, specify -1 to indicate
                infinity.
    cfm       - Corresponds to dSurfaceParameters::soft_cfm.
    erp       - Corresponds to dSurfaceParameters::soft_erp.
    bounce    - Restitution. Value must be in range 0 to 1, 0 means materials
                are not bouncy, 1 means materials are very bouncy
                (corresponds to dSurfaceParameters::bounce).
    bvel      - Minimum velocity necessary for bounce (corresponds to
                dSurfaceParameters::bounce_vel).
    motion1   - Surface velocity in friction direction 1 (corresponds to
                dSurfaceParameters::motion1).
    motion2   - Surface velocity in friction direction 2 (corresponds to
                dSurfaceParameters::motion2).
    slip1     - Coefficient of force dependent slip for friction direction 1
                (corresponds to dSurfaceParameters::slip1)
    slip2     - Coefficient of force dependent slip for friction direction 2
                (corresponds to dSurfaceParameters::slip2)
*/
void n_setsurfparam( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  int i = cmd->In()->GetI();
  const char* param = cmd->In()->GetS();
  float value = cmd->In()->GetF();
  self->SetSurfaceParam( i, param, value );
}

//------------------------------------------------------------------------------
/**
    @cmd
    endsurfaces

    @input
    v

    @output
    v

    @info
    Finish defining surfaces.
*/
void n_endsurfaces( void* slf, nCmd* )
{
  nOdeServer* self = (nOdeServer*)slf;
  self->EndSurfaces();
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginmaterials

    @input
    v

    @output
    v

    @info
    Begin defining materials.
*/
void n_beginmaterials( void* slf, nCmd* )
{
  nOdeServer* self = (nOdeServer*)slf;
  self->BeginMaterials();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addmaterial

    @input
    s ( material name )

    @output
    v

    @info
    Add a new material definition.
*/
void n_addmaterial( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  self->AddMaterial( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    endmaterials

    @input
    v

    @output
    v

    @info
    Finish defining materials.
*/
void n_endmaterials( void* slf, nCmd* )
{
  nOdeServer* self = (nOdeServer*)slf;
  self->EndMaterials();
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginsurfrelations

    @input
    v

    @output
    v

    @info
    Begin assigning surfaces to material pairs.
*/
void n_beginsurfrelations( void* slf, nCmd* )
{
  nOdeServer* self = (nOdeServer*)slf;
  self->BeginSurfaceRelations();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsurfrelation3s

    @input
    s ( material name 1 ), s ( material name 2 ), s ( surface name )

    @output
    v

    @info
    Associate a surface to a pair of materials.
*/
void n_setsurfrelation3s( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  int m1 = self->GetMaterialIndex( cmd->In()->GetS() );
  int m2 = self->GetMaterialIndex( cmd->In()->GetS() );
  int surf = self->GetSurfaceIndex( cmd->In()->GetS() );
  self->SetSurfaceRelation( m1, m2, surf );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsurfrelation3i

    @input
    i ( material index 1 ), i ( material index 2 ), i ( surface index )

    @output
    v

    @info
    Associate a surface to a pair of materials.
*/
void n_setsurfrelation3i( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  int m1 = cmd->In()->GetI();
  int m2 = cmd->In()->GetI();
  int surf = cmd->In()->GetI();
  self->SetSurfaceRelation( m1, m2, surf );
}

//------------------------------------------------------------------------------
/**
    @cmd
    endsurfrelations

    @input
    v

    @output
    v

    @info
    Finish associating surfaces with materials.
*/
void n_endsurfrelations( void* slf, nCmd* )
{
  nOdeServer* self = (nOdeServer*)slf;
  self->EndSurfaceRelations();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsurfaceindex

    @input
    s ( surface name )

    @output
    i ( surface index )

    @info
    Obtain the index of the specified surface.
*/
void n_getsurfaceindex( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  cmd->Out()->SetI( self->GetSurfaceIndex( cmd->In()->GetS() ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmaterialindex

    @input
    s ( material name )

    @output
    i ( material index )

    @info
    Obtain the index of the specified material.
*/
void n_getmaterialindex( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  cmd->Out()->SetI( self->GetMaterialIndex( cmd->In()->GetS() ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setphysicscontext

    @input
    s(OdePhysicsContext)

    @output
    v

    @info
    Set the default physics context for the ODE server.
*/
void n_setphysicscontext( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  nRoot* node = self->kernelServer->Lookup( cmd->In()->GetS() );
  n_assert( node->IsA( self->kernelServer->FindClass( "nodephysicscontext" ) ) );
  self->SetDefaultPhysicsContext( (nOdePhysicsContext*)node );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getphysicscontext

    @input
    v

    @output
    s(OdePhysicsContext)

    @info
    Get the default physics context for the ODE server.
*/
void n_getphysicscontext( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;  
  cmd->Out()->SetS( self->GetDefaultPhysicsContext()->GetFullName().c_str() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcollidecontext

    @input
    s(OdeCollideContext)

    @output
    v

    @info
    Set the default collide context for the ODE server.
*/
void n_setcollidecontext( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;
  nRoot* node = self->kernelServer->Lookup( cmd->In()->GetS() );
  n_assert( node->IsA( self->kernelServer->FindClass( "nodecollidecontext" ) ) );
  self->SetDefaultCollideContext( (nOdeCollideContext*)node );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcollidecontext

    @input
    v

    @output
    s(OdeCollideContext)

    @info
    Get the default collide context for the ODE server.
*/
void n_getcollidecontext( void* slf, nCmd* cmd )
{
  nOdeServer* self = (nOdeServer*)slf;  
  cmd->Out()->SetS( self->GetDefaultPhysicsContext()->GetFullName().c_str());
}
