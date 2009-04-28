#define N_IMPLEMENTS nOdeServer
//------------------------------------------------------------------------------
//  nodeserver_main.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeServer is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "odephysics/nodeserver.h"
nNebulaScriptClass(nOdeServer, "nroot");

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#include "odephysics/nodetrimeshshape.h"
#include "odephysics/nodetrimesh.h"

#ifndef N_ODECOLLIDECONTEXT_H
#include "odephysics/nodecollidecontext.h"
#endif

#include "odephysics/nodesphereshape.h"
#include "odephysics/nodeboxshape.h"
#include "odephysics/nodecapsuleshape.h"
#include "odephysics/nodeplaneshape.h"

//#ifndef N_ODEPHYSICSCONTEXT_H
//#include "odephysics/nodephysicscontext.h"
//#endif

//#include <limits>

nOdeServer* nOdeServer::odeServer = NULL;

//------------------------------------------------------------------------------
/**
*/
nOdeServer::nOdeServer()
  : uniqueId( 0 ), 
    defaultCollideContext( NULL ), 
    numCollClasses( 0 ),
    collTypeTable( NULL ), 
    inBeginCollClasses( false ), 
    inBeginCollTypes( false ),
    ref_PhysicsContext( kernelServer, this ), 
    ref_CollideContext( kernelServer, this ),
    surfaceArray( 0 ), numSurfaces( 0 ),
    surfaceRelationArray( 0 ), materialCount( 0 ), inBeginSurfaces( false ),
    inBeginSurfaceRelations( false ), inBeginMaterials( false ),
    ref_FileServer( kernelServer, this )
{
  nOdeServer::odeServer = this;
  this->ref_FileServer = "/sys/servers/file2";
  
  this->surfaceRelationArray = new int[this->materialCount * this->materialCount];
  
  nOdeCollideShape::rayId = dCreateRay( 0, 1 );
}

//------------------------------------------------------------------------------
/**
*/
nOdeServer::~nOdeServer()
{
  // release the materials
  nStrNode* material;
  while ( (material = this->materialList.RemHead()) )
    delete material;

  // release collision type definitions
  nOdeCollClassNode *ccn;
  while ( (ccn = (nOdeCollClassNode*)this->collClassList.RemHead()) ) 
    delete ccn;
  
  if ( this->collTypeTable ) 
    delete[] this->collTypeTable;

  if ( this->surfaceArray )
    delete[] this->surfaceArray;

  // release any meshes and contexts that may still be around...
  
  nOdeTriMesh* mesh;
  while ( (mesh = (nOdeTriMesh*)this->meshList.GetHead()) ) 
  {
    mesh->RemRef();
    if ( 0 == mesh->GetRef() )
    {
      mesh->Remove();
      delete mesh;
    }
  }
      
  dGeomDestroy( nOdeCollideShape::rayId );
  nOdeCollideShape::rayId = 0;
      
  dCloseODE();
  
  this->ref_FileServer.invalidate();
  nOdeServer::odeServer = NULL;
}

//------------------------------------------------------------------------------
void nOdeServer::BeginSurfaces( int num )
{
  n_assert( !this->inBeginSurfaces );
  n_assert( num > 0 );
  this->inBeginSurfaces = true;
  //this->surfaceArray.Reallocate( num, 0 );
  this->surfaceArray = new nOdeSurface[num];
  this->numSurfaces = num;
}

//------------------------------------------------------------------------------
/**
  @brief Define a new surface.
*/
void nOdeServer::AddSurface( int index, const char* name )
{
  n_assert( this->inBeginSurfaces );
  n_assert( index < this->numSurfaces );
  this->surfaceArray[index].SetName( name );
}

//------------------------------------------------------------------------------
void nOdeServer::EndSurfaces()
{
  n_assert( this->inBeginSurfaces );
}

//------------------------------------------------------------------------------
void nOdeServer::BeginMaterials()
{
  n_assert( !this->inBeginMaterials );
  this->inBeginMaterials = true;
}

//------------------------------------------------------------------------------
/**
  @brief Define a material.
*/
void nOdeServer::AddMaterial( const char* material )
{
  n_assert( this->inBeginMaterials );
  this->materialList.AddTail( new nStrNode( material ) );
}

//------------------------------------------------------------------------------
void nOdeServer::EndMaterials()
{
  n_assert( this->inBeginMaterials );
}

//------------------------------------------------------------------------------
void nOdeServer::BeginSurfaceRelations()
{
  n_assert( !this->inBeginSurfaceRelations );
  this->inBeginSurfaceRelations = true;
}

//------------------------------------------------------------------------------
/**
  @brief Associate a surface with a material pair.
*/
void nOdeServer::SetSurfaceRelation( int m1, int m2, int surfaceIndex )
{
  n_assert( this->inBeginSurfaceRelations );
  n_assert( (m1 < this->materialCount) && (m2 < this->materialCount) );
  this->surfaceRelationArray[m1 * materialCount + m2] = surfaceIndex;
}

//------------------------------------------------------------------------------
void nOdeServer::EndSurfaceRelations()
{
  n_assert( this->inBeginSurfaceRelations );
}

//------------------------------------------------------------------------------
/**
  @brief Get the surface associated with the material pair.
*/
nOdeSurface* nOdeServer::GetSurface( int m1, int m2 )
{
  return this->GetSurface( this->surfaceRelationArray[m1 * materialCount + m2] );
}

//------------------------------------------------------------------------------
/**
  @brief Get the material index for the specified material.
  
  This does a linear search through all materials, so can be slow, ideally
  you should cache the index you get from here and reuse it.
*/
int nOdeServer::GetMaterialIndex( const char* m )
{
  int index = 0;
  for ( nStrNode* node = this->materialList.GetHead(); node; node = node->GetSucc() )
  {
    if ( 0 == strcmp( node->GetName(), m ) )
      return index;
    else
      ++index;
  }
  return -1;
}

//------------------------------------------------------------------------------
nOdeCollType nOdeServer::QueryCollType( nOdeCollClass cc1, nOdeCollClass cc2 )
{
  // check for CollClass override cases
  if ( (cc1 == ODE_COLLCLASS_ALWAYS_IGNORE) || (cc2 == ODE_COLLCLASS_ALWAYS_IGNORE) )
    return ODE_COLLTYPE_IGNORE;
  else if ( (cc1 == ODE_COLLCLASS_ALWAYS_QUICK) || (cc2 == ODE_COLLCLASS_ALWAYS_QUICK) )
    return ODE_COLLTYPE_QUICK;
  else if ( (cc1 == ODE_COLLCLASS_ALWAYS_CONTACT) || 
            (cc2 == ODE_COLLCLASS_ALWAYS_CONTACT) )
    return ODE_COLLTYPE_CONTACT;
  else if ( (cc1 == ODE_COLLCLASS_ALWAYS_EXACT) || (cc2 == ODE_COLLCLASS_ALWAYS_EXACT) )
    return ODE_COLLTYPE_EXACT;
    
  n_assert( this->collTypeTable );
  n_assert( (cc1 >= 0) && (cc2 >= 0) );
  n_assert( int(cc1) < this->numCollClasses );
  n_assert( int(cc2) < this->numCollClasses );

  int index = ( int(cc1) * this->numCollClasses ) + int(cc2);
  return this->collTypeTable[index];
}

//-------------------------------------------------------------------
void nOdeServer::SetDefaultPhysicsContext( nOdePhysicsContext* phys )
{
  this->ref_PhysicsContext = phys;
}

//-------------------------------------------------------------------
nOdePhysicsContext* nOdeServer::GetDefaultPhysicsContext()
{
  return this->ref_PhysicsContext.get();
}

//-------------------------------------------------------------------
void nOdeServer::SetDefaultCollideContext( nOdeCollideContext* ctxt)
{
  this->ref_CollideContext = ctxt;
}
//-------------------------------------------------------------------
nOdeCollideContext* nOdeServer::GetDefaultCollideContext()
{
  return this->ref_CollideContext.get();
}

//-------------------------------------------------------------------
nOdePlaneShape* nOdeServer::NewPlaneShape()
{
  nOdePlaneShape* shape = new nOdePlaneShape();
  shape->SetId( this->uniqueId++ );
  return shape;
}

//-------------------------------------------------------------------
nOdeSphereShape* nOdeServer::NewSphereShape()
{
  nOdeSphereShape* shape = new nOdeSphereShape();
  shape->SetId( this->uniqueId++ );
  return shape;
}

//-------------------------------------------------------------------
nOdeCapsuleShape* nOdeServer::NewCapsuleShape()
{
  nOdeCapsuleShape* shape = new nOdeCapsuleShape();
  shape->SetId( this->uniqueId++ );
  return shape;
}

//-------------------------------------------------------------------
nOdeBoxShape* nOdeServer::NewBoxShape()
{
  nOdeBoxShape* shape = new nOdeBoxShape();
  shape->SetId( this->uniqueId++ );
  return shape;
}

//-------------------------------------------------------------------
/**
  @brief Creates a new mesh shape (or reuses an existing one).
  @param id Identifier for the mesh, the identifier is used to share
            shapes.
  @return A pointer to a mesh shape.
  
  If you want to create multiple mesh shapes that all use a single
  copy of the mesh geometry create all these shapes with the same
  identifier.
  
  NOTE: The mesh geometry will not be loaded from file (since
  the identifier doesn't correspond to a file path), so it is
  up to you to ensure the geometry is loaded before any shapes
  try to make use of it. You only need to load the geometry
  once per identifier. The point is that the returned shape
  may or may not have had its geometry loaded already, use 
  nOdeTriMeshShape::GetTriMesh()->IsLoaded() to decide whether
  to call nOdeTriMeshShape::GetTriMesh()->Load(...) or not.
*/
nOdeTriMeshShape* nOdeServer::NewTriMeshShape( const char* id )
{
  n_assert( id );

  // mesh already exists?
  nOdeTriMesh* mesh = (nOdeTriMesh*)this->meshList.Find( id );
  if ( !mesh ) 
  {
    mesh = new nOdeTriMesh( id );
    this->meshList.AddTail( mesh );
  }
  
  nOdeTriMeshShape* shape = new nOdeTriMeshShape( mesh );
  shape->SetId( this->uniqueId++ );
  return shape;
}

//-------------------------------------------------------------------
/**
  @brief Creates a new mesh shape (or reuses an existing one).
  @param id Identifier for the mesh, the identifier is used to share
            shapes.
  @param file Path to mesh file.
  @return A pointer to a mesh shape.
  
  If you want to create multiple mesh shapes that all use a single
  copy of the mesh geometry create all these shapes with the same
  identifier.
  
  NOTE: The mesh geometry will be loaded from file only if it
  hasn't been already. If you try to create two shapes with the
  same identifier but different file paths, only the first file
  will be loaded.
*/
nOdeTriMeshShape* nOdeServer::NewTriMeshShape( const char* id, 
                                               const char* file )
{
  n_assert( id );

  // mesh already exists?
  nOdeTriMesh* mesh = (nOdeTriMesh*)this->meshList.Find( id );
  if ( !mesh ) 
  {
    mesh = new nOdeTriMesh( id );
    this->meshList.AddTail( mesh );
  }
  
  if ( !mesh->IsLoaded() )
    mesh->Load( this->ref_FileServer.get(), file );
  n_assert( mesh->IsLoaded() && "failed to load mesh" );
    
  nOdeTriMeshShape* shape = new nOdeTriMeshShape( mesh );
  shape->SetId( this->uniqueId++ );
  return shape;
}

//-------------------------------------------------------------------
void nOdeServer::ReleaseOdeShape( nOdeCollideShape* shape )
{
  n_assert( shape );
  delete shape;
}

//-------------------------------------------------------------------
/**
*/
void nOdeServer::BeginCollClasses()
{
  n_assert( !this->inBeginCollClasses );

  // free any previous collision class definitions
  this->numCollClasses = 0;
  nOdeCollClassNode* ccn;
  while ( (ccn = (nOdeCollClassNode*)this->collClassList.RemHead()) ) 
    delete ccn;
    
  this->inBeginCollClasses = true;
}

//-------------------------------------------------------------------
/**
*/
void nOdeServer::AddCollClass( const char* cl_name )
{
  n_assert( this->inBeginCollClasses );
  n_assert( cl_name );

  // make sure the class doesn't exist already
  if ( this->collClassList.Find( cl_name ) ) 
  {
    n_printf( "nOdeServer() WARNING: collision class '%s' already defined!\n",
              cl_name );
    return;
  }

  // create a new node
  nOdeCollClassNode* ccn = new nOdeCollClassNode( cl_name, this->numCollClasses++ );
  this->collClassList.AddTail( ccn );
}

//-------------------------------------------------------------------
/**
*/
void nOdeServer::EndCollClasses()
{
  n_assert( this->inBeginCollClasses );
  this->inBeginCollClasses = false;
}

//-------------------------------------------------------------------
/**
*/
void nOdeServer::BeginCollTypes()
{
  n_assert( !this->inBeginCollClasses );
  n_assert( !this->inBeginCollTypes );

  if ( this->collTypeTable ) 
  {
    delete[] this->collTypeTable;
    this->collTypeTable = NULL;
  }

  // create collision type table and initialize to 
  // "treat all collisions as exact checks"
  int tableSize = this->numCollClasses * this->numCollClasses;
  this->collTypeTable = new nOdeCollType[tableSize];
  int i;
  for ( i = 0; i < tableSize; i++ )
    this->collTypeTable[i] = ODE_COLLTYPE_EXACT;
  
  this->inBeginCollTypes = true;
}

//-------------------------------------------------------------------
/**
    Important: Collision types MUST be bidirectional, if one object
    checks collision with another object, the collision type must
    be identical as if the check would be in the other direction.
    Due to the implementation of the top-level-collision check,
    one of the 2 checks may return false, although a collision may
    take place!

     - 03-Jul-00   floh    created
*/
void nOdeServer::AddCollType( const char *cl1, const char *cl2, nOdeCollType ctype )
{
  n_assert( cl1 );
  n_assert( cl2 );
  n_assert( this->inBeginCollTypes );
  n_assert( this->collTypeTable );

  nOdeCollClassNode* ccn1 = (nOdeCollClassNode*)this->collClassList.Find(cl1);
  nOdeCollClassNode* ccn2 = (nOdeCollClassNode*)this->collClassList.Find(cl2);

  if ( !ccn1 ) 
  {
    n_printf( "nOdeServer(): collision class '%s' not defined!\n", cl1 );
    return;
  }
  if ( !ccn2 ) 
  {
    n_printf( "nOdeServer(): collision class '%s' not defined!\n", cl2 );
    return;
  }

  nOdeCollClass cc1 = ccn1->GetCollClass();
  nOdeCollClass cc2 = ccn2->GetCollClass();

  // enforce bidirectional collision type
  int index;
  index = (int(cc1) * this->numCollClasses) + int(cc2);
  this->collTypeTable[index] = ctype;
  index = (int(cc2) * this->numCollClasses) + int(cc1);
  this->collTypeTable[index] = ctype;
}

//-------------------------------------------------------------------
/**
*/
void nOdeServer::EndCollTypes()
{
  n_assert( this->inBeginCollTypes );
  this->inBeginCollTypes = false;
}

//-------------------------------------------------------------------
/**
*/
nOdeCollClass nOdeServer::QueryCollClass( const char* cc )
{
  n_assert( cc );
  nOdeCollClassNode* ccn = (nOdeCollClassNode*)this->collClassList.Find( cc );
  if ( !ccn ) 
  {
    n_printf( "nOdeServer: WARNING!!! Collision class '%s' not defined!\n", cc );
    n_error("Aborting!\n");
  }
  return ccn->GetCollClass();
}

//-------------------------------------------------------------------
/**
*/
nOdeCollType nOdeServer::QueryCollType( const char* s_cc1, const char* s_cc2 )
{
  n_assert( s_cc1 );
  n_assert( s_cc2 );
  nOdeCollClass cc1 = this->QueryCollClass( s_cc1 );
  nOdeCollClass cc2 = this->QueryCollClass( s_cc2 );
  return this->QueryCollType( cc1, cc2 );
}

//------------------------------------------------------------------------------
/**
  @brief Get the surface that corresponds to the specified index.
*/
nOdeSurface* nOdeServer::GetSurface( int index )
{
  n_assert( this->inBeginSurfaces );
  n_assert( index < this->numSurfaces );
  
  return &(this->surfaceArray[index]);
}

//------------------------------------------------------------------------------
/**
  @brief Get the surface index that corresponds to the specified surface.
  
  This does a linear search through all surfaces, so can be slow, ideally
  you should cache the index you get from here and reuse it.
*/
int nOdeServer::GetSurfaceIndex( const char* name )
{
  n_assert( this->inBeginSurfaces );
  
  for ( int i = 0; i < this->numSurfaces; i++ )
    if ( 0 == strcmp( this->surfaceArray[i].GetName(), name ) )
      return i;
  return -1;
}

//-------------------------------------------------------------------
/**
*/
void nOdeServer::EnableSurfaceParam( int index, const char* param )
{
  n_assert( this->inBeginSurfaces );
  n_assert( index < this->numSurfaces );
  
  nOdeSurface* surf = this->GetSurface( index );
  
  if ( 0 == strcmp( param, "fdir1" ) )
    surf->EnableFlag( dContactFDir1 );
  else if ( 0 == strcmp( param, "approxfdir1" ) )
    surf->EnableFlag( dContactApprox1_1 );
  else if ( 0 == strcmp( param, "approxfdir2" ) )
    surf->EnableFlag( dContactApprox1_2 );
  else if ( 0 == strcmp( param, "approxfboth" ) )
    surf->EnableFlag( dContactApprox1 );
  else
    n_printf( "nOdeServer::EnableSurfaceParam() - unrecognized param %s", param );
}

//-------------------------------------------------------------------
/**
*/
void nOdeServer::SetSurfaceParam( int index, const char* param, float value )
{
  n_assert( this->inBeginSurfaces );
  n_assert( index < this->numSurfaces );
  
  nOdeSurface* surf = this->GetSurface( index );
  
  if ( 0 == strcmp( param, "mu" ) )
  {
    surf->SetMU( value );
  }
  else if ( 0 == strcmp( param, "mu2" ) )
  {
    surf->SetMU2( value );
    surf->EnableFlag( dContactMu2 );
  }
  else if ( 0 == strcmp( param, "cfm" ) )
  {
    surf->SetSoftCFM( value );
    surf->EnableFlag( dContactSoftCFM );
  }
  else if ( 0 == strcmp( param, "erp" ) )
  {
    surf->SetSoftERP( value );
    surf->EnableFlag( dContactSoftERP );
  }
  else if ( 0 == strcmp( param, "bounce" ) )
  {
    surf->SetBounce( value );
    surf->EnableFlag( dContactBounce );
  }
  else if ( 0 == strcmp( param, "bvel" ) )
  {
    surf->SetBounceVel( value );
    surf->EnableFlag( dContactBounce );
  }
  else if ( 0 == strcmp( param, "motion1" ) )
  {
    surf->SetMotion1( value );
    surf->EnableFlag( dContactMotion1 );
  }
  else if ( 0 == strcmp( param, "motion2" ) )
  {
    surf->SetMotion2( value );
    surf->EnableFlag( dContactMotion2 );
  }
  else if ( 0 == strcmp( param, "slip1" ) )
  {
    surf->SetSlip1( value );
    surf->EnableFlag( dContactSlip1 );
  }
  else if ( 0 == strcmp( param, "slip2" ) )
  {
    surf->SetSlip2( value );
    surf->EnableFlag( dContactSlip2 );
  }
  else
    n_printf( "nOdeServer::SetSurfaceParam() - unrecognized param %s", param );
}
