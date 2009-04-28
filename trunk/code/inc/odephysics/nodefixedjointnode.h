#ifndef N_ODEFIXEDJOINTNODE_H
#define N_ODEFIXEDJOINTNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeFixedJointNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeFixedJointNode.

  nOdeFixedJointNode is meant to be used as a wrapper for nOdeFixedJoint(s) 
  in order to be able to use nOdeFixedJoint(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeFixedJointNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODEJOINTNODE_H
#include "odephysics/nodejointnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeFixedJointNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeFixedJointNode : public nOdeJointNode
{
  public:
    /// constructor
    nOdeFixedJointNode();
    /// destructor
    virtual ~nOdeFixedJointNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );
        
    virtual void InitJoint( const char* /*physContext*/ );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
