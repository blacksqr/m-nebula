#ifndef N_ODESLIDERJOINTNODE_H
#define N_ODESLIDERJOINTNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeSliderJointNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeSliderJoint.

  nOdeSliderJointNode is meant to be used as a wrapper for nOdeSliderJoint(s) 
  in order to be able to use nOdeSliderJoint(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeSliderJointNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ODEJOINTNODE_H
#include "odephysics/nodejointnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeSliderJointNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeSliderJointNode : public nOdeJointNode
{
  public:
    /// constructor
    nOdeSliderJointNode();
    /// destructor
    virtual ~nOdeSliderJointNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    virtual void InitJoint( const char* /*physContext*/ );

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif
