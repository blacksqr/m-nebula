#ifndef N_ODECOLLIDESHAPE_H
#define N_ODECOLLIDESHAPE_H
//------------------------------------------------------------------------------
/**
  @class nOdeCollideShape
  @ingroup NODEPhysicsContribModule
  @brief Describes a shape for the ODE collision system.

  (c) 2003 Vadim Macagon
  
  nOdeCollideShape is licensed under the terms of the Nebula License.
*/  
//------------------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_COLLTYPE_H
#include "collide/ncolltype.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#ifndef N_ODESERVER_H
#include "odephysics/nodeserver.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeCollideShape
#include "kernel/ndefdllclass.h"

class nGfxServer;
class nPrimitiveServer;
class nOdeBody;
class nOdeCollideContact;
class nOdeCollideSpace;

//------------------------------------------------------------------------------
class N_PUBLIC nOdeCollideShape
{
  friend class nOdeCollideSpace;
  friend class nOdeCollideContext;
  friend class nOdeTriMesh;
  friend class nOdeServer;

  public:
  
    enum nOdeShapeType { OST_UNKNOWN, OST_SPHERE, OST_BOX, OST_CAPSULE,
                         OST_MESH, 
						 OST_PLANE };
  
    /// constructor
    nOdeCollideShape();
    /// destructor
    virtual ~nOdeCollideShape();
        
    void* GetData() const;
    void SetData( void* data );
       
    /// get the space this shape belongs to (if any)
    nOdeCollideSpace* GetSpace();
       
    nOdeShapeType GetShapeType() const;
        
    /// get the unique shape id
    int GetId() const;
    /// set the shape id
    void SetId( int uniqueId );
    
    /// attach the shape to a body
    virtual void AttachTo( nOdeBody* body );
    /// get the body this shape is attached to
    nOdeBody* GetBody();
    
    //virtual void Enable( bool flag );
    //virtual bool IsEnabled();
    
    /// transform shape (scaling not supported)
    virtual void SetTransform( const matrix44& mat );
    /// get the transform for the shape
    virtual void GetTransform( matrix44* result );
    /// rotate the shape
    virtual void SetRotation( const matrix33& rot );
    virtual void SetRotation( const matrix44& rot );
    virtual void SetRotation( const quaternion& rot );
    /// get rotation of shape
    virtual void GetRotation( matrix33* rot );
    virtual void GetRotation( quaternion* rot );
    /// position the shape
    virtual void SetPosition( const vector3& pos );
    /// get position of shape
    virtual void GetPosition( vector3* pos );
    
    void SetCollClass( nOdeCollClass cc );
    nOdeCollClass GetCollClass() const;
    
    /// obtain total number of collisions the shape was involved in
    int GetNumCollissions() const;
    /// reset number of collisions shape was involved in
    void ClearCollissions();
    
    /// check for intersection of line with shape
    bool LineCheck( nOdeCollType collType, const line3& line, 
                    nOdeCollideContact* contact );
    
    /// visualize the collide shape (in model space)
    virtual void VisualizeLocal( nGfxServer* gfxServer, nPrimitiveServer* prim );
    /// render collision contacts for shape (in world space)
    virtual void VisualizeGlobal( nGfxServer* gfxServer );

  protected:
  
    dGeomID geomId;
    // unique id
    int id;
    // number of collisions this shape was involved in
    int numColls;
    // the application defined collision type
    nOdeCollClass collClass;
    // the shape type
    nOdeShapeType shapeType;
    // user data
    void* userdata;
    
    // ray used for LineCheck() queries
    static dGeomID rayId;
    static nOdeCollType rayCollType;
};

//------------------------------------------------------------------------------
#endif
