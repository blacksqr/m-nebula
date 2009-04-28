#ifndef NODE_COLLIDECONTEXT_H
#define NODE_COLLIDECONTEXT_H
//-------------------------------------------------------------------
//  CLASS
//  nOdeCollideContext -- defines its own collission space 
//						  for ODE
//
//  OVERVIEW
//  A nOdeCollideContext creates an ODE collission space, defined by
//  a collection of nCollideObjects which can collide with 
//  each other. nCollideObjects can be added and remove
//  from the context at any time.
//-------------------------------------------------------------------

#define dSINGLE 1
#include "ode/ode.h"

#include "collide/ncollidecontext.h"

//-------------------------------------------------------------------
class nCollideObject;
class nGfxServer;
typedef int nCollClass;
class nCollideServer;

class nOdeCollideContext : public nCollideContext {
private:
	dSpaceID space;
	
public:
    nOdeCollideContext(nCollideServer *_cs)
        : nCollideContext( _cs )
    { 
		space = dHashSpaceCreate(0);
	};

    virtual ~nOdeCollideContext();
    virtual nCollideObject *NewObject(void);
    virtual void ReleaseObject(nCollideObject *);
    virtual void AddObject(nCollideObject *);
    virtual void RemObject(nCollideObject *);
    virtual int  Collide(void);

	// Overridden from base to allow collideReportHandler access to subclass.
    /// get the collide reports for the collissions computed inside Collide()
	virtual int GetCollissions(nCollideObject *, nCollideReport **&);

	nCollideReportHandler *getReportHandler()
	{
		return &collideReportHandler;
	}

	dContact *getContactPool(){ return &contact[0]; };
protected:

    nCollideReportHandler collideReportHandler;     // collide reports for Collide()

	dSpaceID getOdeSpace()
	{
		return( space );
	};
    dContact contact[255];			// up to N contacts per box


//    virtual int  GetCollisions(nCollideObject *, nCollideReport **&);
//    virtual int  GetAllCollisions(nCollideReport **&);
    
//    instant checks -- move to use ODE?
//    virtual int MovingSphereCheck(const Vector3& p0,           // in: starting position
//                                  const Vector3& v0,           // in: vector to ending position
//                                  float radius,                // in: radius
//                                  nCollClass cc,               // in: collission class for collission type query
//                                  nCollideReport **& cr_ptr);  // out: pointer to array of pointers to nCollideReport's
};
//-------------------------------------------------------------------
#endif
