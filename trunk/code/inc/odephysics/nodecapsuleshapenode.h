#ifndef N_ODECAPSULESHAPENODE_H
#define N_ODECAPSULESHAPENODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeCapsuleShapeNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeCapsuleShape.

  nOdeCapsuleShapeNode is meant to be used as a wrapper for 
  nOdeCapsuleShape(s) in order to be able to use nOdeCapsuleShape(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeCapsuleShapeNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODECOLLIDESHAPENODE_H
#include "odephysics/nodecollideshapenode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeCapsuleShapeNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeCapsuleShapeNode : public nOdeCollideShapeNode
{
  public:
    /// constructor
    nOdeCapsuleShapeNode();
    /// destructor
    virtual ~nOdeCapsuleShapeNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
