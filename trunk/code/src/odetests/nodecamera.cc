#define N_IMPLEMENTS nOdeCamera
//------------------------------------------------------------------------------
//  (C) 2003  Vadim Macagon
//------------------------------------------------------------------------------
#include "odetests/nodecamera.h"

#ifndef N_INPUTSERVER_H
#include "input/ninputserver.h"
#endif

//------------------------------------------------------------------------------
nOdeCamera::nOdeCamera()
: linearSpeed( 0.0f ), angularSpeed( 0.0f ), curXangle( 0.0f ), curYangle( 0.0f )
{
	this->cameraTransform.ident();
}

//------------------------------------------------------------------------------    
nOdeCamera::~nOdeCamera()
{
}

//------------------------------------------------------------------------------
/**
	@brief Process user input.
*/
void nOdeCamera::HandleInput( nInputServer* is, float deltaT )
{
	float dry = is->GetSlider( "cam_turn_right" ) - is->GetSlider( "cam_turn_left" );
	float drx = is->GetSlider( "cam_look_up" ) - is->GetSlider( "cam_look_down" );
	float dz = is->GetButton( "cam_forward" ) - is->GetButton( "cam_back" );
	float dy = is->GetButton( "cam_up" ) - is->GetButton( "cam_down" );
	float dx = is->GetButton( "cam_right" ) - is->GetButton( "cam_left" );

	float dist = deltaT * this->linearSpeed;
	float angle = deltaT * this->angularSpeed;

	if ( dry < -0.001f || dry > 0.001f )
	{
		this->curYangle += dry * angle;
		if ( this->curYangle >= 360.0f )
			this->curYangle -= 360.0f;
		else if ( this->curYangle < 0.0f )
			this->curYangle += 360.0f;
	}

	if ( drx < -0.001f || drx > 0.001f )
	{
		this->curXangle += drx * angle;
		if ( this->curXangle < -90.0f )
			this->curXangle = -90.0f;
		else if ( this->curXangle > 90.0f )
			this->curXangle = 90.0f;
	}

	vector3 camU( 1.0f, 0.0f, 0.0f );
	vector3 camV( 0.0f, 1.0f, 0.0f );
	vector3 camN( 0.0f, 0.0f, 1.0f );
	static vector3 worldUp( 0.0f, 1.0f, 0.0f );

	this->RotateAxes( &camN, &camU, this->curYangle );
	this->RotateAxes( &camV, &camN, this->curXangle );

	vector3 pos = this->cameraTransform.pos_component();
	// compute new position
	pos += camU * (dx * dist) + worldUp * (dy * dist) + camN * -(dz * dist);
	// update matrix
	this->cameraTransform.set( camU.x, camU.y, camU.z, 0.0f,
		camV.x, camV.y, camV.z, 0.0f,
		camN.x, camN.y, camN.z, 0.0f,
		pos.x, pos.y, pos.z, 1.0f );
}
