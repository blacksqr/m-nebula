#ifndef N_ODEBOXSHAPENODE_H
#define N_ODEBOXSHAPENODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeBoxShapeNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeBoxShape.

  nOdeBoxShapeNode is meant to be used as a wrapper for 
  nOdeBoxShape(s) in order to be able to use nOdeBoxShape(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeBoxShapeNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODECOLLIDESHAPENODE_H
#include "odephysics/nodecollideshapenode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeBoxShapeNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeBoxShapeNode : public nOdeCollideShapeNode
{
  public:
    /// constructor
    nOdeBoxShapeNode();
    /// destructor
    virtual ~nOdeBoxShapeNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
