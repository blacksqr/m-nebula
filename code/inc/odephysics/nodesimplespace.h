#ifndef N_ODESIMPLESPACE_H
#define N_ODESIMPLESPACE_H
//------------------------------------------------------------------------------
/**
    @class nOdeSimpleSpace
    @ingroup NODEPhysicsContribModule
    @brief Defines a simple collision space.
    
    (c) 2003  Vadim Macagon
    
    nOdeSimpleSpace is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_ODECOLLIDESPACE_H
#include "odephysics/nodecollidespace.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeSimpleSpace
#include "kernel/ndefdllclass.h"
//------------------------------------------------------------------------------
class N_PUBLIC nOdeSimpleSpace : public nOdeCollideSpace
{
  public:
    nOdeSimpleSpace();
    virtual ~nOdeSimpleSpace();
    
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif // N_ODESIMPLESPACE_H
