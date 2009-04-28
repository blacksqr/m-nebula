#ifndef N_ODEHINGE2JOINTNODE_H
#define N_ODEHINGE2JOINTNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeHinge2JointNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeHinge2Joint.

  nOdeHinge2JointNode is meant to be used as a wrapper for nOdeHinge2Joint(s) 
  in order to be able to use nOdeHinge2Joint(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeHinge2JointNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODEJOINTNODE_H
#include "odephysics/nodejointnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeHinge2JointNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeHinge2JointNode : public nOdeJointNode
{
  public:
    /// constructor
    nOdeHinge2JointNode();
    /// destructor
    virtual ~nOdeHinge2JointNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    virtual void InitJoint( const char* /*physContext*/ );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
