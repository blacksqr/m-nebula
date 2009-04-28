#ifndef N_ODEBOXSHAPE_H
#define N_ODEBOXSHAPE_H
//------------------------------------------------------------------------------
/**
  @class nOdeBoxShape
  @ingroup NODEPhysicsContribModule
  @brief Describes a box shape for the ODE collision system.

  (c) 2003 Vadim Macagon
  
  nOdeBoxShape is licensed under the terms of the Nebula License.
*/  
//------------------------------------------------------------------------------

#ifndef N_ODECOLLIDESHAPE_H
#include "odephysics/nodecollideshape.h"
#endif

//------------------------------------------------------------------------------
class N_PUBLIC nOdeBoxShape : public nOdeCollideShape
{
  public:
    nOdeBoxShape();
    virtual ~nOdeBoxShape();
    
    /// set box size
    void SetSize( float xsize, float ysize, float zsize );
    /// get box size
    void GetSize( float* xsize, float* ysize, float* zsize );
    
    virtual void VisualizeLocal( nGfxServer* gfxServer, nPrimitiveServer* prim );
};

//------------------------------------------------------------------------------
#endif
