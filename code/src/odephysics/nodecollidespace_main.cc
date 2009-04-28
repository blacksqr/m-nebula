#define N_IMPLEMENTS nOdeCollideSpace
//------------------------------------------------------------------------------
//  nodecollidespace.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeCollideSpace is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodecollidespace.h"
nNebulaScriptClass(nOdeCollideSpace, "nroot");

#ifndef N_ODECOLLIDECONTEXT_H
#include "odephysics/nodecollidecontext.h"
#endif

#ifndef N_ODECOLLIDESHAPENODE_H
#include "odephysics/nodecollideshapenode.h"
#endif

#ifndef N_ODECOLLIDESHAPE_H
#include "odephysics/nodecollideshape.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

#ifndef N_PRIMITIVESERVER_H
#include "gfx/nprimitiveserver.h"
#endif

#include "mathlib/vector.h"
#include "mathlib/line.h"


//------------------------------------------------------------------------------
nOdeCollideSpace::nOdeCollideSpace()
  : spaceId(0), ref_Context(0), ref_CurSubSpace(this), curGeomIndex(0)
{
}

//------------------------------------------------------------------------------
/**
  @brief Add this space to the parent space (if there is one).
*/
void nOdeCollideSpace::Initialize()
{
  nClass* spaceClass = this->kernelServer->FindClass( "nodecollidespace" );
  if ( this->GetParent()->IsA( spaceClass ) )
  {
    nOdeCollideSpace* parentSpace = (nOdeCollideSpace*)this->GetParent();
    parentSpace->AddSubSpace( this );
  }
}

//------------------------------------------------------------------------------
nOdeCollideSpace::~nOdeCollideSpace()
{
  /*
    Sub-spaces are child nodes of this space so they will be destroyed
    first, hence the only geoms remaining in the space by the time we
    get here should be shapes. There should be no shapes left at this stage!
  */
  n_assert( (0 == this->GetFirstShape()) && "Shapes must be removed first!" );
  if ( this->spaceId )
    dSpaceDestroy( this->spaceId );
}

//------------------------------------------------------------------------------
/**
  @brief Associate a collide context with the space 
         (and propagate to the sub-spaces).
  
  Make sure you always call this before actually trying to make use of the
  space, collision detection will not work otherwise.
*/
void nOdeCollideSpace::SetContext( nOdeCollideContext* context )
{
  this->ref_Context = context;
  nOdeCollideSpace* subSpace = this->GetFirstSubSpace();
  while ( subSpace )
  {
    subSpace->SetContext( context );
    subSpace = this->GetNextSubSpace();
  }
}

//------------------------------------------------------------------------------
nOdeCollideContext* nOdeCollideSpace::GetContext() const
{
  return this->ref_Context.get();
}

//------------------------------------------------------------------------------
/**
  @brief Create and add a sub-space to the space.
  @param className Can be one of:
                     - nodesimplespace
                     - nodehashspace
                     - nodequadtreespace
  @param subSpaceName The name of the sub-space, must not contain any '/'
                      as it is placed directly under this space in the NOH.
  @return The new sub-space instance.
*/
nOdeCollideSpace* nOdeCollideSpace::AddSubSpace( const char* className, 
                                                 const char* subSpaceName )
{
  this->kernelServer->PushCwd( this );
  nOdeCollideSpace* space = (nOdeCollideSpace*)
                              this->kernelServer->New( className, subSpaceName );
  this->kernelServer->PopCwd();
  return space;
}

//------------------------------------------------------------------------------
/**
  @brief Add a sub-space to this space.
  The collide context for the sub-space will be set to the same one as
  for the parent space.
*/
void nOdeCollideSpace::AddSubSpace( nOdeCollideSpace* subSpace )
{
  n_assert( subSpace );
  n_assert( this->spaceId );
  dSpaceAdd( this->spaceId, (dGeomID)subSpace->spaceId );
}

//------------------------------------------------------------------------------
/**
  @brief Destroy the sub-space (and recursively the sub-spaces within it).
*/
void nOdeCollideSpace::RemSubSpace( nOdeCollideSpace* subSpace )
{
  n_assert( subSpace );
  n_assert( this->spaceId );
  // ODE will remove it from the parent space (if needed)
  //dSpaceRemove( this->spaceId, (dGeomID)subSpace->spaceId );
  subSpace->Release();
}

//------------------------------------------------------------------------------
/**
  @brief Destroy all sub-spaces (and recursively the sub-spaces within them).
*/
void nOdeCollideSpace::RemAllSubSpaces()
{
  nClass* spaceClass = this->kernelServer->FindClass( "nodecollidespace" );
  nRoot* child = this->GetHead();
  
  while ( child )
  {
    if ( child->IsA( spaceClass ) )
      child->Release();
    else
      n_error( "Only nOdeCollideSpace(s) can be placed under an nOdeCollideSpace!" );
    
    child = this->GetHead();
  } 
}

//------------------------------------------------------------------------------
/**
  @brief Get the first sub-space in the space (if any).
  @return Pointer to a collide space, NULL if there are no sub-spaces in this 
          space.
  
  Only sub-spaces in the immediate space are considered.
*/
nOdeCollideSpace* nOdeCollideSpace::GetFirstSubSpace()
{
  this->ref_CurSubSpace = this->GetHead();
  if ( !this->ref_CurSubSpace.isvalid() )
    return 0;
    
  nClass* spaceClass = this->kernelServer->FindClass( "nodecollidespace" );
    
  if ( this->ref_CurSubSpace->IsA( spaceClass ) )
    return ((nOdeCollideSpace*)this->ref_CurSubSpace.get());
  else
  {
    n_error( "Only nOdeCollideSpace(s) can be placed under an nOdeCollideSpace!" );
    return 0;
  }
}

//------------------------------------------------------------------------------
/**
  @brief Get the next sub-space in the space.
  @return Pointer to a collide space, NULL when all sub-spaces in the space 
          have been traversed.
          
  Once all sub-spaces have been traversed (or at any other time) you can call 
  GetFirstSubSpace() to reset the iterator back to the first sub-space.
  
  If you remove sub-spaces while traversing, and it turns out to be the next
  sub-space in the traversal then the traversal will terminate
  (GetNextSubSpace() will return NULL) before actually reaching the end.
*/
nOdeCollideSpace* nOdeCollideSpace::GetNextSubSpace()
{
  if ( this->ref_CurSubSpace.isvalid() )
  {
    this->ref_CurSubSpace = this->ref_CurSubSpace->GetSucc();
    if ( !this->ref_CurSubSpace.isvalid() )
      return 0;
      
    nClass* spaceClass = this->kernelServer->FindClass( "nodecollidespace" );
    
    if ( this->ref_CurSubSpace->IsA( spaceClass ) )
      return ((nOdeCollideSpace*)this->ref_CurSubSpace.get());
    else
    {
      n_error( "Only nOdeCollideSpace(s) can be placed under an nOdeCollideSpace!" );
      return 0;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
void nOdeCollideSpace::AddShape( nOdeCollideShape* cs )
{
  n_assert( cs );
  dSpaceAdd( this->spaceId, cs->geomId );
}

//------------------------------------------------------------------------------
void nOdeCollideSpace::RemShape( nOdeCollideShape* cs )
{
  n_assert( cs );
  dSpaceRemove( this->spaceId, cs->geomId );
}

//------------------------------------------------------------------------------
/**
  @brief Remove all collide shapes from the space.
*/
void nOdeCollideSpace::RemAllShapes()
{
  dGeomID geom;
  int numGeoms = dSpaceGetNumGeoms( this->spaceId );
  int i = numGeoms - 1;
  
  while ( i >= 0 )
  {
    geom = dSpaceGetGeom( this->spaceId, i-- );
    if ( 0 == dGeomIsSpace( geom ) )
      dSpaceRemove( this->spaceId, geom );
  }
}

//------------------------------------------------------------------------------
/** 
  @brief Get the first shape in the space (if any).
  @return Pointer to a collide shape, NULL if there are no shapes in this space.
  
  Only shapes in the immediate space are considered, i.e. shapes in sub-spaces
  won't be taken into account.
  
  If you remove shapes while traversing, and it turns out to be the next
  shape in the traversal then the traversal will terminate
  (GetNextShape() will return NULL) before actually reaching the end.
*/
nOdeCollideShape* nOdeCollideSpace::GetFirstShape()
{
  dGeomID geom;
  this->curGeomIndex = 0;
  int numGeoms = dSpaceGetNumGeoms( this->spaceId );
  while ( this->curGeomIndex < numGeoms )
  {
    geom = dSpaceGetGeom( this->spaceId, this->curGeomIndex++ );
    if ( 0 == dGeomIsSpace( geom ) )
      return (nOdeCollideShape*)dGeomGetData( geom );
  }
  return 0;
}
    
//------------------------------------------------------------------------------
/**
  @brief Get the next shape in the space.
  @return Pointer to a collide shape, NULL when all shapes in the space have
          been traversed.
          
  Once all shapes have been traversed (or at any other time) you can call 
  GetFirstShape() to reset the iterator back to the first shape.
*/
nOdeCollideShape* nOdeCollideSpace::GetNextShape()
{
  dGeomID geom;
  int numGeoms = dSpaceGetNumGeoms( this->spaceId );
  while ( this->curGeomIndex < numGeoms )
  {
    geom = dSpaceGetGeom( this->spaceId, this->curGeomIndex++ );
    if ( 0 == dGeomIsSpace( geom ) )
      return (nOdeCollideShape*)dGeomGetData( geom );
  }
  return 0;
}

//------------------------------------------------------------------------------
/**
  @brief Clear contact counts for all shapes in this space (and sub-spaces).
*/
inline
void nOdeCollideSpace::ClearContactCounts()
{
  // clear the collision counters in all collide shapes
  int numGeoms = dSpaceGetNumGeoms( this->spaceId );
  dGeomID geom = 0;
  for ( int i = 0; i < numGeoms; i++ )
  {
    geom = dSpaceGetGeom( this->spaceId, i );
    if ( dGeomIsSpace( geom ) )
      ((nOdeCollideSpace*)dGeomGetData( geom ))->ClearContactCounts();
    else
      ((nOdeCollideShape*)dGeomGetData( geom ))->ClearCollissions();
  }
}

//------------------------------------------------------------------------------
/**
  @brief Detect collision between shapes in the space.
  @return Number of collision reports recorded.
*/
int nOdeCollideSpace::Collide( bool clearCache )
{
  this->ClearContactCounts();
  return this->ref_Context->Collide( this->spaceId, clearCache );
}

//------------------------------------------------------------------------------
/**
  @brief Check for intersection of a line against the shapes in the context.
  @param line      [in] The line to test (in global space).
  @param collType  [in] 
                        - ODE_COLLTYPE_IGNORE:  illegal (makes no sense)
                        - ODE_COLLTYPE_QUICK:   return all contacts (unsorted)
                        - ODE_COLLTYPE_CONTACT: return closest contact only
                        - ODE_COLLTYPE_EXACT:   same as ODE_COLLTYPE_QUICK
  @param collClass [in] The collision class to associate with the line. Use
                        ODE_COLLCLASS_ALWAYS_EXACT to disable filtering (so
                        the line will be tested against shapes regardless of 
                        their collision classes).
  @param crPtr [out] Pointer to contact information.
  @return Number of detected contacts (<b>at most 1 per shape</b>).
*/
int
nOdeCollideSpace::LineCheck( const line3& line, nOdeCollType collType, 
                             nOdeCollClass collClass, 
                             nOdeCollideReport**& crPtr )
{
  return this->ref_Context->LineCheck( this->spaceId, line, collType,
                                       collClass, crPtr );
}

//------------------------------------------------------------------------------
void nOdeCollideSpace::Visualize( nGfxServer* gs, nPrimitiveServer* prim )
{
  n_assert(gs);
  nOdeCollideShape* shape = this->GetFirstShape();
  if ( shape ) 
  {
    nRState rs;
    rs.Set( N_RS_LIGHTING, N_FALSE );          gs->SetState( rs );
    rs.Set( N_RS_TEXTUREHANDLE, 0 );           gs->SetState( rs );
    rs.Set( N_RS_ALPHABLENDENABLE, N_FALSE );  gs->SetState( rs );
    rs.Set( N_RS_ZFUNC, N_CMP_LESSEQUAL );     gs->SetState( rs );
    rs.Set( N_RS_ZBIAS, 1.0f );                gs->SetState( rs );
    matrix44 ivm;

    // get the current inverse view matrix
    gs->GetMatrix( N_MXM_INVVIEWER, ivm );

    // for each shape in the system...
    for ( ; shape; shape = this->GetNextShape() )
    {
      // get the current transformation
      matrix44 m;
      shape->GetTransform( &m );

      // multiply by inverse viewer matrix
      m.mult_simple( ivm );

      // render the object
      gs->SetMatrix( N_MXM_MODELVIEW, m );
      gs->Rgba( 0.0f, 0.0f, 1.0f, 1.0f );
      shape->VisualizeLocal( gs, prim );

      // render any collision contacts
      gs->SetMatrix( N_MXM_MODELVIEW, ivm );
      gs->Rgba( 1.0f, 0.0f, 0.0f, 1.0f );
      shape->VisualizeGlobal( gs );
    }
    rs.Set( N_RS_ZBIAS, 0.0f ); gs->SetState( rs );
  }
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
