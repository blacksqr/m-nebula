#define N_IMPLEMENTS nOdeJointNode
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//
//  nOdeJointNode is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "odephysics/nodejointnode.h"

nNebulaScriptClass(nOdeJointNode, "nroot")

#include "odephysics/nodephysicscontext.h"
#include "odephysics/nodejoint.h"
#include "odephysics/nodebodynode.h"

//------------------------------------------------------------------------------
/**
*/
nOdeJointNode::nOdeJointNode()
: ref_PhysContext(kernelServer, this),
joint(0),
autoClean(true)
{
}

//------------------------------------------------------------------------------
/**
*/
nOdeJointNode::~nOdeJointNode()
{
	if ( this->autoClean )
	{
		n_assert( this->joint );
		n_assert( this->ref_PhysContext.isvalid() );
		this->ref_PhysContext->ReleaseOdeJoint( this->joint );
		this->joint = 0;
	}
}

//------------------------------------------------------------------------------
/**
	@brief Create the underlying nOdeJoint instance.
	This obviously must be called before trying to do anything else with
	the joint.
*/
void nOdeJointNode::InitJoint( const char* physContext )
{
	n_warn( "nOdeJointNode::InitJoint(): Pure virtual method called!" );
}

//------------------------------------------------------------------------------
/**
	@brief Enable/disable auto cleanup mode (enabled by default).
	If auto cleanup is enabled the underlying nOdeJoint instance will be released
	when the node is destroyed, otherwise it is up to you to release the nOdeJoint
	instance.
*/
void
nOdeJointNode::SetAutoCleanup( bool cleanup )
{
	this->autoClean = cleanup;
}

//------------------------------------------------------------------------------
/**
*/
bool
nOdeJointNode::GetAutoCleanup()
{
	return this->autoClean;
}


//------------------------------------------------------------------------------
/**
	@brief Obtain the underlying nOdeJoint instance.
	@warning If you disable auto cleanup YOU are responsible for releasing the
	nOdeJoint instance you get from this method.
*/
nOdeJoint* nOdeJointNode::GetJoint() const
{
	n_assert( this->joint );
	return joint;
}

#ifdef ODE_BREAKABLE_JOINTS

//------------------------------------------------------------------------------
void nOdeJointNode::SetBreakModeFlag( const char* flagName, bool enable )
{
	int flag = 0;
	if ( strcmp( "b1force", flagName ) == 0 )
		flag = dJOINT_BREAK_AT_B1_FORCE;
	else if ( strcmp( "b1torque", flagName ) == 0 )
		flag = dJOINT_BREAK_AT_B1_TORQUE;
	else if ( strcmp( "b2force", flagName ) == 0 )
		flag = dJOINT_BREAK_AT_B2_FORCE;
	else if ( strcmp( "b2torque", flagName ) == 0 )
		flag = dJOINT_BREAK_AT_B2_TORQUE;
	else
	{
		n_error( "Unknown flag: %s", flagName );
		return;
	}

	this->joint->SetBreakModeFlag( flag, enable );
}

//------------------------------------------------------------------------------
bool nOdeJointNode::GetBreakModeFlag( const char* flag )
{
	int flag = 0;
	if ( strcmp( "b1force", flagName ) == 0 )
		flag = dJOINT_BREAK_AT_B1_FORCE;
	else if ( strcmp( "b1torque", flagName ) == 0 )
		flag = dJOINT_BREAK_AT_B1_TORQUE;
	else if ( strcmp( "b2force", flagName ) == 0 )
		flag = dJOINT_BREAK_AT_B2_FORCE;
	else if ( strcmp( "b2torque", flagName ) == 0 )
		flag = dJOINT_BREAK_AT_B2_TORQUE;
	else
	{
		n_error( "Unknown flag: %s", flagName );
		return false;
	}

	return this->joint->GetBreakModeFlag( flag );
}

#endif // ODE_BREAKABLE_JOINTS

//------------------------------------------------------------------------------
/**
*/
void nOdeJointNode::AttachTo( const char* b1, const char* b2 )
{
	n_assert( this->joint );
	nOdeBody* ob1 = 0;
	nOdeBody* ob2 = 0;

	if ( strcmp( b1, "none" ) != 0 )
	{
		nRoot* node = this->kernelServer->Lookup( b1 );
		n_assert( node && node->IsA( this->kernelServer->FindClass( "nodebodynode" ) ) );
		ob1 = ((nOdeBodyNode*)node)->GetBody();
	}

	this->bodyPath1 = b1;

	if ( strcmp( b2, "none" ) != 0 )
	{
		nRoot* node = this->kernelServer->Lookup( b2 );
		n_assert( node && node->IsA( this->kernelServer->FindClass( "nodebodynode" ) ) );
		ob2 = ((nOdeBodyNode*)node)->GetBody();
	}

	this->bodyPath2 = b2;

	n_assert( (ob1 || ob2) && "At least one of the bodies must be valid!" );

	this->joint->AttachTo( ob1, ob2 );
}

//------------------------------------------------------------------------------
/**
*/
const char* nOdeJointNode::GetType()
{
	n_assert( this->joint );

	switch ( this->joint->GetType() )
	{
	case dJointTypeBall:
		return "ball";
	case dJointTypeHinge:
		return "hinge";
	case dJointTypeSlider:
		return "slider";
	case dJointTypeContact:
		return "contact";
	case dJointTypeUniversal:
		return "universal";
	case dJointTypeHinge2:
		return "hinge2";
	case dJointTypeFixed:
		return "fixed";
	case dJointTypeAMotor:
		return "amotor";
	default:
		return "uknown";
	}
}

//------------------------------------------------------------------------------
/**
*/
const char* nOdeJointNode::GetBody( int index )
{
	n_assert( this->joint );
	n_assert( (0 == index) || (1 == index) );

	if ( 0 == this->joint->GetBody( index ) )
		return "none";

	if ( 0 == index )
		return this->bodyPath1.c_str();
	else
		return this->bodyPath2.c_str();
}

//------------------------------------------------------------------------------
/**
*/
void nOdeJointNode::SetParam( const char* param, int axis, float value )
{
	n_assert( this->joint );
	n_assert( (0 <= axis) && (axis < 3) );

	if ( strcmp( param, "lostop" ) == 0 )
		this->joint->SetParam( dParamLoStop + dParamGroup * axis, value );
	else if ( strcmp( param, "histop" ) == 0 )
		this->joint->SetParam( dParamHiStop + dParamGroup * axis, value );
	else if ( strcmp( param, "vel" ) == 0 )
		this->joint->SetParam( dParamVel + dParamGroup * axis, value );
	else if ( strcmp( param, "fmax" ) == 0 )
		this->joint->SetParam( dParamFMax + dParamGroup * axis, value );
	else if ( strcmp( param, "fudgefactor" ) == 0 )
		this->joint->SetParam( dParamFudgeFactor + dParamGroup * axis, value );
	else if ( strcmp( param, "bounce" ) == 0 )
		this->joint->SetParam( dParamBounce + dParamGroup * axis, value );
	else if ( strcmp( param, "cfm" ) == 0 )
		this->joint->SetParam( dParamCFM + dParamGroup * axis, value );
	else if ( strcmp( param, "stoperp" ) == 0 )
		this->joint->SetParam( dParamStopERP + dParamGroup * axis, value );
	else if ( strcmp( param, "stopcfm" ) == 0 )
		this->joint->SetParam( dParamStopCFM + dParamGroup * axis, value );
	else if ( strcmp( param, "suspensionerp" ) == 0 )
		this->joint->SetParam( dParamSuspensionERP + dParamGroup * axis, value );
	else if ( strcmp( param, "suspensioncfm" ) == 0 )
		this->joint->SetParam( dParamSuspensionCFM + dParamGroup * axis, value );
	else
		n_warn( "nOdeJointNode::SetParam(): Unknown param!" );
}

//------------------------------------------------------------------------------
/**
*/
float nOdeJointNode::GetParam( const char* param, int axis )
{
	n_assert( this->joint );
	n_assert( (0 <= axis) && (axis < 3) );

	if ( strcmp( param, "lostop" ) == 0 )
		return this->joint->GetParam( dParamLoStop + dParamGroup * axis );
	else if ( strcmp( param, "histop" ) == 0 )
		return this->joint->GetParam( dParamHiStop + dParamGroup * axis );
	else if ( strcmp( param, "vel" ) == 0 )
		return this->joint->GetParam( dParamVel + dParamGroup * axis );
	else if ( strcmp( param, "fmax" ) == 0 )
		return this->joint->GetParam( dParamFMax + dParamGroup * axis );
	else if ( strcmp( param, "fudgefactor" ) == 0 )
		return this->joint->GetParam( dParamFudgeFactor + dParamGroup * axis );
	else if ( strcmp( param, "bounce" ) == 0 )
		return this->joint->GetParam( dParamBounce + dParamGroup * axis );
	else if ( strcmp( param, "cfm" ) == 0 )
		return this->joint->GetParam( dParamCFM + dParamGroup * axis );
	else if ( strcmp( param, "stoperp" ) == 0 )
		return this->joint->GetParam( dParamStopERP + dParamGroup * axis );
	else if ( strcmp( param, "stopcfm" ) == 0 )
		return this->joint->GetParam( dParamStopCFM + dParamGroup * axis );
	else if ( strcmp( param, "suspensionerp" ) == 0 )
		return this->joint->GetParam( dParamSuspensionERP + dParamGroup * axis );
	else if ( strcmp( param, "suspensioncfm" ) == 0 )
		return this->joint->GetParam( dParamSuspensionCFM + dParamGroup * axis );
	else
		n_warn( "nOdeJointNode::SetParam(): Unknown param!" );

	return 0.0f;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
