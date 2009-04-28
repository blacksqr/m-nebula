#include "odetests/nodehingetest.h"

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_INPUTSERVER_H
#include "input/ninputserver.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_PRIMITIVESERVER_H
#include "gfx/nprimitiveserver.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

nOdeHingeTest::nOdeHingeTest( nKernelServer* ks )
  : occasionalError( false )
{
  this->odeServer = (nOdeServer*)ks->Lookup( "/sys/servers/ode" );
  this->collContext = odeServer->GetDefaultCollideContext();
  this->physContext = odeServer->GetDefaultPhysicsContext();
  
  nOdeMass mass;
  mass.SetBox( 1, 0.5f, 0.5f, 0.5f );
  mass.AdjustTo( 1 );
  
  quaternion quat;
  quat.set_rotate_axis_angle( vector3(1, 1, 0), 0.25f * PI );

  this->bodies[0] = this->physContext->NewOdeBody();
  this->bodies[0]->SetMass( mass );
  this->bodies[0]->SetPosition( vector3(0.5f*0.5f, 0.5f*0.5f, 1) );
  this->bodies[0]->SetRotation( quat );

  this->bodies[1] = this->physContext->NewOdeBody();  
  this->bodies[1]->SetMass( mass );
  this->bodies[1]->SetPosition( vector3(-0.5f*0.5f, -0.5f*0.5f, 1) );
  this->bodies[1]->SetRotation( quat );
  
  this->joint = this->physContext->NewHingeJoint();
  this->joint->AttachTo( this->bodies[0], this->bodies[1] );
  this->joint->SetAnchor( vector3(0, 0, 1) );
  this->joint->SetAxis( vector3(1, -1, 1.41421356f) );
}

nOdeHingeTest::~nOdeHingeTest()
{
  this->physContext->ReleaseOdeJoint( this->joint );
  this->physContext->ReleaseOdeBody( this->bodies[0] );
  this->physContext->ReleaseOdeBody( this->bodies[1] );
}

void nOdeHingeTest::HandleInput( nInputServer* input )
{
  if ( input->GetButton( "hinge_error" ) )
    this->occasionalError = !this->occasionalError;
}

void nOdeHingeTest::Update()
{
  const float kd = -0.3f; // angular damping constant
  // add an oscillating torque to body 0, and also damp its rotational motion
  static float a = 0;
  vector3 w;
  this->bodies[0]->GetAngularVel( &w );
  this->bodies[0]->AddTorque( vector3( kd * w.x, 
                                       kd * w.y + 0.1f * n_cos(a),
                                       kd * w.z + 0.1f * n_sin(a) ) );
  a += 0.01f;

  // occasionally re-orient one of the bodies to create a deliberate error.
  if ( this->occasionalError )
  {
    static int count = 0;
    if ( (count % 20) == 0 ) 
    {
      matrix33 r1;
      // randomly adjust orientation of body 0
      this->bodies[0]->GetRotation( &r1 );
      quaternion quat;
      quat.set_rotate_axis_angle( vector3(RandFloat() - 0.5f, 
                                          RandFloat() - 0.5f, 
                                          RandFloat() - 0.5f),
                                  RandFloat() - 0.5f );
      matrix33 r2( quat );
      matrix33 r3 = r1 * r2;
      this->bodies[0]->SetRotation( r3 );

      // randomly adjust position of body 0
      vector3 pos;
      this->bodies[0]->GetPosition( &pos );
      this->bodies[0]->SetPosition( vector3(pos.x + 0.2f * (RandFloat() - 0.5f), 
                                            pos.y + 0.2f * (RandFloat() - 0.5f), 
                                            pos.z + 0.2f * (RandFloat() - 0.5f)) );
    }
    count++;
  }
}

void nOdeHingeTest::Visualize( nGfxServer* gs, nPrimitiveServer* prim )
{
  matrix44 ivm, m;
  // get the current inverse view matrix
  gs->GetMatrix( N_MXM_INVVIEWER, ivm );
  gs->PushMatrix( N_MXM_MODELVIEW );
  gs->Rgba( 1, 1, 0, 1);
  this->bodies[0]->GetTransform( &m );
  m.mult_simple( ivm );
  gs->SetMatrix( N_MXM_MODELVIEW, m );
  prim->WireBox( 0.5f, 0.5f, 0.5f );
  gs->Rgba( 0, 1, 1, 1);
  this->bodies[1]->GetTransform( &m );
  m.mult_simple( ivm );
  gs->SetMatrix( N_MXM_MODELVIEW, m );
  prim->WireBox( 0.5f, 0.5f, 0.5f * 0.8f );
  gs->PopMatrix( N_MXM_MODELVIEW );
}

