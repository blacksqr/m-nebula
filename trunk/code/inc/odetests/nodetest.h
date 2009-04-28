#ifndef N_ODETEST_H
#define N_ODETEST_H
//------------------------------------------------------------------------------
/**
    @class nOdeTest

    @brief Simple framework for the ode tests.

    (C) 2003  Vadim Macagon
*/
//------------------------------------------------------------------------------
#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOdeTest
#include "kernel/ndefdllclass.h"

class nScriptServer;
class nGfxServer;
class nConServer;
class nInputServer;
class nSceneGraph2;
class nSpecialFxServer;
class nChannelServer;
class nPrimitiveServer;
class nOdeServer;
class nOdeCollideSpace;
class nOdeCar;
class nOdeCamera;
class nOdeGround;
class nOdeHingeTest;
class nOdeMeshTest;
//------------------------------------------------------------------------------
class nOdeTest : public nRoot
{
  public:
    enum { ODE_TEST_NONE, ODE_TEST_HINGE, ODE_TEST_BUGGY,
           ODE_TEST_MESH };
  
    /// constructor
    nOdeTest();
    /// destructor
    virtual ~nOdeTest();
    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);

    void SetSleep( float sleep );
    float GetSleep() const;
    
    /// enable/disable display of grid
    void SetGrid( bool display );
    /// find out if grid is being displayed
    bool GetGrid();
    
    /// set view transform
    void SetViewTransform( const matrix44& mat );
    /// set camera transform (inverse of view transform)
    void SetCameraTransform( const matrix44& mat );
    
    /// start game
    void Start();
    /// stop game
    void Stop();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
    
  private:
  
    void Cleanup();
  
    void HandleInput();
  
    /// updates the world for the next frame
    bool Trigger();
    /// renders a frame
    void RenderFrame( float time );
    /// renders a grid under OpenGL
    void RenderGrid( nGfxServer* gs, matrix44& ivwr );
    
    // debug collision info
    //nQuickWatch* collideInfo;
    //nQuickWatch* stepsInfo;
  
    /// true when the game needs to be stopped 
    bool stopRequested;
    /// amount of time to sleep after each frame
    float sleepTime;
    /// true when the grid needs to be displayed
    bool grid;
    
    int timeChannel;
    double lastTime;
    
    /// the path to the world lights
    nAutoRef<nRoot> ref_Lights;
    
    /// view transform
    matrix44 viewTransform;
    /// camera transform (inverse of view transform)
    matrix44 cameraTransform;
    
    nAutoRef<nGfxServer>        ref_GfxServer;
    nAutoRef<nInputServer>      ref_InputServer;
    nAutoRef<nSceneGraph2>      ref_SceneGraph;
    nAutoRef<nConServer>        ref_ConServer;
    nAutoRef<nSpecialFxServer>  ref_SfxServer;
    nAutoRef<nChannelServer>    ref_ChanServer;
    nAutoRef<nScriptServer>     ref_ScriptServer;
    nAutoRef<nPrimitiveServer>  ref_PrimitiveServer;
    
    /// ode tests stuff
    nAutoRef<nOdeServer>  ref_OdeServer;
    nAutoRef<nOdeCollideSpace> ref_CollSpace;
    nOdeCamera* camera;
    // buggy test
    nOdeCar* car;
    nOdeGround* ground;
    // hinge test
    nOdeHingeTest* hingeTest;
    // tri-mesh test
    nOdeMeshTest* meshTest;
    int testNum;  // the active test
};

//------------------------------------------------------------------------------
inline
void nOdeTest::SetSleep( float sleep )
{
  n_assert( !(sleep < 0.0f) );
  this->sleepTime = sleep;
}

//------------------------------------------------------------------------------
inline
float nOdeTest::GetSleep() const
{
  return this->sleepTime;
}

//------------------------------------------------------------------------------
inline
void nOdeTest::SetGrid( bool display )
{
  this->grid = display;
}

//------------------------------------------------------------------------------
inline
bool nOdeTest::GetGrid()
{
  return this->grid;
}

//------------------------------------------------------------------------------
/**
  @brief Set the current view transform.
  
  Call either SetViewTransform or SetCameraTransform whenever the view needs
  to be changed.
*/
inline
void nOdeTest::SetViewTransform( const matrix44& view )
{
  this->viewTransform = view;
  this->cameraTransform = view;
  this->cameraTransform.invert_simple();
}

//------------------------------------------------------------------------------
/**
  @brief Set the current camera transform, and current view transform.
  
  The camera transform is the inverse fo the view transform, so you only
  need to call either SetViewTransform or SetCameraTransform every time
  the view needs to be updated.
*/
inline
void nOdeTest::SetCameraTransform( const matrix44& cam )
{
  this->cameraTransform = cam;
  this->viewTransform = cam;
  this->viewTransform.invert_simple();
}

//------------------------------------------------------------------------------
#endif

