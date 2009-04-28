#ifndef N_ODEGROUND_H
#define N_ODEGROUND_H

#ifndef N_ODEPHYSICS_H
#include "odephysics/odephysics.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

class nKernelServer;
class nGfxServer;
class nPrimitiveServer;
//------------------------------------------------------------------------------
class nOdeGround
{
  public:
    nOdeGround( nKernelServer* ks );
    ~nOdeGround();
    
    void Visualize( nGfxServer* gs, nPrimitiveServer* prim );
    
  private:
    nOdeServer* odeServer;
    nOdeCollideContext* collContext;
    nOdePhysicsContext* physContext;
    nAutoRef<nOdeCollideSpace> ref_CollSpace;
  
    nOdeBoxShape* groundBox;
    nOdePlaneShape* groundPlane;
};

//------------------------------------------------------------------------------
#endif // N_ODEGROUND_H
