#ifndef N_ODEQUADTREESPACE_H
#define N_ODEQUADTREESPACE_H
//------------------------------------------------------------------------------
/**
    @class nOdeQuadTreeSpace
    @ingroup NODEPhysicsContribModule
    @brief Defines a quad tree space.
    
    NOTE: This space doesn't work yet because dSpaceGetGeom has not been
    implemented in ODE yet, so if you really want to use this space go
    implement it.
    
    (c) 2003  Vadim Macagon
    
    nOdeQuadTreeSpace is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_ODECOLLIDESPACE_H
#include "odephysics/nodecollidespace.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeQuadTreeSpace
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nOdeQuadTreeSpace : public nOdeCollideSpace
{
  public:
    nOdeQuadTreeSpace();
    virtual ~nOdeQuadTreeSpace();
    virtual bool SaveCmds( nPersistServer* );
    
    // initialize the space (must be called before you try using the space!)
    void SetParams( const vector3& /*center*/, const vector3& /*extents*/, 
                    int /*depth*/ );
    // get the params
    void GetParams( vector3* /*center*/, vector3* /*extents*/, int* /*depth*/ );
        
    static nKernelServer* kernelServer;
    
  private:
    vector3 center;
    vector3 extents;
    int depth;
};

//------------------------------------------------------------------------------
#endif // N_ODEQUADTREESPACE_H
