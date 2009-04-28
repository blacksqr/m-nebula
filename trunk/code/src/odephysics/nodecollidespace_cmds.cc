#define N_IMPLEMENTS nOdeCollideSpace
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//------------------------------------------------------------------------------
#include "odephysics/nodecollidespace.h"
#include "odephysics/nodecollideshapenode.h"
#include "odephysics/nodecollidecontext.h"
#include "kernel/nkernelserver.h"

static void n_setcontext( void* slf, nCmd* cmd );
static void n_getcontext( void* slf, nCmd* cmd );
static void n_addshape( void* slf, nCmd* cmd );
static void n_remshape( void* slf, nCmd* cmd );
static void n_remallshapes( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
	@scriptclass
	nodecollidespace

	@superclass
	nroot

	@classinfo
	Base class for various ODE space implementations.

	Spaces can contain other spaces, you can add a space into an existing
	one by creating a new node under the existing space. The nesting hierarchy 
	corresponds directly to the NOH, please note that anything you put under an
	nodecollidespace in the NOH must itself be an nodecollidespace.

	e.g.
	sel /sys/servers/ode/spaces     <- Let's say we put all our spaces here.
	new nodehashspace root        <- Create our top-level space
	sel root
	new nodesimplespace sub1    <- Create a sub-space
	new nodesimplespace sub2    <- Create another one
	sel sub2
	new nodesimplespace sub3  <- And another one
*/
void
n_initcmds( nClass* clazz )
{
	clazz->BeginCmds();
	clazz->AddCmd( "v_setcontext_s", 'SCXT', n_setcontext );
	clazz->AddCmd( "s_getcontext_v", 'GCXT', n_getcontext );
	clazz->AddCmd( "v_addshape_s", 'AS__', n_addshape );
	clazz->AddCmd( "v_remshape_s", 'RS__', n_remshape );
	clazz->AddCmd( "v_remallshapes_v", 'RAS_', n_remallshapes );
	clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
	@cmd
	setcontext

	@input
	s(OdeCollideContext)

	@output
	v

	@info
	Associate an nOdeCollideContext with this space, this must be done in
	order for collision detection to work. CollideContext can be "none"
	to indicate the current association should be removed.
*/
static
void
n_setcontext( void* slf, nCmd* cmd )
{
	nOdeCollideSpace* self = (nOdeCollideSpace*)slf;
	const char* contextName = cmd->In()->GetS();
	if ( strcmp( contextName, "none" ) != 0 )
	{
		nRoot* node = self->kernelServer->Lookup( contextName );
		n_assert( node->IsA( self->kernelServer->FindClass( "nodecollidecontext" ) ) );
		self->SetContext( (nOdeCollideContext*)node );
	}
	else
		self->SetContext( 0 );
}

//------------------------------------------------------------------------------
/**
	@cmd
	getcontext

	@input
	v

	@output
	s(OdeCollideContext)

	@info
	Obtain the nOdeCollideContext currently associated with the space.
	CollideContext may be "none".
*/
static
void
n_getcontext( void* slf, nCmd* cmd )
{
	nOdeCollideSpace* self = (nOdeCollideSpace*)slf;
	nOdeCollideContext* context = self->GetContext();	
	cmd->Out()->SetS( context ? context->GetFullName().c_str() : "none");	
}

//------------------------------------------------------------------------------
/**
	@cmd
	addshape

	@input
	s(OdeCollideShapeNode)

	@output
	v

	@info
	Add a collide shape to the space.

	NOTE: This command is not taken into account by the persistency mechanism,
	cloning or saving & restoring the space will no produce an exact copy because
	no shapes will be added to the copy - it is up to you to add shapes to the
	new/restored copy.
*/
static
void
n_addshape( void* slf, nCmd* cmd )
{
	nOdeCollideSpace* self = (nOdeCollideSpace*)slf;
	nRoot* node = self->kernelServer->Lookup( cmd->In()->GetS() );
	n_assert( node->IsA( self->kernelServer->FindClass( "nodecollideshapenode" ) ) );
	self->AddShape( ((nOdeCollideShapeNode*)node)->GetShape() );
}

//------------------------------------------------------------------------------
/**
	@cmd
	remshape

	@input
	s(OdeCollideShapeNode)

	@output
	v

	@info
	Remove a collide shape from the space.
*/
static
void
n_remshape( void* slf, nCmd* cmd )
{
	nOdeCollideSpace* self = (nOdeCollideSpace*)slf;
	nRoot* node = self->kernelServer->Lookup( cmd->In()->GetS() );
	n_assert( node->IsA( self->kernelServer->FindClass( "nodecollideshapenode" ) ) );
	self->RemShape( ((nOdeCollideShapeNode*)node)->GetShape() );
}

//------------------------------------------------------------------------------
/**
	@cmd
	remallshapes

	@input
	v

	@output
	v

	@info
	Remove all collide shapes from the context.
*/
static
void
n_remallshapes( void* slf, nCmd* )
{
	nOdeCollideSpace* self = (nOdeCollideSpace*)slf;
	self->RemAllShapes();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
