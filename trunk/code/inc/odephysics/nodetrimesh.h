#ifndef N_ODETRIMESH_H
#define N_ODETRIMESH_H
//------------------------------------------------------------------------------
/**
    @class nOdeTriMesh
    @ingroup NODEPhysicsContribModule
    @brief Stores collision mesh data and handles collision between the ODE
           tri-list geom and other geoms.

    (c) 2003 Vadim Macagon
    Based on Radon Labs nOpcodeShape & nCollideShape.
    
    nOdeTriMesh is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#undef N_DEFINES
#define N_DEFINES nOdeTriMesh
#include "kernel/ndefdllclass.h"

class nFileServer2;
class nGfxServer;
class nFile;
class nOdeServer;
//------------------------------------------------------------------------------
class N_PUBLIC nOdeTriMesh : public nHashNode
{
  public:
  
    friend class nOdeTriMeshShape;
    
    /// constructor
    nOdeTriMesh( const char* id );
    /// destructor
    ~nOdeTriMesh();

    /// increment refcount by one
    int AddRef();
    /// decrement refcount by one
    int RemRef();
    /// get current refcount
    int GetRef();

    /// begin defining collide mesh
    void Begin( int numVertices, int numTriangles );
    /// add a vertex
    void SetVertex( int index, vector3& v );
    /// add a triangle
    void SetTriangle( int index, int p0Index, int p1Index, int p2Index );
    /// finish defining geometry
    void End();
    
    /// has the mesh been loaded?
    bool IsLoaded();
    
    /// load collide geometry from file
    bool Load( nFileServer2* fileServer, const char* filename );

    /// visualize collide mesh
    void Visualize( nGfxServer* gfxServer );
    
  private:
    
    /// load n3d file
    bool LoadN3D(nFileServer2* fileServer, const char* filename);
    /// load nvx file
    bool LoadNVX(nFileServer2* fileServer, const char* filename);
    /// read 32 bit integer from file
    int readInt(nFile* file);
    
    int refCount;

    static nOdeServer* odeServer;
    dTriMeshDataID meshDataId;
    int numVertices;
    int numFaces;
    float* vertexData;
    int*   faceData;
    bool isLoaded;
};

//------------------------------------------------------------------------------
#endif
