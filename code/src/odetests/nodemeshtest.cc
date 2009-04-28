#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#include "odetests/nodemeshtest.h"

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

//#ifndef N_3DNODE_H
//#include "node/n3dnode.h"
//#endif

nOdeMeshTest::nOdeMeshTest( nKernelServer* ks )
  : ref_CollSpace(ks, 0)
{
  this->odeServer = (nOdeServer*)ks->Lookup( "/sys/servers/ode" );
  this->collContext = odeServer->GetDefaultCollideContext();
  this->physContext = odeServer->GetDefaultPhysicsContext();
  this->ref_CollSpace = "/sys/servers/ode/spaces/root";
      
  this->ground = this->odeServer->NewTriMeshShape( "ground", "meshes:ground.n3d" );
  this->ground->SetCollClass( this->odeServer->QueryCollClass( "ground" ) );
  this->ground->EnableTemporalCoherence( nOdeCollideShape::OST_SPHERE, false );
  this->ground->EnableTemporalCoherence( nOdeCollideShape::OST_BOX, false );
  this->ground->EnableTemporalCoherence( nOdeCollideShape::OST_CAPSULE, false );
  //this->ground->GetTriMesh()->Load( fileServer, "meshes:ground.n3d" );
  this->ref_CollSpace->AddShape( this->ground );
  
  nOdeMass mass;  
  nOdeBoxShape* box;
  nOdeSphereShape* sphere;
  nOdeCapsuleShape* cap;
  nOdeBody* body;
  nOdeCollClass collClass = this->odeServer->QueryCollClass( "blob" );
  
  box = this->odeServer->NewBoxShape();
  box->SetSize( 0.5f, 0.25f, 0.25f );
  box->SetCollClass( collClass );
  this->objects[0].shape = box;
  body = this->physContext->NewOdeBody();
  mass.SetBox( 1, 0.5f, 0.25f, 0.25f );
  mass.AdjustTo( 1 );
  body->SetMass( mass );
  this->objects[0].body = body;
  box->AttachTo( body );
  box->SetPosition( vector3(0.5f, 1.0f, 0.0f) );
  this->ref_CollSpace->AddShape( box );
  //body->Disable();
 
  sphere = this->odeServer->NewSphereShape();
  sphere->SetRadius( 0.2f );
  sphere->SetCollClass( collClass );
  this->objects[1].shape = sphere;
  body = this->physContext->NewOdeBody();
  mass.Reset();
  mass.SetSphere( 1, 0.2f );
  mass.AdjustTo( 1 );
  body->SetMass( mass );
  this->objects[1].body = body;
  sphere->AttachTo( body );
  sphere->SetPosition( vector3(-0.5f, 1.0f, 0.0f) );
  this->ref_CollSpace->AddShape( sphere );
  //body->Disable();
  
  cap = this->odeServer->NewCapsuleShape();
  cap->SetSize( 0.2f, 0.5f );
  cap->SetCollClass( collClass );
  this->objects[2].shape = cap;
  body = this->physContext->NewOdeBody();
  mass.Reset();
  mass.SetCapsule( 1, 0.2f, 0.5f );
  mass.AdjustTo( 1 );
  body->SetMass( mass );
  this->objects[2].body = body;
  cap->AttachTo( body );
  cap->SetPosition( vector3(0.0f, 1.0f, 0.0f) );
  this->ref_CollSpace->AddShape( cap );
  //body->Disable();
  
  //quaternion quat;
  //quat.set_rotate_x( n_rad2deg( 20 ) );
  //this->objects[2].body->SetRotation( quat );
    
  this->surfIndex = this->odeServer->GetSurfaceIndex( "blob" );
}

nOdeMeshTest::~nOdeMeshTest()
{
  for ( int i = 0; i < 3; i++ )
  {
    this->ref_CollSpace->RemShape( this->objects[i].shape );
    this->odeServer->ReleaseOdeShape( this->objects[i].shape );
    this->physContext->ReleaseOdeBody( this->objects[i].body );
  }
  this->ref_CollSpace->RemShape( this->ground );
  this->odeServer->ReleaseOdeShape( this->ground );
}

void nOdeMeshTest::HandleInput( nInputServer* input )
{
  static nOdeBody* selectedBody = 0;
  if ( input->GetButton( "sel_sphere" ) )
    selectedBody = this->objects[1].body;
  else if ( input->GetButton( "sel_capsule" ) )
    selectedBody = this->objects[2].body;
  else if ( input->GetButton( "sel_box" ) )
    selectedBody = this->objects[0].body;
  
  if ( !selectedBody )
    return;
  
  vector3 curVelocity;
  selectedBody->GetLinearVel( &curVelocity );
    
  if ( input->GetButton( "speed_up" ) )
  {
    vector3 velocityChange( 0, 0, 0.2f );
    curVelocity += velocityChange;
    selectedBody->SetLinearVel( curVelocity );
  }
  else if ( input->GetButton( "slow_down" ) )
  {
    vector3 velocityChange( 0, 0, 0.2f );
    curVelocity -= velocityChange;
    selectedBody->SetLinearVel( curVelocity );
  }
  else if ( input->GetButton( "steer_left" ) )
  {
    vector3 velocityChange( 0.2f, 0, 0 );
    curVelocity -= velocityChange;
    selectedBody->SetLinearVel( curVelocity );
  }
  else if ( input->GetButton( "steer_right" ) )
  {
    vector3 velocityChange( 0.2f, 0, 0 );
    curVelocity += velocityChange;
    selectedBody->SetLinearVel( curVelocity );
  }
  else if ( input->GetButton( "resetbuggy" ) )
  {
    selectedBody->SetForce( vector3(0, 0, 0) );
    selectedBody->SetTorque( vector3(0, 0, 0) );
    selectedBody->SetLinearVel( vector3(0, 0, 0) );
    selectedBody->SetAngularVel( vector3(0, 0, 0) );
  }
}

void nOdeMeshTest::Update()
{
}

void nOdeMeshTest::Collide()
{
  int i, j, numColls;
  nOdeCollideReport** report;
  nOdeCollideContact* contact;
  
  for ( int k = 0; k < 3; k++ )
  {
    numColls = this->collContext->GetCollissionReports( this->objects[k].shape, 
                                                        report ); 
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

void nOdeMeshTest::Visualize( nGfxServer* gs, nPrimitiveServer* prim )
{
  matrix44 ivm, m;
  // get the current inverse view matrix
  gs->GetMatrix( N_MXM_INVVIEWER, ivm );
  gs->PushMatrix( N_MXM_MODELVIEW );
  prim->EnableLighting( false );
  gs->Rgba( 0, 1, 0, 1 );
  this->ground->VisualizeLocal( gs, prim );
  gs->Rgba( 0, 1, 1, 1 );
  for ( int i = 0; i < 3; i++ )
  {
    this->objects[i].body->GetTransform( &m );
    m.mult_simple( ivm );
    gs->SetMatrix( N_MXM_MODELVIEW, m );
    this->objects[i].shape->VisualizeLocal( gs, prim );
  }
  gs->PopMatrix( N_MXM_MODELVIEW );
}

