#ifndef N_ODECOLLIDECONTEXT_H
#define N_ODECOLLIDECONTEXT_H
//------------------------------------------------------------------------------
/**
    @class nOdeCollideContext
    @ingroup NODEPhysicsContribModule
    @brief A collision database of sorts.

    nOdeCollideContext is basically a database used by the collision 
    sub-system, one of these will probably suffice in most cases.
    OdePhysics doesn't expose ODE's collision sub-system directly, so the
    user won't be providing a callback that will be called each time two
    shapes collide. Instead OdePhysics collects results into an
    nOdeCollideContext and you can then query that collide context to
    see what collided.
    
    (c) 2003  Vadim Macagon
    
    nOdeCollideContext is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#ifndef N_ODECOLLIDEREPORT_H
#include "odephysics/nodecollidereport.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ODECOLLTYPE_H
#include "odephysics/nodecolltype.h"
#endif

#ifndef N_ODESERVER_H
#include "odephysics/nodeserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeCollideContext
#include "kernel/ndefdllclass.h"

class line3;
class sphere;
class nOdeCollideSpace;

//------------------------------------------------------------------------------
class N_PUBLIC nOdeCollideContext : public nRoot
{
  public:
    friend class nOdeServer;
      
    nOdeCollideContext();
    virtual ~nOdeCollideContext();
          
    /// compute contacts between collision shapes in the space
    int Collide( dSpaceID spaceId, bool clearCache );
    
    /// get the collide reports for the collisions computed inside Collide()
    int GetCollissionReports( nOdeCollideShape*, nOdeCollideReport**& );
    
    /// do a line-model check
    int LineCheck( dSpaceID spaceId, const line3& line, nOdeCollType collType, 
                   nOdeCollClass collClass, nOdeCollideReport**& crPtr );
       
    static nKernelServer* kernelServer;
    
  private:
    enum { N_MAX_CONTACT_GEOMS = 12 };

    static void RayCallback( void* data, dGeomID g1, dGeomID g2 );
    /// check for collision between a ray geom and some other geom
    void RayCollide( dGeomID ray, dGeomID shape );
    static void NearCallback( void* data, dGeomID g1, dGeomID g2 );
    /// check for collision between two shapes
    void Collide( nOdeCollideShape* s1, nOdeCollideShape* s2 );
    
    // class to give ray/sphere for Check() methods
    nOdeCollClass checkClass;
    // temp contacts array
    dContactGeom contactGeoms[N_MAX_CONTACT_GEOMS];
    // collide reports for Collide()
    nOdeCollideReportHandler collideReportHandler;
    // collide reports for Check() functions
    nOdeCollideReportHandler checkReportHandler;
    
    nAutoRef<nOdeServer> ref_OdeServer;
};

//------------------------------------------------------------------------------
#endif
