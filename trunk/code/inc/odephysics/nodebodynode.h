#ifndef N_ODEBODYNODE_H
#define N_ODEBODYNODE_H
//------------------------------------------------------------------------------
/**
    @class nOdeBodyNode
    @ingroup NODEPhysicsContribModule
    @brief A wrapper for nOdeBody.

    nOdeBodyNode is meant to be used as a wrapper for nOdeBody(s) in order to
    be able to use nOdeBody(s) via script. nOdeBody itself is not exposed
    to script directly because I didn't want to impose the memory overhead
    introduced by deriving from nRoot on people who had no need for
    script-side manipulation of nOdeBody(s).
    
    The underlying nOdeBody instance can be easily obtained, allowing you
    to dispose of the node if you wish. This allows you to create an
    nOdeBodyNode in script, setup the body via script, turn off auto-cleanup
    for the node and destroy the node, just don't forget to release the
    nOdeBody instance after you're done with it!

    (C) 2003  Vadim Macagon
    
    nOdeBodyNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ODEBODY_H
#include "odephysics/nodebody.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeBodyNode
#include "kernel/ndefdllclass.h"

class nOdePhysicsContext;
//------------------------------------------------------------------------------
class N_PUBLIC nOdeBodyNode : public nRoot
{
  public:
    /// constructor
    nOdeBodyNode();
    /// destructor
    virtual ~nOdeBodyNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* persistServer );

    /// create the underlying nOdeBody instance
    void InitBody( const char* physContextPath );

    /// copy the mass from a mass node for the body
    void CopyMass( const char* massPath );
    
    void SetMassTotal( float totalMass );
    float GetMassTotal();
    void SetMassCOG( const vector3& cog );
    void GetMassCOG( vector3* cog );
    void SetMassInertia( const matrix33& inertia );
    void GetMassInertia( matrix33* inertia );

    /// set what should happen to the nOdeBody instance when the node is destroyed
    void SetAutoCleanup( bool cleanup );
    /// check the current cleanup mode
    bool GetAutoCleanup() const;
    
    nOdeBody* GetBody() const;
    
    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
    
  private:
    nOdeBody* body;
    bool autoClean;
    nDynAutoRef<nOdePhysicsContext> ref_PhysContext;
};

//------------------------------------------------------------------------------
/**
  @brief Enable/disable auto cleanup mode (enabled by default).
  If auto cleanup is enabled the underlying nOdeBody instance will be released
  when the node is destroyed, otherwise it is up to you to release the nOdeBody
  instance.
*/
inline
void
nOdeBodyNode::SetAutoCleanup( bool cleanup )
{
  this->autoClean = cleanup;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nOdeBodyNode::GetAutoCleanup() const
{
  return this->autoClean;
}

//------------------------------------------------------------------------------
/**
  @brief Obtain the underlying nOdeBody instance.
  @warning If you disable auto cleanup YOU are responsible for releasing the
           nOdeBody instance you get from this method.
*/
inline
nOdeBody*
nOdeBodyNode::GetBody() const
{
  return this->body;
}

//------------------------------------------------------------------------------
#endif

