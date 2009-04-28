#ifndef N_ODETRIMESHSHAPE_H
#define N_ODETRIMESHSHAPE_H
//------------------------------------------------------------------------------
/**
  @class nOdeTriMeshShape
  @ingroup NODEPhysicsContribModule
  @brief Describes a tri-mesh shape for the ODE collision system.
  
  Note that mesh/mesh collision is not supported.

  (c) 2003 Vadim Macagon
  
  nOdeTriMeshShape is licensed under the terms of the Nebula License.
*/  
//------------------------------------------------------------------------------

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_ODECOLLIDESHAPE_H
#include "odephysics/nodecollideshape.h"
#endif

class nOdeTriMesh;
//------------------------------------------------------------------------------
class N_PUBLIC nOdeTriMeshShape : public nOdeCollideShape
{
  public:
  
    nOdeTriMeshShape( nOdeTriMesh* mesh );
    virtual ~nOdeTriMeshShape();
    
    nOdeTriMesh* GetTriMesh() const;
    
    void EnableTemporalCoherence( nOdeShapeType shapeType, bool enable );
    bool IsTemporalCoherenceEnabled( int shapeType ) const;
    
    /// visualize the collide shape (in model space)
    virtual void VisualizeLocal( nGfxServer* gfxServer, nPrimitiveServer* prim );
    
  private:
    
    nOdeTriMesh* triMesh;
};

//------------------------------------------------------------------------------
#endif
