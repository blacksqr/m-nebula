#ifndef N_ODECAR_H
#define N_ODECAR_H
//------------------------------------------------------------------------------
/**
  @brief A simple 3 wheeled buggy for the buggy test demo.
  
  nOdeCar is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_INPUTSERVER_H
#include "input/ninputserver.h"
#endif

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
class nOdeCar
{
  public:
    nOdeCar( nKernelServer* ks );
    ~nOdeCar();
    
    void SpeedUp();
    void SlowDown();
    void SteerLeft();
    void SteerRight();
    void Reset();
    
    void HandleInput( nInputServer* is );
    void Update();
    void Collide();
    void Visualize( nGfxServer* gs, nPrimitiveServer* prim );
    
  private:
  
    nOdeServer* odeServer;
    nOdeCollideContext* collContext;
    nOdePhysicsContext* physContext;
    nKernelServer* kernelServer;
    nAutoRef<nOdeCollideSpace> ref_CollSpace;
  
    float steer;
    float speed;
    nOdeBoxShape* chassisShape;
    nOdeBody* chassisBody;
    nOdeSphereShape* wheelShapes[4];
    nOdeBody* wheelBodies[4];
    nOdeHinge2Joint* joints[4];
    int surfIndex;
};

//------------------------------------------------------------------------------
inline
void nOdeCar::SpeedUp()
{
  this->speed += 0.3f;
}

//------------------------------------------------------------------------------
inline
void nOdeCar::SlowDown()
{
  this->speed -= 0.3f;
}

//------------------------------------------------------------------------------
inline
void nOdeCar::SteerLeft()
{
  this->steer -= 0.5f;
}

//------------------------------------------------------------------------------
inline
void nOdeCar::SteerRight()
{
  this->steer += 0.5f;
}

//------------------------------------------------------------------------------
inline
void nOdeCar::Reset()
{
  this->speed = 0.0f;
  this->steer = 0.0f;
}

//------------------------------------------------------------------------------
inline
void nOdeCar::HandleInput( nInputServer* input )
{
  if ( input->GetButton( "speed_up" ) )
    this->SpeedUp();
  else if ( input->GetButton( "slow_down" ) )
    this->SlowDown();
  else if ( input->GetButton( "steer_left" ) )
    this->SteerLeft();
  else if ( input->GetButton( "steer_right" ) )
    this->SteerRight();
  else if ( input->GetButton( "resetbuggy" ) )
    this->Reset();
}

//------------------------------------------------------------------------------
#endif // N_ODECAR_H
