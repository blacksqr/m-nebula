#define N_IMPLEMENTS nOdeNode
//==============================================================================
//  misc/nOdeNode_main.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/nodenode.h"
#include "node/n3dnode.h"
#include "gfx/nscenegraph2.h"
#include "gfx/ngfxserver.h"
#include "node/nmeshnode.h"

#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"

#include "collide/ncollideserver.h"
#include "collide/ncollideshape.h"
#include "collide/ncollidereporthandler.h"

#include "nemesis/node_physics.h"
#include "nemesis/node_collideshapes.h"
#include "nemesis/node_collideobject.h"
#include "nemesis/node_collidecontext.h"

#include "nemesis/node_ball_joint.h"
#include "nemesis/node_hinge_joint.h"
#include "nemesis/node_hinge2_joint.h"
#include "nemesis/node_slider_joint.h"

#include "misc/nrsrcpath.h"
#include "mathlib/plane2.h"

#include "kernel/nfileserver2.h"

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
nOdeNode::nOdeNode() :
    ref_pz(pKernelServer, this),
    ref_sg(pKernelServer, this),
    ref_shadow(pKernelServer, this),
    ref_gs(pKernelServer, this),
	ref_fs2(pKernelServer, this),
    myCollideObject(NULL),
    myCollideShape(NULL),
    myCollideContext(NULL),
    myPhysicsBodyID(-1),
    myShapeType( nOdePhysics::ODE_SPHERE ),
    myRadius(0),
    myHeight(0),
    myReference( NULL ),
    myJointsNeedConnecting( false )
{
    ref_pz = "/sys/servers/physics";
    ref_gs = "/sys/servers/gfx";
    ref_sg = "/sys/servers/sgraph";
    ref_shadow = "/sys/servers/shadow";
	ref_fs2 = "/sys/servers/file2";

    myFileName[0] = '\0';
}

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
nOdeNode::~nOdeNode()
{
    if (dependNode.IsLinked())
    {
        dependNode.Remove();
    }

    for( vector<JointContainer *>::iterator i = myJoints.begin(); i != myJoints.end(); i++)
    {
        JointContainer *jc = *(i);
        delete jc;
    }

    if (ref_pz.isvalid())
    {
        myCollideContext->ReleaseObject( myCollideObject );
        ref_pz->ReleaseShape( myCollideShape );
    }
}

void nOdeNode::MakeSphere( float radius )
{
    myRadius = radius;
    myShapeType = nOdePhysics::ODE_SPHERE;

    setCollisionData( 0 );
}

void nOdeNode::MakeCylinder( float radius, float height )
{
    myRadius    = radius + (height * .5);
    myCylRadius = radius;
    myHeight    = height;

    myShapeType = nOdePhysics::ODE_CYLINDER;
    setCollisionData( 0 );
}

void nOdeNode::MakeBox( float minx, float miny, float minz, float maxx, float maxy, float maxz )
{
    myBox.vmin.set( minx, miny, minz );
    myBox.vmax.set( maxx, maxy, maxz );

    myShapeType = nOdePhysics::ODE_BOX;
    vector3 test( vector3( minx, miny, minz) - vector3( maxx, maxy, maxz) );

    myRadius = test.len() * .5;

    setCollisionData( 0 );
}

void nOdeNode::MakePlane( float a, float b, float c, float d)
{
    myPlane = new Plane( vector3(a,b,c), d);
    myShapeType = nOdePhysics::ODE_PLANE;

    // Largest number imaginable..... Except d3d8 barfs on it 
    // so we'll use the clip max :)  This only controls the
    // Nebulized bounding volume visualization (radius/box)
    // since a Plane has no true radius
    myRadius = ref_gs->clip_maxz;
    setCollisionData( 0 );
}

void nOdeNode::setCollisionLink( nVisNode* linkTo, nOdePhysics::nOdeTypes shape )
{
    myReference = linkTo;
    myShapeType = shape;

    if (setCollisionData( -1 ))
    {
        myReference = NULL;
    }
}

void nOdeNode::setCollisionFile( const char* linkTo, nOdePhysics::nOdeTypes shape )
{
    n_strncpy2(myFileName, linkTo, sizeof(myFileName));
    myShapeType = shape;

    if (setCollisionData( -1 ))
    {
        myReference = NULL;
    }
}

//==============================================================================
//  2001.04.07  WhiteGold       created
//------------------------------------------------------------------------------
void nOdeNode::setCollisionClass( const char *coll_type )
{
    myCollideObject->SetCollClass(ref_pz->QueryCollClass(coll_type));
}

//-------------------------------------------------------------------
//  Attach()
//   Enforces dependency linkage, as well as renders the bounding 
//   volumes
//-------------------------------------------------------------------
bool nOdeNode::Attach(nSceneGraph2 *sg)
{
    // Now be sure we are a dependant of our n3dnode parent
    // so Compute() can properly update the transformation
    // matrixes with information from ODE
    if (!dependNode.IsLinked() && myCollideObject )
    {
        nRoot *parent = this->GetParent();
        if(parent && parent->IsA(pKernelServer->FindClass("n3dnode")))
        {
            myParent3DNode = (n3DNode *)parent;
            myParent3DNode->DependsOn( this );

            // Set initial transform for the nOdeNode's collision info
            matrix44 pmat( myParent3DNode->GetM() );
            myCollideObject->Transform( 0.0, pmat );
            lastt.set( pmat );
        }
    }

    return( true );
}

//-------------------------------------------------------------------
//  Compute()
//   This is a dependency node for the physics engine.
//   the parent n3DNode will force a call on the Compute()
//   of this class before updating it's internal matrices.
//-------------------------------------------------------------------
void nOdeNode::Compute(nSceneGraph2 *sg)
{
    // Hand compute to our childer
    nVisNode::Compute( sg );

    if (myCollideObject && !myReference)
    {
        // 1) Get the current ODE physics position of this object
        matrix44 trans( myCollideObject->GetTransform() );
        matrix44 pmat( myParent3DNode->GetM() );
        quaternion cq( pmat.get_quaternion() );
        quaternion lq( lastt.get_quaternion() );            

        // 1.a) Check to see if our parent matrix has been changed 
        //      since last call - if so we need to reset our collision 
        //      ODE matrix to sync up

        if (lastt.M41 != pmat.M41 || lastt.M42 != pmat.M42 || lastt.M43 != pmat.M43 ||
            !(cq == lq) )
        {
            myCollideObject->Transform(0.0, pmat );
            matrix44 DEBUG_ME( myCollideObject->GetTransform() );
            lastt.set( pmat );
            debugt.set( pmat );
        } else {

            // 2) Set the rotation/transform on our linked parent 
            quaternion q( trans.get_quaternion() );         
            myParent3DNode->Qxyzw( q.x, q.y, q.z, q.w );
            myParent3DNode->Txyz( trans.M41, trans.M42, trans.M43);
            lastt.set( myParent3DNode->GetM() );

            // And finally hand the right transform back to the 
            // base nCollideObject.. If I knew matrix math better
            // I would do it at GetTransform() time.... bleah
            //
            // Oddly enough there MUST be a bug somewhere that I 
            // don't see.. This matrix I'm setting here is nearly 
            // identical to the matrix GetTransform() returns, yet
            // if I set the Transform() in GetTransform() the 
            // bounding volume doesn't render.. If I do it here, 
            // it does... GO FIGGER..
            myCollideObject->nCollideObject::Transform(nOdePhysics::pKernelServer->ts->GetTime(), lastt );
        }
    }
}

// Subclasses may choose another method of rendering.
// we already define WireframeCylinder and WireframeSphere
//
// TODO:: need to tweak this when we get our origins straight
void nOdeNode::renderCollisionBoundingVolume( matrix44 &vm )
{
    nRState rs;
    rs.Set(N_RS_TEXTUREHANDLE,0);     ref_gs->SetState(rs);
    rs.Set(N_RS_LIGHTING, N_FALSE);   ref_gs->SetState(rs);

    if (myCollideObject->GetNumCollissions()) {
        ref_gs->Rgba(0.0f,0.7f,0.0f,1.0f);
    }
    else {
        ref_gs->Rgba(0.0f,0.0f,1.0f,1.0f);
    }

    vector3 pos = vm.pos_component();
    WireframeSphere(
        pos,
        myCollideShape->GetRadius()
    ).render(ref_gs.get());

    pos.y -= myCollideShape->GetRadius();
    WireframeCylinder(
        pos,
        myCollideShape->GetRadius(),
        2 * myCollideShape->GetRadius()
    ).render(ref_gs.get());

}

// Get the mesh, find it's index buffer, wind through
// the mesh handing the triangles to the collision shape
//
// returns FALSE if it couldn't realize the shape this
// frame.  Try again next frame
bool nOdeNode::setCollisionData( int type )
{
    if (myCollideObject == NULL && ref_pz.isvalid() ) 
    {
        myCollideContext = (nOdeCollideContext *)ref_pz->GetDefaultContext();
        myCollideObject  = (nOdeCollideObject *)myCollideContext->NewObject();
        myCollideObject->SetCollClass(ref_pz->QueryCollClass("bird"));
        myCollideObject->SetClientData(this);
    }
    bool staticShape = false;

    // Burp.  nMeshNode is all TRI-LISTS (hopefully)
    // nPMeshNodes are tri-STRIPS (maybe)
    nClass *meshClass = pKernelServer->FindClass("nmeshnode");
    if (myReference && myReference->IsA(meshClass) || myFileName[0] != '\0') 
    {
        // The name we'll ultimately give the shape
        char name_buf[N_MAXPATH];
        name_buf[0] = '\0';

        if (myReference)
        {
            nMeshNode *mesh = (nMeshNode *)(myReference);
            nVertexBuffer *vb = mesh->GetVertexBuffer();
	        n_strncpy2(name_buf,vb->GetName(),sizeof(name_buf)-1);

        } else // myFileName[0] != '\0'
        {
            n_strncpy2(name_buf,myFileName,sizeof(name_buf)-1);
        }

        // Append the shape type to the buffer
        // multiple meshes may want different shapes :)
        switch( myShapeType )
        {
            case nOdePhysics::ODE_BOX:
                strcat( name_buf, "b" );
                break;
            case nOdePhysics::ODE_CYLINDER:
                strcat( name_buf, "c" );
                break;
            case nOdePhysics::ODE_PLANE:
                strcat( name_buf, "p" );
                break;
            case nOdePhysics::ODE_SPHERE:
                strcat( name_buf, "s" );
                break;
            case nOdePhysics::ODE_TRILIST:
                strcat( name_buf, "t" );
                break;
        }
        myCollideShape = ref_pz->NewShape( name_buf, myShapeType );
        if (!myCollideShape->IsInitialized()) 
        {

			myCollideShape->Load( ref_fs2.get(), myFileName );
        }
	} else { // No reference to work from. This is a standalone thing
        myCollideShape = ref_pz->NewShape( this->GetName() , this->myShapeType );
        nOdeSphereCollideShape *sh = (nOdeSphereCollideShape *)myCollideShape;
        switch( myShapeType )
        {
            case nOdePhysics::ODE_CYLINDER:
                ((nOdeCylinderCollideShape *)myCollideShape)->setBoundingCylinder( myCylRadius, myHeight, 1 );
                break;
            case nOdePhysics::ODE_BOX:
                ((nOdeBoxCollideShape *)myCollideShape)->setBoundingBox( myBox.vmax.x - myBox.vmin.x,
                                                                         myBox.vmax.y - myBox.vmin.y,
                                                                         myBox.vmax.z - myBox.vmin.z);
                break;
            case nOdePhysics::ODE_PLANE:
                ((nOdePlaneCollideShape *)myCollideShape)->setPlane( *myPlane );
                staticShape = true;
                break;
            case nOdePhysics::ODE_SPHERE:
		{
                sh->Begin(1,0);
		vector3 v = vector3(0.0f, myRadius, 0.0f);
                sh->SetVertex(0, v );
		}
                break;
        }
    }

    myCollideObject->SetShape(myCollideShape);
    myCollideObject->SetRadius( myCollideShape->GetRadius() );
    myCollideObject->setStatic( staticShape );

    myCollideContext->AddObject(myCollideObject);

    if (!dependNode.IsLinked())
    {
        nRoot *parent = this->GetParent();
        if(parent && parent->IsA(pKernelServer->FindClass("n3dnode")))
        {
            myParent3DNode = (n3DNode *)parent;
            myParent3DNode->DependsOn( this );

            // Set initial transform for the nOdeNode's collision info
            matrix44 pmat( myParent3DNode->GetM() );
            myCollideObject->Transform( 0.0, pmat );
            lastt.set( pmat );
        }
    }

    return( true );
}

//  Unimplemented
void nOdeNode::SetCollideable( bool collide )
{ 
    collidable = collide; 
};

void nOdeNode::SetPhysicsable( bool physit  )
{ 
    physicsable = physit;
    ((nOdeCollideObject *)myCollideObject)->setStatic( physit );
    lastt.ident();
};

const char *nOdeNode::connectWithBallJoint( const char *otherPath, vector3 *anchor )
{
    static int xxx = 0;

    nRoot *temp = pKernelServer->Lookup( otherPath );
    nOdeNode *other = NULL;
    nOdeCollideObject *co2 = NULL;

    char buf[255];
    
    buf[0] = '\0';
    sprintf( buf, "/lib/joints/b%d", xxx++ );

    if (temp && temp->IsA( nOdeNode::pClass ) )
    {
        other = (nOdeNode *)temp;
        co2 = other->getCollideObject();
    }

    nOdeBallJoint *bj = (nOdeBallJoint *) pKernelServer->New( "node_ball_joint", buf );
    bj->setAnchor( anchor );
    bj->connectJointToObjects((nOdeCollideObject *)myCollideObject, co2);

    JointContainer *jc = new JointContainer;
    jc->joint= bj;
    jc->connected = true;
    jc->otherObject = co2;

    myJoints.push_back(jc);

    return( bj->GetName() );
}

const char *nOdeNode::connectWithSliderJoint( const char *otherPath, vector3 *axis )
{
    static int xxx = 0;

    nRoot *temp = pKernelServer->Lookup( otherPath );
    nOdeNode *other = NULL;
    nOdeCollideObject *co2 = NULL;

    char buf[255];
    
    buf[0] = '\0';
    sprintf( buf, "/lib/joints/sl%d", xxx++ );

    if (temp && temp->IsA( nOdeNode::pClass ) )
    {
        other = (nOdeNode *)temp;
        co2 = other->getCollideObject();
    }

    nOdeSliderJoint *bj = (nOdeSliderJoint *) pKernelServer->New( "node_ball_joint", buf );
    bj->setAxis( axis );
    bj->connectJointToObjects((nOdeCollideObject *)myCollideObject, co2);

    JointContainer *jc = new JointContainer;
    jc->joint= bj;
    jc->connected = true;
    jc->otherObject = co2;

    myJoints.push_back(jc);

    return( bj->GetName() );
}

const char *nOdeNode::connectWithHingeJoint( const char *otherPath, vector3 *anchor, vector3 *axis)
{
    static int xxx = 0;

    nRoot *temp = pKernelServer->Lookup( otherPath );
    nOdeNode *other = NULL;
    nOdeCollideObject *co2 = NULL;

    char buf[255];
    
    buf[0] = '\0';
    sprintf( buf, "/lib/joints/h%d", xxx++ );

    if (temp && temp->IsA( nOdeNode::pClass ) )
    {
        other = (nOdeNode *)temp;
        co2 = other->getCollideObject();
    }

    nOdeHingeJoint *hj = (nOdeHingeJoint *) pKernelServer->New( "node_hinge_joint", buf );
    hj->setAnchor( anchor );
    hj->setAxis( axis );
    hj->connectJointToObjects((nOdeCollideObject *)myCollideObject, co2);

    JointContainer *jc = new JointContainer;
    jc->joint= hj;
    jc->connected = true;
    jc->otherObject = co2;

    myJoints.push_back(jc);

    return( hj->GetName() );

}

const char *nOdeNode::connectWithHinge2Joint( const char *otherPath, vector3 *anchor, vector3 *axis, vector3 *axis2)
{
    static int xxx = 0;

    nRoot *temp = pKernelServer->Lookup( otherPath );
    nOdeNode *other = NULL;
    nOdeCollideObject *co2 = NULL;

    char buf[255];
    
    buf[0] = '\0';
    sprintf( buf, "/lib/joints/hh%d", xxx++ );

    if (temp && temp->IsA( nOdeNode::pClass ) )
    {
        other = (nOdeNode *)temp;
        co2 = other->getCollideObject();
    }

    nOdeHinge2Joint *hj = (nOdeHinge2Joint *) pKernelServer->New( "node_hinge2_joint", buf );
    hj->setAnchor( anchor );
    hj->setAxis( axis );
    hj->setAxis2( axis2 );
    hj->connectJointToObjects((nOdeCollideObject *)myCollideObject, co2);

    JointContainer *jc = new JointContainer;
    jc->joint= hj;
    jc->connected = true;
    jc->otherObject = co2;

    myJoints.push_back(jc);

    return( hj->GetName() );
}

const char *nOdeNode::connectWithFixedJoint( const char *otherPath )
{
    static int xxx = 0;
    char buf[255];
    
    buf[0] = '\0';
    sprintf( buf, "/lib/joints/f%d", xxx++ );

    nRoot *temp = pKernelServer->Lookup( otherPath );
    nOdeNode *other = NULL;
    nOdeCollideObject *co2 = NULL;

    if (temp && temp->IsA( nOdeNode::pClass ) )
    {
        other = (nOdeNode *)temp;
        co2 = other->getCollideObject();
    }

    nOdeJoint *bj = (nOdeJoint *)pKernelServer->New( "node_joint", buf);
    bj->connectJointToObjects((nOdeCollideObject *)myCollideObject, co2);

    JointContainer *jc = new JointContainer;
    jc->joint= bj;
    jc->connected = true;
    jc->otherObject = co2;

    myJoints.push_back(jc);

    return( bj->GetName() );
}

void nOdeNode::addForce( vector3 *force )
{
    if (myCollideObject)
    {
        ((nOdeCollideObject *)myCollideObject)->addForce( force );
    }
}

void nOdeNode::addTorque( vector3 *torque )
{
    if (myCollideObject)
    {
        ((nOdeCollideObject *)myCollideObject)->addTorque( torque );
    }
}
