#ifndef N_ODEPLANESHAPENODE_H
#define N_ODEPLANESHAPENODE_H
//------------------------------------------------------------------------------
/**
  @class nOdePlaneShapeNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdePlaneShape.

  nOdePlaneShapeNode is meant to be used as a wrapper for 
  nOdePlaneShape(s) in order to be able to use nOdePlaneShape(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdePlaneShapeNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODECOLLIDESHAPENODE_H
#include "odephysics/nodecollideshapenode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdePlaneShapeNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdePlaneShapeNode : public nOdeCollideShapeNode
{
  public:
    /// constructor
    nOdePlaneShapeNode();
    /// destructor
    virtual ~nOdePlaneShapeNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
