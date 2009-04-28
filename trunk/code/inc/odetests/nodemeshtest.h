#ifndef N_ODEMESHTEST_H
#define N_ODEMESHTEST_H
//------------------------------------------------------------------------------
/**
  @brief Tests the tri-mesh shape.
      
  (c) 2003 Vadim Macagon
  
  nOdeMeshTest is licensed under the terms of the Nebula License.
*/

#ifndef N_ODEPHYSICS_H
#include "odephysics/odephysics.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

class nKernelServer;
class nInputServer;
class nGfxServer;
class nPrimitiveServer;

//------------------------------------------------------------------------------
struct nOdeObject
{
  nOdeBody* body;
  nOdeCollideShape* shape;
};

//------------------------------------------------------------------------------
class nOdeMeshTest
{
  public:
    nOdeMeshTest( nKernelServer* ks );
    ~nOdeMeshTest();
  
    void HandleInput( nInputServer* input );
    void Update();
    void Collide();
    void Visualize( nGfxServer* gs, nPrimitiveServer* prim );
  
  private:
    nOdeServer* odeServer;
    nOdeCollideContext* collContext;
    nOdePhysicsContext* physContext;
    nAutoRef<nOdeCollideSpace> ref_CollSpace;
    
    nOdeObject objects[3];
    nOdeTriMeshShape* ground;
    int surfIndex;
    
};

//------------------------------------------------------------------------------
#endif // N_ODEMESHTEST
