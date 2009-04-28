#ifndef N_ODEMASSNODE_H
#define N_ODEMASSNODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeMassNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeMass.

  nOdeMassNode is meant to be used as a wrapper for nOdeMass(es) in order to
  be able to use nOdeMass(es) via script.
  
  Since you generally need to give a body a mass only once when you set 
  everything up you probably don't want one nOdeMassNode per nOdeBodyNode.
  What you could do instead is create one nOdeMassNode set it up, copy
  the mass to a body (using nOdeBodyNode::CopyMass()), then set up the 
  nOdeMassNode for the next body, copy the mass to the next body, and so on.
  
  (c) 2003 Vadim Macagon
  
  nOdeMassNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ODEMASS_H
#include "odephysics/nodemass.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeMassNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeMassNode : public nRoot
{
  public:
    /// constructor
    nOdeMassNode();
    /// destructor
    virtual ~nOdeMassNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    /// get the underlying nOdeMass
    nOdeMass& GetMass();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
    
  private:
    nOdeMass mass;
};

//------------------------------------------------------------------------------
inline
nOdeMass& nOdeMassNode::GetMass()
{
  return this->mass;
}

//------------------------------------------------------------------------------
#endif
