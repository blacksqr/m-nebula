#ifndef N_ODEJOINTNODE_H
#define N_ODEJOINTNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeJointNode

  @brief A wrapper for nOdeJoint.

  nOdeJointNode descendants are meant to be used as wrappers for nOde*Joint(s) 
  in order to be able to use nOde*Joint(s) via script.

  I should note that cloning or persisting joints will likely be of use only
  for the initial setup of your ODE simulation, this is due to the nature in
  which joints work - most things are relative to position/orientation of the 
  bodies at the time the joint is attached. You will have to do some work
  yourself in order to save/load a simulation in progress.

  (c) 2003 Vadim Macagon
  
  nOdeJointNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#include "util/nstl.h"

#undef N_DEFINES
#define N_DEFINES nOdeJointNode
#include "kernel/ndefdllclass.h"

class nOdeJoint;
class nOdePhysicsContext;
class nOdeBodyNode;

//------------------------------------------------------------------------------
class N_PUBLIC nOdeJointNode : public nRoot
{
  public:
    /// constructor
    nOdeJointNode();
    /// destructor
    virtual ~nOdeJointNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    /// create the underlying nOdeJoint instance
    virtual void InitJoint( const char* /*physContext*/ );

    /// set what should happen to the nOdeJoint instance when the node is destroyed
    void SetAutoCleanup( bool cleanup );
    /// check the current cleanup mode
    bool GetAutoCleanup();

    /// obtain the underlying nOdeJoint instance
    nOdeJoint* GetJoint() const;
    
    void AttachTo( const char* b1, const char* b2 );
    const char* GetType();
    const char* GetBody( int index );
    /// set a joint parameter
    virtual void SetParam( const char* param, int axis, float value );
    /// get a joint parameter
    virtual float GetParam( const char* param, int axis );

#ifdef ODE_BREAKABLE_JOINTS
    void SetBreakModeFlag( const char* flag, bool value );
    bool GetBreakModeFlag( const char* flag );
#endif // ODE_BREAKABLE_JOINTS

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
    
  protected:
    nOdeJoint* joint;
    nDynAutoRef<nOdePhysicsContext> ref_PhysContext;
    
  private:
    stl_string bodyPath1;
    stl_string bodyPath2;
    bool autoClean;
};

//------------------------------------------------------------------------------
#endif
