#ifndef N_ODETRIMESHSHAPENODE_H
#define N_ODETRIMESHSHAPENODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeTriMeshShapeNode
  @ingroup NODEPhysicsContribModule
  @brief a brief description of the class

  nOdeTriMeshShapeNode is meant to be used as a wrapper for 
  nOdeTriMeshShape(s) in order to be able to use nOdeTriMeshShape(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeTriMeshShapeNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODECOLLIDESHAPENODE_H
#include "odephysics/nodecollideshapenode.h"
#endif

#include "util/nstl.h"

#undef N_DEFINES
#define N_DEFINES nOdeTriMeshShapeNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeTriMeshShapeNode : public nOdeCollideShapeNode
{
  public:
    /// constructor
    nOdeTriMeshShapeNode();
    /// destructor
    virtual ~nOdeTriMeshShapeNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );
    
    void InitShape( const char* id, const char* filename = 0 );
    
    void EnableTemporalCoherence( const char* shapeType, bool enable );
    bool IsTemporalCoherenceEnabled( const char* shapeType );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
  
  private:
    stl_string id;
    stl_string filename;
};

//------------------------------------------------------------------------------
#endif
