#ifndef N_ODEHINGETEST_H
#define N_ODEHINGETEST_H
//------------------------------------------------------------------------------
/**
  @brief Tests the hinge joint.
*/
//------------------------------------------------------------------------------
#ifndef N_ODEPHYSICS_H
#include "odephysics/odephysics.h"
#endif

class nKernelServer;
class nInputServer;
class nGfxServer;
class nPrimitiveServer;
//------------------------------------------------------------------------------
class nOdeHingeTest
{
  public:
    nOdeHingeTest( nKernelServer* ks );
    ~nOdeHingeTest();
  
    void HandleInput( nInputServer* input );
    void Update();
    void Visualize( nGfxServer* gs, nPrimitiveServer* prim );
  
  private:
    
    float RandFloat()
    {
      return ((float)rand()) / (float)0xffffffff;
    };
  
    nOdeServer* odeServer;
    nOdeCollideContext* collContext;
    nOdePhysicsContext* physContext;
    
    bool occasionalError;
    nOdeBody* bodies[2];
    nOdeHingeJoint* joint;
};  

//------------------------------------------------------------------------------
#endif // N_ODEHINGETEST_H
