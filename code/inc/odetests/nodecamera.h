#ifndef N_ODECAMERA_H
#define N_ODECAMERA_H
//------------------------------------------------------------------------------
/**
	@class nOdeCamera

	@brief Provides user with the ability to control a camera and view the scene
	through it.

	(C) 2003  Vadim Macagon

	nOdeCamera is licensed under the terms of the Nebula License.
*/

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

class nInputServer;
//------------------------------------------------------------------------------
class nOdeCamera
{
public:
    nOdeCamera();
    ~nOdeCamera();
    
    /// set speed at which camera should rotate
    void SetAngularSpeed( float speed );
    /// set speed at which camera should move
    void SetLinearSpeed( float speed );
    
    /// process user input
    void HandleInput( nInputServer* is, float delta );
    /// rotate v1 and v2 about a vector perpendicular to both by some angle
    void RotateAxes( vector3* v1, vector3* v2, float degrees );
    
    const matrix44& GetTransform();
    
    float linearSpeed;
    float angularSpeed;
    float curYangle;
    float curXangle;
    
    matrix44 cameraTransform;
};

//------------------------------------------------------------------------------
inline
void nOdeCamera::SetAngularSpeed( float speed )
{
	this->angularSpeed = speed;
}

//------------------------------------------------------------------------------
inline
void nOdeCamera::SetLinearSpeed( float speed )
{
	this->linearSpeed = speed;
}

//------------------------------------------------------------------------------
inline
const matrix44& nOdeCamera::GetTransform()
{
	return this->cameraTransform;
}

//------------------------------------------------------------------------------
/**
  Assumes v1 and v2 are perpendicular axes and rotates about an axis
  perpedicular to both of them.
*/
inline
void nOdeCamera::RotateAxes( vector3* v1, vector3* v2, float angle )
{
	if( angle < 0.001f  && angle > -0.001f )
		return;

	float a = n_deg2rad( angle );
	float c = n_cos( a );
	float s = n_sin( a );

	vector3 temp( *v1 * c - *v2 * s );
	v2->set( *v1 * s + *v2 * c );
	v2->norm();
	v1->set( temp );
	v1->norm();
}

//------------------------------------------------------------------------------
#endif // N_ODECAMERA_H
