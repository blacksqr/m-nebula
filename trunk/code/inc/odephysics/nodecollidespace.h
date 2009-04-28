#ifndef N_ODECOLLIDESPACE_H
#define N_ODECOLLIDESPACE_H
//------------------------------------------------------------------------------
/**
    @class nOdeCollideSpace
    @ingroup NODEPhysicsContribModule
    @brief Defines a collision space.

    nOdeCollideSpace corresponds to an ODE Space. OdePhysics currently works
    with Simple and Hash spaces, Quad Tree spaces haven't yet been fully
    implemented in core ODE so nOdeQuadTreeSpace will only work once that's
    been fixed. nOdeCollideSpace(s) store collision detection results into
    an nOdeCollideContext - which you have to supply them with.

    An nOdeCollideSpace can consist of a collection of nOdeCollideShapes which 
    can collide with each other, or with shapes in overlapping spaces.
    
    nOdeCollideSpace(s) (and descendants) can also be nested, the nesting 
    hierarchy corresponds directly to the NOH, please note that anything you 
    put under an nOdeCollideSpace in the NOH must itself be an nOdeCollideSpace.
    
    e.g.
    sel /sys/servers/ode/spaces     <- Let's say we put all our spaces here.
      new nodehashspace root        <- Create our top-level space
      sel root
        new nodesimplespace sub1    <- Create a sub-space
        new nodesimplespace sub2    <- Create another one
        sel sub2
          new nodesimplespace sub3  <- And another one
    
    (c) 2003  Vadim Macagon
    
    nOdeCollideSpace is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_ODECOLLTYPE_H
#include "odephysics/nodecolltype.h"
#endif

#ifndef N_ODESERVER_H
#include "odephysics/nodeserver.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeCollideSpace
#include "kernel/ndefdllclass.h"

class nGfxServer;
class nPrimitiveServer;
class nOdeCollideContext;
class nOdeCollideShape;
class nOdeCollideShapeNode;
class nOdeCollideReport;
class line3;
class vector3;
//------------------------------------------------------------------------------
class N_PUBLIC nOdeCollideSpace : public nRoot
{
  public:
    nOdeCollideSpace();
    virtual void Initialize();
    virtual ~nOdeCollideSpace();
    
    void SetContext( nOdeCollideContext* );
    nOdeCollideContext* GetContext() const;
        
    /// create and add a sub-space to the space
    nOdeCollideSpace* AddSubSpace( const char* className, const char* subSpaceName );
    /// remove and release a sub-space from the space
    void RemSubSpace( nOdeCollideSpace* );
    /// remove and release all sub-spaces (recursively)
    void RemAllSubSpaces();
    /// obtain the first sub-space
    nOdeCollideSpace* GetFirstSubSpace();
    /// obtain the next sub-space in the list
    nOdeCollideSpace* GetNextSubSpace();
        
    /// add collide shape to the space
    void AddShape( nOdeCollideShape* );
    /// remove collide shape from the space
    void RemShape( nOdeCollideShape* );
    /// remove all shapes from the space
    void RemAllShapes();
    /// obtain the first shape
    nOdeCollideShape* GetFirstShape();
    /// obtain the next shape
    nOdeCollideShape* GetNextShape();
        
    /// compute contacts between shapes in the space
    int Collide( bool clearCache = true );
        
    /// find any shapes (in this space) intersected by the line
    int LineCheck( const line3& line, nOdeCollType collType, 
                   nOdeCollClass collClass, nOdeCollideReport**& crPtr );
    
    /// debug visualization
    virtual void Visualize( nGfxServer*, nPrimitiveServer* );
    
    static nKernelServer* kernelServer;
    
  protected:
    /// add a sub-space to this space
    void AddSubSpace( nOdeCollideSpace* subSpace );
        
    // corresponding space
    dSpaceID spaceId;
    // collide context this space is attached to
    nRef<nOdeCollideContext> ref_Context;
    
  private:
    /// clear contact counters for shapes in this space
    void ClearContactCounts();
    
    /// iterators
    nRef<nRoot> ref_CurSubSpace;
    int curGeomIndex;
};

//------------------------------------------------------------------------------
#endif // N_ODECOLLIDESPACE_H
