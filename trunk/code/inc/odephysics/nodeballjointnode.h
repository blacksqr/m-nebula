#ifndef N_ODEBALLJOINTNODE_H
#define N_ODEBALLJOINTNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeBallJointNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeBallJoint.

  nOdeBallJointNode is meant to be used as a wrapper for nOdeBallJoint(s) in 
  order to be able to use nOdeBallJoint(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeBallJointNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODEJOINTNODE_H
#include "odephysics/nodejointnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeBallJointNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeBallJointNode : public nOdeJointNode
{
  public:
    /// constructor
    nOdeBallJointNode();
    /// destructor
    virtual ~nOdeBallJointNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    virtual void InitJoint( const char* /*physContext*/ );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
