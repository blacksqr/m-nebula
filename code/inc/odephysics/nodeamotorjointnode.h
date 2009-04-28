#ifndef N_ODEAMOTORJOINTNODE_H
#define N_ODEAMOTORJOINTNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeAMotorJointNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeAMotorJoint.

  nOdeAMotorJointNode is meant to be used as a wrapper for nOdeAMotorJoint(s) 
  in order to be able to use nOdeAMotorJoint(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeAMotorJointNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODEJOINTNODE_H
#include "odephysics/nodejointnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeAMotorJointNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeAMotorJointNode : public nOdeJointNode
{
  public:
    /// constructor
    nOdeAMotorJointNode();
    /// destructor
    virtual ~nOdeAMotorJointNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    virtual void InitJoint( const char* /*physContext*/ );
    
    void SetMode( const char* /*mode*/ );
    const char* GetMode();
        
    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
