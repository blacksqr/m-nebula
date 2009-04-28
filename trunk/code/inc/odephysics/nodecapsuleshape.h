#ifndef N_ODECAPSULESHAPE_H
#define N_ODECAPSULESHAPE_H
//------------------------------------------------------------------------------
/**
  @class nOdeCapsuleShape
  @ingroup NODEPhysicsContribModule
  @brief Describes a capsule shape for the ODE collision system.

  (c) 2003 Vadim Macagon
  
  nOdeCapsuleShape is licensed under the terms of the Nebula License.
*/  
//------------------------------------------------------------------------------

#ifndef N_ODECOLLIDESHAPE_H
#include "odephysics/nodecollideshape.h"
#endif

class nGfxServer;
class nPrimitiveServer;
//------------------------------------------------------------------------------
class N_PUBLIC nOdeCapsuleShape : public nOdeCollideShape
{
  public:
    nOdeCapsuleShape();
    virtual ~nOdeCapsuleShape();
    
    void SetSize( float rad, float len );
    void GetSize( float* rad, float* len );
    
    /// visualize the collide shape (in model space)
    virtual void VisualizeLocal( nGfxServer* gfxServer, nPrimitiveServer* prim );
};

//------------------------------------------------------------------------------
#endif
