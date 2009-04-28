#ifndef N_ODESPHERESHAPENODE_H
#define N_ODESPHERESHAPENODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeSphereShapeNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeSphereShape.

  nOdeSphereShapeNode is meant to be used as a wrapper for 
  nOdeSphereShape(s) in order to be able to use nOdeSphereShape(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeSphereShapeNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODECOLLIDESHAPENODE_H
#include "odephysics/nodecollideshapenode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeSphereShapeNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeSphereShapeNode : public nOdeCollideShapeNode
{
  public:
    /// constructor
    nOdeSphereShapeNode();
    /// destructor
    virtual ~nOdeSphereShapeNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
