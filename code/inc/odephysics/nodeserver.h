#ifndef N_ODESERVER_H
#define N_ODESERVER_H
//------------------------------------------------------------------------------
/**
    @class nOdeServer
    @ingroup NODEPhysicsContribModule
    @brief A physics and a collision server rolled into one.

    This is a wrapper for ODE, it will provide some additional functionality
    that will simplify some things a bit (hopefully). 
    
    The collision part of the server is somewhat similar to the old collide 
    server, except that nCollideObject(s) were done away with, and instead the
    nOdeCollideShape(s) keep track of position/rotation of the shape. Collision
    classes and collision types will still work, although there may be less
    collision types.
    
    The physics part of the server will allow the creation of nOdeBody(s) which
    basically correspond to ODE rigid bodies, and the creation of various joints. 
    You can attach nOdeCollideShape(s) to nOdeBody(s).
    
    The server also maintains a collection of nOdeSurface(s), the idea being
    that you can define frequently used surface parameters via script on startup
    and re-use them over and over during the contact handling phase. Basically
    I'm thinking of having the system work similar to the collision classes,
    but instead of having "collision classes" you have "surface classes",
    so when objects with different surface classes come into contact you
    ask the server which surface parameters should be used and it gives you
    an nOdeSurface.

    (C) 2003  Vadim Macagon
    
    nOdeServer is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------

#ifndef N_ODECOLLTYPE_H
#include "odephysics/nodecolltype.h"
#endif

#ifndef N_STRNODE_H
#include "util/nstrnode.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_ODESURFACE_H
#include "odephysics/nodesurface.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeServer
#include "kernel/ndefdllclass.h"

class nOdeBoxShape;
class nOdeSphereShape;
class nOdeCapsuleShape;
class nOdeTriMeshShape;
class nOdePlaneShape;
class nOdeCollideShape;
class nOdeCollideContext;
//------------------------------------------------------------------------------
typedef int nOdeCollClass;
class nOdeCollClassNode : public nStrNode 
{
  private:
    nOdeCollClass collClass;

  public:
    nOdeCollClassNode( const char *n, nOdeCollClass cl )
        : nStrNode(n),
          collClass(cl)
    { }
    
    nOdeCollClass GetCollClass() 
    { return this->collClass; }
};

//------------------------------------------------------------------------------
class N_PUBLIC nOdeServer : public nRoot
{
  public:
    /// constructor
    nOdeServer();
    /// destructor
    virtual ~nOdeServer();
    
    // collision
    
    void SetDefaultCollideContext( nOdeCollideContext* );
    nOdeCollideContext* GetDefaultCollideContext();
        
    virtual nOdePlaneShape* NewPlaneShape();
    virtual nOdeBoxShape* NewBoxShape();
    virtual nOdeSphereShape* NewSphereShape();
    virtual nOdeCapsuleShape* NewCapsuleShape();
    virtual nOdeTriMeshShape* NewTriMeshShape( const char* id );
    virtual nOdeTriMeshShape* NewTriMeshShape( const char* id, const char* file );
    virtual void ReleaseOdeShape( nOdeCollideShape* );

    // define collision classes and collision check relationships
    void BeginCollClasses();
    void AddCollClass( const char* );
    void EndCollClasses();

    void BeginCollTypes();
    void AddCollType( const char*, const char*, nOdeCollType );
    void EndCollTypes();

    virtual nOdeCollClass QueryCollClass( const char* );
    virtual nOdeCollType QueryCollType( const char*, const char* );
    nOdeCollType QueryCollType( nOdeCollClass cc1, nOdeCollClass cc2 );
    
    // physics
    
    void SetDefaultPhysicsContext( nOdePhysicsContext* );
    nOdePhysicsContext* GetDefaultPhysicsContext();
        
    // surface parameters
    
    void BeginSurfaces( int num );
    void AddSurface( int index, const char* name );
    void EnableSurfaceParam( int index, const char* param );
    void SetSurfaceParam( int index, const char* param, float value );
    void EndSurfaces();
    
    void BeginMaterials();
    void AddMaterial( const char* material );
    void EndMaterials();
        
    void BeginSurfaceRelations();
    void SetSurfaceRelation( int material1, int material2, int surfaceIndex );
    void EndSurfaceRelations();
    
    nOdeSurface* GetSurface( int index );
    nOdeSurface* GetSurface( int material1, int material2 );
    int GetSurfaceIndex( const char* name );
    int GetMaterialIndex( const char* material );
        
    static nKernelServer* kernelServer;
    static nOdeServer* odeServer;
    
  private:
  
    int uniqueId;
    nList contextList;
    nHashList meshList;
    nAutoRef<nFileServer2> ref_FileServer;
    nOdeCollideContext* defaultCollideContext;
    // default physics context
    nDynAutoRef<nOdePhysicsContext> ref_PhysicsContext;
    // default collide context
    nDynAutoRef<nOdeCollideContext> ref_CollideContext;
    
    bool inBeginCollClasses;
    bool inBeginCollTypes;
    int numCollClasses;
    nStrList collClassList;
    nOdeCollType* collTypeTable;
    
    bool inBeginSurfaces;
    bool inBeginMaterials;
    bool inBeginSurfaceRelations;
    nOdeSurface* surfaceArray;
    int numSurfaces;
    int* surfaceRelationArray;
    nStrList materialList;
    int materialCount;
};

//------------------------------------------------------------------------------
#endif
