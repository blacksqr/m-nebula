#ifndef N_ODESPHERESHAPE_H
#define N_ODESPHERESHAPE_H
//------------------------------------------------------------------------------
/**
  @class nOdeSphereShape
  @ingroup NODEPhysicsContribModule
  @brief Describes a sphere shape for the ODE collision system.

  (c) 2003 Vadim Macagon
  
  nOdeSphereShape is licensed under the terms of the Nebula License.
*/  
//------------------------------------------------------------------------------

#ifndef N_ODECOLLIDESHAPE_H
#include "odephysics/nodecollideshape.h"
#endif

//------------------------------------------------------------------------------
class N_PUBLIC nOdeSphereShape : public nOdeCollideShape
{
  public:
    nOdeSphereShape();
    virtual ~nOdeSphereShape();
    
    void SetRadius( float rad );
    float GetRadius();
    
    virtual void VisualizeLocal( nGfxServer* gfxServer, nPrimitiveServer* prim );
};

//------------------------------------------------------------------------------
#endif
