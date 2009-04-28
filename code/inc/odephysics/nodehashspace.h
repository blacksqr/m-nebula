#ifndef N_ODEHASHSPACE_H
#define N_ODEHASHSPACE_H
//------------------------------------------------------------------------------
/**
    @class nOdeHashSpace
    @ingroup NODEPhysicsContribModule
    @brief Defines a hash space.
    
    (c) 2003  Vadim Macagon
    
    nOdeHashSpace is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_ODECOLLIDESPACE_H
#include "odephysics/nodecollidespace.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeHashSpace
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeHashSpace : public nOdeCollideSpace
{
  public:
    nOdeHashSpace();
    virtual ~nOdeHashSpace();
    virtual bool SaveCmds( nPersistServer* );
        
    /// set the smallest and largest cell sizes 
    void SetLevels( int minLevel, int maxLevel );
    /// get the smallest and largest cell sizes 
    void GetLevels( int* minLevel, int* maxLevel );
    
    static nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif // N_ODEHASHSPACE_H
