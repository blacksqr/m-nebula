#ifndef N_ODECOLLIDESHAPENODE_H
#define N_ODECOLLIDESHAPENODE_H
//------------------------------------------------------------------------------
/**
  @class nOdeCollideShapeNode
  @ingroup NODEPhysicsContribModule
  @brief A wrapper for nOdeCollideShape.

  nOdeCollideShapeNode descendants are meant to be used as wrappers for 
  nOde*Shape(s) in order to be able to use nOde*Shape(s) via script.

  (c) 2003 Vadim Macagon
  
  nOdeCollideShapeNode is licensed under the terms of the Nebula License.
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#include "util/nstl.h"

#ifndef N_ODESERVER_H
#include "odephysics/nodeserver.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeCollideShapeNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nOdeCollideShape;

class N_PUBLIC nOdeCollideShapeNode : public nRoot
{
  public:
    /// constructor
    nOdeCollideShapeNode();
    /// destructor
    virtual ~nOdeCollideShapeNode();
    /// persistency
    virtual bool SaveCmds( nPersistServer* pesistServer );

    nOdeCollideShape* GetShape() const;
    
    const char* GetShapeType();

    /// set if the nOdeCollideShape instance is destroyed along with the node
    void SetAutoCleanup( bool cleanup );
    /// check the current cleanup mode
    bool GetAutoCleanup() const;

    void SetCollClass( const char* /*collClass*/ );
    const char* GetCollClass();
    
    /// attach the shape to a body
    void AttachTo( const char* /*bodyNode*/ );
    /// get the body this shape is attached to
    const char* GetBody();
    
    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
    
  protected:
    nOdeCollideShape* shape;
    nAutoRef<nOdeServer> ref_OdeServer;
        
  private:
    //nDynAutoRef<nOdeBodyNode> ref_OdeBodyNode;
    bool autoClean;
    stl_string collClass;
    stl_string bodyPath;
};

//------------------------------------------------------------------------------
#endif
