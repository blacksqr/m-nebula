#ifndef N_ODEHINGEJOINTNODE_H
#define N_ODEHINGEJOINTNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeHingeJointNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeHingeJoint.

  nOdeHingeJointNode is meant to be used as a wrapper for nOdeHingeJoint(s) 
  in order to be able to use nOdeHingeJoint(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeHingeJointNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODEJOINTNODE_H
#include "odephysics/nodejointnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeHingeJointNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeHingeJointNode : public nOdeJointNode
{
  public:
    /// constructor
    nOdeHingeJointNode();
    /// destructor
    virtual ~nOdeHingeJointNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    virtual void InitJoint( const char* /*physContext*/ );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
