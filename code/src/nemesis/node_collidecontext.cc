#define N_IMPLEMENTS nOdeCollideContext
//-------------------------------------------------------------------
//  nOdeCollideContext.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "misc/nquickwatch.h"
#include "collide/ncollideobject.h"
#include "nemesis/node_collidecontext.h"
#include "nemesis/node_collideobject.h"
 
//-------------------------------------------------------------------
//  ~nOdeCollideContext()
//  16-Jun-00   floh    created
//-------------------------------------------------------------------
nOdeCollideContext::~nOdeCollideContext()
{
    // release all owned collide objects
	// If you do it this way you won't assert in
	// nCNode or nNode's destructor.
    nOdeCollideObject *co;
    while ((co = (nOdeCollideObject *) this->owned_list.RemHead())) {
		if (co->IsAttached()) this->RemObject(co);
		co->SetContext(NULL);
        delete co;
    };
	
	dSpaceDestroy( space );
}

//-------------------------------------------------------------------
//  NewObject()
//  Construct a new collide object.
//  27-Jun-00   floh    created
//-------------------------------------------------------------------
nCollideObject *nOdeCollideContext::NewObject(void)
{
    nOdeCollideObject *co = new nOdeCollideObject;
    co->SetId(this->unique_id++);
    co->SetContext(this);
    this->owned_list.AddTail(co);
    return co;
}


//-------------------------------------------------------------------
//  ReleaseObject()
//  Kill an owned ode collide object. -- Overridden to correctly 
//  point to and release the nCollideObject
//  12-Feb-02   whitegold    created
//-------------------------------------------------------------------
void nOdeCollideContext::ReleaseObject(nCollideObject *co)
{
    n_assert(co);
    n_assert(co->GetContext() == this);
    if (co->IsAttached()) this->RemObject(co);
    co->SetContext(NULL);
    co->Remove();
	nOdeCollideObject *oco = static_cast<nOdeCollideObject *>(co);
    delete oco;
}

//-------------------------------------------------------------------
//  AddObject()
//  26-Jun-00   floh    created
//-------------------------------------------------------------------
void nOdeCollideContext::AddObject(nCollideObject *co)
{
	nCollideContext::AddObject( co );

	// Add the object into the ODE collision space
	nOdeCollideObject *oco = (nOdeCollideObject *)co;
	dSpaceAdd (space, oco->myOdeGeometryId);
}

//-------------------------------------------------------------------
//  RemObject()
//  26-Jun-00   floh    created
//-------------------------------------------------------------------
void nOdeCollideContext::RemObject(nCollideObject *co)
{
	nCollideContext::RemObject( co );

	nOdeCollideObject *oco = static_cast<nOdeCollideObject *>(co);
	dSpaceRemove( space, oco->myOdeGeometryId );
}


//-------------------------------------------------------------------
//  GetCollissions()
//  Get all collissions an object is involved in.
//  Returns pointer to an internal collission array and
//  the number of collissions.
//  04-Jul-00   floh    created
//-------------------------------------------------------------------
int nOdeCollideContext::GetCollissions(nCollideObject *co, nCollideReport **& cr)
{
    if (co->GetNumCollissions() > 0) {
        return this->collideReportHandler.GetCollissions(co,cr);
    } else {
        cr = NULL;
        return 0;
    }
}


// this is called by dSpaceCollide when two objects in space are
// potentially colliding.
static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  int i;

  // exit without doing anything if the two bodies are connected by a joint
  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);
  if (b1 && b2 && dAreConnected (b1,b2)) return;

  nOdeCollideObject *oc1, *oc2;
  oc1 = (nOdeCollideObject *)dGeomGetData( o1 );
  oc2 = (nOdeCollideObject *)dGeomGetData( o2 );

  nOdePhysics *cs = (nOdePhysics  *)((nOdeCollideContext *)data)->GetCollideServer();

  // Return if these objects are in the "Ignore" category
  nCollType ct = cs->QueryCollType(oc1->GetCollClass(),oc2->GetCollClass());
  if (COLLTYPE_IGNORE == ct) return;

  dContact *contact = ((nOdeCollideContext *)data)->getContactPool();			// up to 3 contacts per box
  for (i=0; i<255; i++) 
  {
	
	dSurfaceParameters *sp1 = oc1->GetSurfaceParameters();
	dSurfaceParameters *sp2 = oc1->GetSurfaceParameters();
	
	// We have 2, possibly different, surfaces... Now what?
	// I say we average them for now.. 
	contact[i].surface.mode = sp2->mode | sp1->mode;
	
	if (sp1->mu == dInfinity || 
		sp2->mu == dInfinity )
	{
		contact[i].surface.mu = dInfinity;
	} else {
		contact[i].surface.mu = (sp2->mu + sp1->mu) * .5;
	}	

	if ( contact[i].surface.mode & dContactBounce )
	{
		contact[i].surface.bounce = (sp1->bounce + sp2->bounce) * .5;
		contact[i].surface.bounce_vel = (sp1->bounce_vel + sp2->bounce_vel) * .5;
	}

	if ( contact[i].surface.mode & dContactMu2 )
		contact[i].surface.mu2 = (sp1->mu2 + sp2->mu2) * .5;
	if ( contact[i].surface.mode & dContactSoftERP )
		contact[i].surface.soft_erp = (sp1->soft_erp + sp2->soft_erp) * .5;
	if ( contact[i].surface.mode & dContactSoftCFM )
		contact[i].surface.soft_cfm = (sp1->soft_cfm + sp2->soft_cfm) * .5;
	if ( contact[i].surface.mode & dContactMotion1 )
		contact[i].surface.motion1 = (sp1->motion1 + sp2->motion1) * .5;
	if ( contact[i].surface.mode & dContactMotion2 )
		contact[i].surface.motion2 = (sp1->motion2 + sp2->motion2) * .5;
	if ( contact[i].surface.mode & dContactSlip1 )
		contact[i].surface.slip1 = (sp1->slip1 + sp2->slip1) * .5;
	if ( contact[i].surface.mode & dContactSlip2 )
		contact[i].surface.slip2 = (sp1->slip2 + sp2->slip2) * .5;

	contact[i].geom.normal[1] = 2000.0;
  
  }
  
  nCollideReportHandler *crh = ((nOdeCollideContext *)data)->getReportHandler();

  dGeomID go1 = o1;
  dGeomID go2 = o2;
//  const char *name = oc1->GetShape()->GetName();
//  int len = strlen( name );
//  const char *name2 = oc2->GetShape()->GetName();
//  int len2 = strlen( name2 );
//  if ( name[len - 1] == 's' && name2[len2-1] == 't' )
  //{
//	 dGeomID o3 = go1;  
//	 go1 = go2;
//	 go2 = o3;
//  }

  if (int numc = dCollide (go1,go2,48,&contact[0].geom,sizeof(dContact))) 
  {
    for (i=0; i<numc; i++) 
	{
      dJointID c = dJointCreateContact (cs->getWorldID() , cs->getCollJointGroupID() ,contact+i);
      dJointAttach (c,b1,b2);

      // Update the object's collision information with the number of collisions
	  // reported 
	  oc1->incrementCollision();
	  oc2->incrementCollision();

	  // Now create the nCollideReports
      nCollideReport cr;
	
	  cr.co1 = oc1;
	  cr.co2 = oc2;
	  cr.contact = vector3( contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2] );
	  cr.co1_normal = vector3( contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2]);
	  cr.co2_normal = -cr.co1_normal;
	  
	  // Hmm.. What to do, if anything, with these values. //((nOdeCollideContext *)data)->GetCollideServer()->ks->
	  cr.tstamp = nOdePhysics::pKernelServer->ts->GetFrameTime();

//	    const dReal *pos1 = dGeomGetPosition( o1 );
//	    const dReal *pos2 = dGeomGetPosition( o2 );
//      cr.co1_center = vector3( pos1[0], pos1[1], pos1[2] );
//      cr.co2_center = vector3( pos2[0], pos2[1], pos2[2] );

      crh->AddCollission(cr, oc1->GetId(), oc2->GetId());
    }
  }
}

//-------------------------------------------------------------------
//  Collide()
//  Call collide on each object in the context. After this, each
//  object's collission array holds all collissions this object
//  was involved with.
//  26-Jun-00   floh    created
//-------------------------------------------------------------------
int nOdeCollideContext::Collide(void)
{
    // first, clear the collission counters in all collide objects
    nNode *context_node;
    for (context_node = this->attached_list.GetHead();
         context_node;
         context_node = context_node->GetSucc())
    {
        nCollideObject *co = (nCollideObject *) context_node->GetPtr();
        co->ClearCollissions();
    }

    // check the collission status for each object
    this->collideReportHandler.BeginFrame();
	
 	dSpaceCollide( space, this, &nearCallback );

    this->collideReportHandler.EndFrame();

    int num_coll = this->collideReportHandler.GetNumCollissions();
    return num_coll;
}


//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
