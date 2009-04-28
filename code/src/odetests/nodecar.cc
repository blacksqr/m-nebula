#include "odetests/nodecar.h"
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//------------------------------------------------------------------------------
#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
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

#define LENGTH 0.7f // chassis length
#define WIDTH 0.5f  // chassis width
#define HEIGHT 0.2f // chassis height
#define RADIUS 0.1f  // wheel radius
#define STARTY 1.5f // starting height of chassis
#define CMASS 1.0f    // chassis mass
#define WMASS 0.2f  // wheel mass

nOdeCar::nOdeCar( nKernelServer* ks )
  : kernelServer( ks ), steer( 0 ), speed( 0 ), 
    ref_CollSpace( ks, 0 )
{
  this->odeServer = (nOdeServer*)kernelServer->Lookup( "/sys/servers/ode" );
  this->collContext = odeServer->GetDefaultCollideContext();
  this->physContext = odeServer->GetDefaultPhysicsContext();
  this->ref_CollSpace = "/sys/servers/ode/spaces/root";
  nOdeMass mass;
  
  // create chassis
  this->chassisShape = odeServer->NewBoxShape();
  this->chassisShape->SetSize( LENGTH, HEIGHT, WIDTH );
  this->chassisShape->SetCollClass( this->odeServer->QueryCollClass( "car" ) );
  this->chassisBody = physContext->NewOdeBody();
  this->chassisBody->SetPosition( vector3( 0, STARTY, 0 ) );
  mass.SetBox( 1.0f, LENGTH, HEIGHT, WIDTH );
  mass.AdjustTo( CMASS );
  this->chassisBody->SetMass( mass );
  this->chassisShape->AttachTo( this->chassisBody );
  this->ref_CollSpace->AddShape( this->chassisShape );
  
  //quaternion quat;
  //quat.set_rotate_axis_angle( vector3(1, 0, 0), 0.5f * PI );
  nOdeBody* body;
  
  int i;
  
  // create wheels
  for ( i = 0; i < 4; i++ )
  {
    this->wheelShapes[i] = odeServer->NewSphereShape();
    this->wheelShapes[i]->SetRadius( RADIUS );
    body = this->physContext->NewOdeBody();
    //body->SetRotation( quat );
    mass.Reset();
    mass.SetSphere( 1.0f, RADIUS );
    mass.AdjustTo( WMASS );
    body->SetMass( mass );
    this->wheelShapes[i]->AttachTo( body );
    this->wheelShapes[i]->SetCollClass( this->odeServer->QueryCollClass( "car" ) );
    this->wheelBodies[i] = body;
    this->ref_CollSpace->AddShape( this->wheelShapes[i] );
  }
  
  wheelBodies[0]->SetPosition(vector3(0.5f*LENGTH, STARTY - HEIGHT*1.5f, WIDTH*0.5f));
  wheelBodies[1]->SetPosition(vector3(0.5f*LENGTH, STARTY - HEIGHT*1.5f, -WIDTH*0.5f));
  wheelBodies[2]->SetPosition(vector3(-0.5f*LENGTH, STARTY - HEIGHT*1.5f, WIDTH*0.5f));
  wheelBodies[3]->SetPosition(vector3(-0.5f*LENGTH, STARTY - HEIGHT*1.5f, -WIDTH*0.5f));
  
  // create front and back wheel hinges
  vector3 bodyPos;
  for ( i = 0; i < 4; i++ ) 
  {
    nOdeHinge2Joint* j = this->physContext->NewHinge2Joint();
    j->AttachTo( this->chassisBody, this->wheelBodies[i] );
    this->wheelBodies[i]->GetPosition( &bodyPos );
    j->SetAnchor( bodyPos );
    j->SetAxis1( vector3( 0.0f, 1.0f, 0.0f ) );
    j->SetAxis2( vector3( 0.0f, 0.0f, (i%2)==0 ? -1.0f : 1.0f ) );
    // set joint suspension
    j->SetParam( dParamSuspensionERP, 0.25f );
    j->SetParam( dParamSuspensionCFM, 0.004f );
    j->SetParam( dParamLoStop, 0.0f );
    j->SetParam( dParamHiStop, 0.0f );
    j->SetParam( dParamFMax, 50.0f );
    j->SetParam( dParamVel2, 0.0f );
    j->SetParam( dParamFMax2, 80.0f );
    this->joints[i] = j;
  }
  /*
  // lock back wheels along the steering axis
  for ( i = 1; i < 3; i++ ) 
  {
    // set stops to make sure wheels always stay in alignment
    this->joints[i]->SetParam( dParamLoStop, 0.0f );
    this->joints[i]->SetParam( dParamHiStop, 0.0f );
  }
  */
  this->surfIndex = this->odeServer->GetSurfaceIndex( "buggy" );
}

nOdeCar::~nOdeCar()
{
  this->ref_CollSpace->RemShape( this->chassisShape );
  this->odeServer->ReleaseOdeShape( this->chassisShape );
  this->physContext->ReleaseOdeBody( this->chassisBody );
  for ( int i = 0; i < 4; i++ )
  {
    this->ref_CollSpace->RemShape( this->wheelShapes[i] );
    this->odeServer->ReleaseOdeShape( this->wheelShapes[i] );
    this->physContext->ReleaseOdeBody( this->wheelBodies[i] );
    this->physContext->ReleaseOdeJoint( this->joints[i] );
  }
}

void nOdeCar::Update()
{
  // motor
  nOdeHinge2Joint* j = this->joints[0];
  j->SetParam( dParamVel2, -this->speed );
  //j->SetParam( dParamFMax2, 0.1f );

  // steering
  float v = this->steer - j->GetAngle1();
  if ( v > 0.1f ) 
    v = 0.1f;
  if ( v < -0.1f ) 
    v = -0.1f;
  v *= 10.0f;
  j->SetParam( dParamVel, v );
  j->SetParam( dParamFMax, 0.2f );
  j->SetParam( dParamLoStop, -0.75f );
  j->SetParam( dParamHiStop, 0.75f );
  j->SetParam( dParamFudgeFactor, 0.1f );
  
  // motor
  j = this->joints[1];
  j->SetParam( dParamVel2, this->speed );
  //j->SetParam( dParamFMax2, 0.1f );

  // steering
  v = this->steer - j->GetAngle1();
  if ( v > 0.1f ) 
    v = 0.1f;
  if ( v < -0.1f ) 
    v = -0.1f;
  v *= 10.0f;
  j->SetParam( dParamVel, v );
  j->SetParam( dParamFMax, 0.2f );
  j->SetParam( dParamLoStop, -0.75f );
  j->SetParam( dParamHiStop, 0.75f );
  j->SetParam( dParamFudgeFactor, 0.1f );
}

/**
  Convert any contacts that were detected into contact joints.
*/
void nOdeCar::Collide()
{
  int i, j;
  nOdeCollideReport** report;
  nOdeCollideContact* contact;
  // handle chassis collisions
  int numColls = this->collContext->GetCollissionReports( this->chassisShape, report );
  for ( i = 0; i < numColls; i++ )
  {
    for ( j = 0; j < report[i]->GetNumContacts(); j++ )
    {
      contact = report[i]->GetContact( j );
      this->physContext->CreateContact( report[i]->cs1->GetBody(),
                                        report[i]->cs2->GetBody(),
                                        *contact,
                                        *(this->odeServer->
                                        GetSurface( this->surfIndex )));
    }
  }
  // handle wheel collisions
  for ( int w = 0; w < 4; w++ )
  {
    numColls = this->collContext->GetCollissionReports( this->wheelShapes[w], report );
    for ( i = 0; i < numColls; i++ )
    {
      for ( j = 0; j < report[i]->GetNumContacts(); j++ )
      {
        contact = report[i]->GetContact( j );
        this->physContext->CreateContact( report[i]->cs1->GetBody(),
                                          report[i]->cs2->GetBody(),
                                          *contact,
                                          *(this->odeServer->
                                          GetSurface( this->surfIndex )));
      }
    }
  }
}

void nOdeCar::Visualize( nGfxServer* gs, nPrimitiveServer* prim )
{
  matrix44 ivm, m;
  // get the current inverse view matrix
  gs->GetMatrix( N_MXM_INVVIEWER, ivm );
  gs->PushMatrix( N_MXM_MODELVIEW );
  // draw chassis
  float lx, ly, lz;
  this->chassisShape->GetSize( &lx, &ly, &lz );
  this->chassisBody->GetTransform( &m );
  m.mult_simple( ivm );
  gs->SetMatrix( N_MXM_MODELVIEW, m );
  prim->EnableLighting( false );
  gs->Rgba( 0, 1, 1, 1 );
  prim->WireBox( lx, ly, lz );
  // draw wheels
  gs->Rgba( 1, 1, 1, 1 );
  for ( int i = 0; i < 4; i++ )
  {
    this->wheelBodies[i]->GetTransform( &m );
    m.mult_simple( ivm );
    gs->SetMatrix( N_MXM_MODELVIEW, m );
    //prim->WireCylinder( this->wheelShapes[i]->GetRadius(), 0.02f, true, 16, 1 );
    prim->SolidCylinder( this->wheelShapes[i]->GetRadius(), 0.02f, true, 16, 1 );
  }
  gs->PopMatrix( N_MXM_MODELVIEW );
}

