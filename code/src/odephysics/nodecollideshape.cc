#define N_IMPLEMENTS nOdeCollideShape
//------------------------------------------------------------------------------
//  nodecollideshape.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeCollideShape is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodecollideshape.h"
#include "mathlib/matrix.h"
#include "mathlib/vector.h"
#include "gfx/ngfxtypes.h"
#include "odephysics/nodebody.h"
#include "odephysics/nodecollidereport.h"
#include "odephysics/nodecollidespace.h"
#include "odephysics/odeutil.h"

dGeomID nOdeCollideShape::rayId = 0;
nOdeCollType nOdeCollideShape::rayCollType = ODE_COLLTYPE_QUICK;

//------------------------------------------------------------------------------
/**
*/
nOdeCollideShape::~nOdeCollideShape()
{
}

//------------------------------------------------------------------------------
/**
*/
nOdeCollideShape::nOdeCollideShape() 
  : geomId( 0 ), collClass( 0 ), shapeType( OST_UNKNOWN ), userdata( 0 ),
    numColls( 0 ), id( 0 )
{
}

//---------------------------------------------------------------
/**
  @brief Get the space this shape belongs to (if any).
*/
nOdeCollideSpace* nOdeCollideShape::GetSpace()
{
  dSpaceID spaceId = dGeomGetSpace( this->geomId );
  if ( spaceId )
    return (nOdeCollideSpace*)dGeomGetData( (dGeomID)spaceId );
  return 0;
}

//---------------------------------------------------------------
/**
*/
nOdeCollClass nOdeCollideShape::GetCollClass() const 
{
  return this->collClass;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::ClearCollissions()
{
  this->numColls = 0;
}

//------------------------------------------------------------------------------
/**
  @brief Obtain total number of collisions the shape was involved in.
  
  Unlike the report handler the value returned is not the number of
  collision reports for this shape but rather the total number of collisions
  this shape was involved in. eg. if there are two reports, one with 3 contacts
  and one with 2, the value returned will be 5.
*/
int nOdeCollideShape::GetNumCollissions() const
{
  return this->numColls;
}

//------------------------------------------------------------------------------
/**
*/
int nOdeCollideShape::GetId() const
{
  return this->id;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::SetId( int uniqueId )
{
  this->id = uniqueId;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::SetData( void* data )
{
  this->userdata = data;
}

//------------------------------------------------------------------------------
/**
*/
void* nOdeCollideShape::GetData() const
{
  return this->userdata;
}

//------------------------------------------------------------------------------
/**
*/
nOdeCollideShape::nOdeShapeType nOdeCollideShape::GetShapeType() const
{
  return this->shapeType;
}

//------------------------------------------------------------------------------
/**
*/
//void nOdeCollideShape::Enable( bool flag )
//{
//  if ( flag )
//    dGeomEnable( this->geomId );
//  else
//    dGeomDisable( this->geomId );
//}

//------------------------------------------------------------------------------
/**
*/
//bool nOdeCollideShape::IsEnabled()
//{
//  return (1 == dGeomIsEnabled( this->geomId ));
//}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::SetCollClass( nOdeCollClass cc ) 
{
  this->collClass = cc;
}

//------------------------------------------------------------------------------
nOdeBody* nOdeCollideShape::GetBody()
{
  dBodyID body = dGeomGetBody( this->geomId );
  if ( body > 0 )
    return (nOdeBody*)dBodyGetData( body );
  else
    return NULL;
}

//------------------------------------------------------------------------------
void nOdeCollideShape::AttachTo( nOdeBody* body )
{
  dGeomSetBody( this->geomId, body->bodyId );
}

//------------------------------------------------------------------------------
/**
  @brief Check for intersection of line with shape.
  @param collType [in] 
                      - ODE_COLLTYPE_IGNORE: illegal (makes no sense)
                      - ODE_COLLTYPE_QUICK:  return as soon as the first contact
                                             is found
                      - ODE_COLLTYPE_CONTACT: return closest contact
                      - ODE_COLLTYPE_EXACT: same as ODE_COLLTYPE_QUICK
  @param line [in] Line to test (in global space).
  @param contact [out] Pointer to structure where contact information should be
                       stored.
  @return True if contact was detected.
*/
bool nOdeCollideShape::LineCheck( nOdeCollType collType, const line3& line,
                                  nOdeCollideContact* contact )
{
  n_assert( 0 != nOdeCollideShape::rayId );
  n_assert( collType != ODE_COLLTYPE_IGNORE );
  
  nOdeCollideShape::rayCollType = collType;
  
  // setup ray geom
  dGeomRaySet( nOdeCollideShape::rayId, line.b.x, line.b.y, line.b.z,
               line.m.x, line.m.y, line.m.z );
  dGeomRaySetLength( nOdeCollideShape::rayId, line.len() );
  
  switch ( collType )
  {
    case ODE_COLLTYPE_QUICK:
    case ODE_COLLTYPE_EXACT:
      dGeomRaySetParams( nOdeCollideShape::rayId,
                         1/*firstContact*/, 0/*backFaceCull*/ );
      dGeomRaySetClosestHit( nOdeCollideShape::rayId, 0 );
      break;
    case ODE_COLLTYPE_CONTACT:
      dGeomRaySetParams( nOdeCollideShape::rayId,
                         0/*firstContact*/, 0/*backFaceCull*/ );
      dGeomRaySetClosestHit( nOdeCollideShape::rayId, 1 );
      break;
  }
  
  dContactGeom contactGeom;
  
  // generate at most 1 contact
  int numContacts = dCollide( nOdeCollideShape::rayId, this->geomId, 
                              1, &contactGeom, sizeof( dContactGeom ) );
  
  contact->contact.set( contactGeom.pos[0], contactGeom.pos[1], contactGeom.pos[2] );
  contact->normal.set( contactGeom.normal[0], 
                       contactGeom.normal[1], 
                       contactGeom.normal[2] );
  contact->depth = contactGeom.depth;
  
  return (numContacts > 0);
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::VisualizeLocal( nGfxServer* /*gfxServer*/, 
                                       nPrimitiveServer* prim )
{ 
  // empty
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::VisualizeGlobal( nGfxServer* /*gfxServer*/ )
{ 
  // empty
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::SetTransform( const matrix44& mat )
{
  dMatrix3 rotation;
  nOdeUtil::NebMatrixToODE( mat, rotation );
  dGeomSetPosition( this->geomId, mat.M41, mat.M42, mat.M43 );
  dGeomSetRotation( this->geomId, rotation );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::GetTransform( matrix44* result )
{
  const dVector3& position = *(const dVector3*)dGeomGetPosition( this->geomId );
  const dMatrix3& rotation = *(const dMatrix3*)dGeomGetRotation( this->geomId );
  nOdeUtil::OdeMatrixToNeb( position, rotation, *result );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::SetPosition( const vector3& pos )
{
  dGeomSetPosition( this->geomId, pos.x, pos.y, pos.z );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::GetPosition( vector3* pos )
{
  const dReal* res = dGeomGetPosition( this->geomId );
  pos->set( res[0], res[1], res[2] );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::SetRotation( const matrix44& rot )
{
  dMatrix3 rotation;
  nOdeUtil::NebMatrixToODE( rot, rotation );
  dGeomSetRotation( this->geomId, rotation );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::SetRotation( const matrix33& rot )
{
  dMatrix3 rotation;
  nOdeUtil::NebMatrixToODE( rot, rotation );
  dGeomSetRotation( this->geomId, rotation );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::SetRotation( const quaternion& rot )
{
  dQuaternion quat;
  nOdeUtil::NebQuatToOde( rot, quat );
  dMatrix3 mat;
  dQtoR( quat, mat );
  dGeomSetRotation( this->geomId, mat );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::GetRotation( matrix33* rot )
{
  const dMatrix3& rotation = *(const dMatrix3*)dGeomGetRotation( this->geomId );
  nOdeUtil::OdeMatrixToNeb( rotation, *rot );
}

//------------------------------------------------------------------------------
/**
*/
void nOdeCollideShape::GetRotation( quaternion* res )
{
  const dMatrix3& mat = *(const dMatrix3*)dGeomGetRotation( this->geomId );
  dQuaternion quat;
  dRtoQ( mat, quat );
  nOdeUtil::OdeQuatToNeb( quat, res );
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
