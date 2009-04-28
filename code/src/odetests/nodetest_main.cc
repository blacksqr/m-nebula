#define N_IMPLEMENTS nOdeTest
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//------------------------------------------------------------------------------
#include "odetests/nodetest.h"
nNebulaScriptClass(nOdeTest, "nroot");

#include "kernel/ntimeserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nremoteserver.h"
#include "gfx/ngfxserver.h"
#include "input/ninputserver.h"
#include "gfx/nchannelserver.h"
#include "gfx/nscenegraph2.h"
#include "misc/nconserver.h"
#include "misc/nspecialfxserver.h"
#include "odephysics/odephysics.h"
#include "gfx/nprimitiveserver.h"
#include "odetests/nodecar.h"
#include "odetests/nodeground.h"
#include "odetests/nodecamera.h"
#include "odetests/nodehingetest.h"
#include "odetests/nodemeshtest.h"


//------------------------------------------------------------------------------
/**
*/
nOdeTest::nOdeTest()
: ref_GfxServer( kernelServer, this ), ref_OdeServer( kernelServer, this ),
ref_InputServer( kernelServer, this ), ref_SceneGraph( kernelServer, this ),
ref_ConServer( kernelServer, this ), ref_SfxServer( kernelServer, this ),
ref_ChanServer( kernelServer, this ), ref_ScriptServer( kernelServer, this ),
ref_PrimitiveServer( kernelServer, this ), ref_CollSpace( kernelServer, this ),
ref_Lights( kernelServer, this ), grid( false ), stopRequested( false ),
timeChannel( -1 ), lastTime( 0.0 ), testNum( ODE_TEST_NONE ),
ground( 0 ), car( 0 ), hingeTest( 0 ), meshTest( 0 )
{
	this->ref_GfxServer     = "/sys/servers/gfx";
	this->ref_InputServer   = "/sys/servers/input";
	this->ref_SceneGraph    = "/sys/servers/sgraph2";
	this->ref_SfxServer     = "/sys/servers/specialfx";
	this->ref_ConServer     = "/sys/servers/console";
	this->ref_ChanServer    = "/sys/servers/channel";
	this->ref_ScriptServer  = "/sys/servers/script";
	this->ref_OdeServer     = "/sys/servers/ode";
	this->ref_CollSpace     = "/sys/servers/ode/spaces/root";
	this->ref_PrimitiveServer = "/sys/servers/primitive";
	this->ref_Lights        = "/lights";
	this->camera = new nOdeCamera();
	this->camera->SetAngularSpeed( 500 );
	this->camera->SetLinearSpeed( 5 );

	this->kernelServer->GetRemoteServer()->Open("gfxserv");
}

//------------------------------------------------------------------------------
/**
*/
nOdeTest::~nOdeTest()
{
	delete this->camera;
	if ( this->meshTest ) 
		delete this->meshTest;
	if ( this->car )
		delete this->car;
	if ( this->ground )
		delete this->ground;
	if ( this->hingeTest )
		delete this->hingeTest;

	this->Cleanup();
}

//-------------------------------------------------------------------
/**
	@brief Starts the game.

	This method returns when one of the following happens:
	- The rendering window is closed.
	- The 'exit' script command is executed.
	- The 'Stop' method on nOdeTest is called.

	- 22-Mar-03   Vadim Macagon   created
*/
//-------------------------------------------------------------------
void nOdeTest::Start()
{
	this->kernelServer->ts->EnableFrameTime();
	this->kernelServer->ts->ResetTime();

	n_assert( this->ref_ChanServer.isvalid() );

	this->timeChannel = this->ref_ChanServer->GenChannel( "time" );
	this->lastTime = this->kernelServer->ts->GetFrameTime();

	while ( this->Trigger() );

	this->stopRequested = false;

	n_printf( "Test Stopped!" );
}

//-------------------------------------------------------------------
/**
@brief Stops the game.
*/
void nOdeTest::Stop()
{
	this->stopRequested = true;
	this->kernelServer->ts->DisableFrameTime();

	n_printf( "Requested test to stop" );

	this->Cleanup();
}

//-------------------------------------------------------------------
/**
@brief Cleans up the component lists.
*/
void nOdeTest::Cleanup()
{
}

//-------------------------------------------------------------------
/**
	Triggers the following servers:
	- GFX Server
	- Script Server
	- Kernel Server
	- Time Server
	- Input Server

	And then renders a frame.

	- 22-Mar-03   Vadim     created
*/
//-------------------------------------------------------------------
bool nOdeTest::Trigger()
{
	if ( this->stopRequested ) 
	{
		n_printf( "obeying stop request" );
		return false;
	}

	if ( !this->ref_GfxServer->Trigger() ) 
		return false;

	if ( !this->ref_ScriptServer->Trigger() ) 
		return false;

	this->kernelServer->Trigger();

	// trigger the time server
	this->kernelServer->ts->Trigger();

	double time = kernelServer->ts->GetFrameTime();
	float deltaT = float( time - this->lastTime );
	this->lastTime = time;

	this->ref_InputServer->Trigger( time );
	this->HandleInput();

	this->kernelServer->GetRemoteServer()->Trigger();

	// handle other input
	nInputServer* input = this->ref_InputServer.get();
	this->camera->HandleInput( input, deltaT );
	this->SetCameraTransform( this->camera->GetTransform() );

	switch ( this->testNum )
	{
	case ODE_TEST_HINGE:
		this->hingeTest->HandleInput( input );
		this->hingeTest->Update();
		break;
	case ODE_TEST_BUGGY:
		this->car->HandleInput( input );
		this->car->Update();
		break;	  
	case ODE_TEST_MESH:
		this->meshTest->HandleInput( input );
		this->meshTest->Update();
		break;
	}

	//nOdeCollideContext* collContext = ref_OdeServer->GetDefaultCollideContext();
	nOdePhysicsContext* physContext = ref_OdeServer->GetDefaultPhysicsContext();

	int numCollisions = this->ref_CollSpace->Collide();
	if ( numCollisions > 0 )
	{
		switch ( this->testNum )
		{
		case ODE_TEST_BUGGY:
			this->car->Collide();
			break;
		case ODE_TEST_MESH:
			this->meshTest->Collide();
			break;
		}
	}

	physContext->Step( 0.05f );

	this->ref_ChanServer->SetChannel1f( this->timeChannel, (float)time );

	// finally render the frame
	this->RenderFrame( (float)time );

	// flush input events
	this->ref_InputServer->FlushEvents();

	if ( this->sleepTime > 0.0f ) 
		n_sleep( this->sleepTime );

	return true;
}

//-------------------------------------------------------------------
/**
*/
void nOdeTest::HandleInput()
{
	nInputServer* input = this->ref_InputServer.get();
	int newTestNum = ODE_TEST_NONE;
	if ( input->GetButton( "test_hinge" ) )
	{
		newTestNum = ODE_TEST_HINGE;
	}
	else if ( input->GetButton( "test_buggy" ) )
	{
		newTestNum = ODE_TEST_BUGGY;
	}
	else if ( input->GetButton( "test_mesh" ) )
	{
		newTestNum = ODE_TEST_MESH;
	}
	if ( (newTestNum != ODE_TEST_NONE) && (newTestNum != this->testNum) )
	{
		if ( this->testNum != ODE_TEST_NONE )
		{
			// cleanup old test
			switch( this->testNum )
			{
			case ODE_TEST_HINGE:
				delete this->hingeTest;
				this->hingeTest = 0;
				break;
			case ODE_TEST_BUGGY:
				delete this->car;
				this->car = 0;
				delete this->ground;
				this->ground = 0;
				break;
			case ODE_TEST_MESH:
				delete this->meshTest;
				this->meshTest = 0;
				break;
			}
		}
		// setup new test
		switch( newTestNum )
		{
		case ODE_TEST_HINGE:
			this->ref_OdeServer->GetDefaultPhysicsContext()->
				SetGravity( vector3(0, 0, 0) );
			this->hingeTest = new nOdeHingeTest( this->kernelServer );
			break;
		case ODE_TEST_BUGGY:
			this->ref_OdeServer->GetDefaultPhysicsContext()->
				SetGravity( vector3(0, -9.81f, 0) );
			this->car = new nOdeCar( this->kernelServer );
			this->ground = new nOdeGround( this->kernelServer );
			break;
		case ODE_TEST_MESH:
			this->ref_OdeServer->GetDefaultPhysicsContext()->
				SetGravity( vector3(0, -0.1, 0) );
			this->meshTest = new nOdeMeshTest( this->kernelServer );
			break;
		}
		this->testNum = newTestNum;
	}
}

//-------------------------------------------------------------------
/**
@brief Render a single frame.

@param time Current time.

- 22-Mar-03   Vadim     created
*/
//-------------------------------------------------------------------
void nOdeTest::RenderFrame( float time )
{
	n_assert( this->ref_GfxServer.isvalid() );
	nGfxServer* gs = this->ref_GfxServer.get();
	n_assert( this->ref_ConServer.isvalid() );
	nConServer* con = this->ref_ConServer.get();

	// Render
	if ( gs->BeginScene() ) 
	{
		// trigger particle server (needs timestamp)
		/*
		if ( this->ref_ParticleServer.isvalid() ) 
		{
		this->ref_ParticleServer->Trigger();
		}
		*/

		gs->SetMatrix( N_MXM_VIEWER, this->cameraTransform );

		nSpecialFxServer* fx = 0;
		if ( this->ref_SfxServer.isvalid() ) 
			fx = this->ref_SfxServer.get();

		n_assert( this->ref_SceneGraph.isvalid() );
		nSceneGraph2* sg = this->ref_SceneGraph.get();
		if ( sg->BeginScene( this->viewTransform ) )
		{
			// trigger specialfx server
			if ( fx ) 
				fx->Begin();

			// attach the world lights
			if ( this->ref_Lights.isvalid() )
			{
				nVisNode* node = 0;
				node = (nVisNode*)this->ref_Lights->GetHead();
				for( ; node; node = (nVisNode*)node->GetSucc() )
				{
					sg->Attach( node , 0 );
				}
			}

			if ( fx ) 
				fx->End( sg );

			// render the scene
			sg->EndScene( true );
		}

		if ( this->grid )
			this->RenderGrid( gs, this->viewTransform );

		// render collision shapes
		//this->ref_OdeServer->GetDefaultCollideContext()->Visualize( gs );

		nPrimitiveServer* prim = this->ref_PrimitiveServer.get();

		switch ( this->testNum )
		{
		case ODE_TEST_HINGE:
			this->hingeTest->Visualize( gs, prim );
			break;
		case ODE_TEST_BUGGY:
			this->ground->Visualize( gs, prim );
			this->car->Visualize( gs, prim );
			break;
		case ODE_TEST_MESH:
			this->meshTest->Visualize( gs, prim );
			break;
		}

		// test primitive server
		gs->SetMatrix( N_MXM_MODELVIEW, this->viewTransform );
		this->ref_GfxServer->Rgba( 1.0f, 0.0f, 0.0f, 1.0f );
		//prim->WireCone( 1.0f, 2.0f, true, 16, 1 );
		//prim->WireCone( 1.0f, 2.0f, false, 16, 3 );
		//prim->SolidCone( 1.0f, 2.0f, true, 16, 3 );
		//prim->WireCylinder( 1.0f, 2.0f, true, 16, 3 );
		//prim->SolidCylinder( 1.0f, 2.0f, true, 16, 3 );
		//prim->WireCube( 1.0f );
		//prim->WireBox( 1.0f, 2.0f, 0.5f );
		//prim->WireSphere( 1.0f, 16, 16 );
		//prim->SolidSphere( 2.0f, 16, 16 );
		//prim->SolidCapsule( 0.5f, 1.0f, 16, 8 );

		con->Render();                        
		gs->EndScene();
	}
}

//-------------------------------------------------------------------
/**
@brief Render the grid. (OpenGL only for now)

@param gs     Pointer to gfx server.
@param ivwr   Current inverse viewer matrix.

- 18-Sep-02   Vadim   ripped from nObserver
*/
//-------------------------------------------------------------------
void nOdeTest::RenderGrid( nGfxServer* gs, matrix44& ivwr )
{
	gs->SetMatrix( N_MXM_MODELVIEW, ivwr );
	nPrimitiveServer* ps = this->ref_PrimitiveServer.get();
	ps->SetColor( 0.5f, 0.5f, 1.0f, 1.0f );
	ps->EnableLighting( false );
	ps->WirePlane( 25.0f, 1.0f );
}
