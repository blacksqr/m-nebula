#ifndef N_ODEUNIVERSALJOINTNODE_H
#define N_ODEUNIVERSALJOINTNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeUniversalJointNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeUniversalJoint.

  nOdeUniversalJointNode is meant to be used as a wrapper for 
  nOdeUniversalJoint(s) in order to be able to use nOdeUniversalJoint(s) 
  via script.

  (c) 2003 Vadim Macagon
  
  nOdeUniversalJointNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODEJOINTNODE_H
#include "odephysics/nodejointnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeUniversalJointNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeUniversalJointNode : public nOdeJointNode
{
  public:
    /// constructor
    nOdeUniversalJointNode();
    /// destructor
    virtual ~nOdeUniversalJointNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    virtual void InitJoint( const char* /*physContext*/ );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
