#include "odetests/nodeground.h"
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//------------------------------------------------------------------------------
#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_ODEPHYSICS_H
#include "odephysics/odephysics.h"
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

//------------------------------------------------------------------------------
nOdeGround::nOdeGround( nKernelServer* ks )
: ref_CollSpace( ks, 0 )
{
	this->odeServer = (nOdeServer*)ks->Lookup( "/sys/servers/ode" );
	this->collContext = odeServer->GetDefaultCollideContext();
	this->physContext = odeServer->GetDefaultPhysicsContext();
	this->ref_CollSpace = "/sys/servers/ode/spaces/root";

	this->groundBox = odeServer->NewBoxShape();
	this->groundBox->SetSize( 2.0f, 1.5f, 1.0f );
	this->groundBox->SetPosition( vector3(2.0f, -0.7f, 0) );
	matrix33 boxRot;
	boxRot.rotate( vector3(0, 0, 1), -0.15f );
	this->groundBox->SetRotation( boxRot );
	this->groundBox->SetCollClass( odeServer->QueryCollClass( "ground" ) );
	this->groundPlane = odeServer->NewPlaneShape();
	this->groundPlane->SetParams( vector3(0, 1, 0), 0 );
	this->groundPlane->SetCollClass( odeServer->QueryCollClass( "ground" ) );
	this->ref_CollSpace->AddShape( this->groundBox );
	this->ref_CollSpace->AddShape( this->groundPlane );
}

nOdeGround::~nOdeGround()
{
	this->ref_CollSpace->RemShape( this->groundBox );
	this->odeServer->ReleaseOdeShape( this->groundBox );
	this->ref_CollSpace->RemShape( this->groundPlane );
	this->odeServer->ReleaseOdeShape( this->groundPlane );
}

void nOdeGround::Visualize( nGfxServer* gs, nPrimitiveServer* prim )
{
	matrix44 ivm, m;
	// get the current inverse view matrix
	gs->GetMatrix( N_MXM_INVVIEWER, ivm );
	gs->PushMatrix( N_MXM_MODELVIEW );
	prim->EnableLighting( true );
	gs->Rgba( 0, 0.8f, 0, 1 );
	//m.rotate_x( PI * 0.5f );
	//m.mult_simple( ivm );
	//gs->SetMatrix( N_MXM_MODELVIEW, m );
	gs->SetMatrix( N_MXM_MODELVIEW, ivm );
	// render plane
	//prim->WirePlane();
	prim->SolidPlane();
	this->groundBox->GetTransform( &m );
	m.mult_simple( ivm );
	gs->SetMatrix( N_MXM_MODELVIEW, m );
	float lx, ly, lz;
	this->groundBox->GetSize( &lx, &ly, &lz );
	prim->EnableLighting( false );
	gs->Rgba( 0, 1, 0, 1 );
	// render box
	prim->WireBox( lx, ly, lz );
	gs->PopMatrix( N_MXM_MODELVIEW );
}

