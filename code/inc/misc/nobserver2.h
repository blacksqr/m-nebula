#ifndef N_OBSERVER2_H
#define N_OBSERVER2_H
//-------------------------------------------------------------------
//  CLASS
//  nObserver
//
//  OVERVIEW
//  Ein nObserver Objekt erlaubt den Aufbau eines flexibleren
//  und maechtigeren gfxserv in einem normalen nsh-Script. 
//-------------------------------------------------------------------
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nObserver2
#include "kernel/ndefdllclass.h"
//-------------------------------------------------------------------
class nGfxServer;
class nInputServer;
class nConServer;
class nScriptServer;
class nParticleServer;
class nSceneGraph2;
class nChannelServer;
class nInputEvent;
class n3DNode;
class nSpecialFxServer;
class nGuiServer;
class nOdePhysics;

// Uncomment if you are using the java server
//class nJavaServer;

class N_DLLCLASS nObserver2 : public nRoot {
protected:
    nAutoRef<nGfxServer>        ref_gs;
    nAutoRef<nInputServer>      ref_is;
    nAutoRef<nConServer>        ref_con;
    nAutoRef<nScriptServer>     ref_ss;
    nAutoRef<nSceneGraph2>      ref_sg;
    nAutoRef<nChannelServer>    ref_chn;
    nAutoRef<nParticleServer>   ref_ps;
    nAutoRef<nSpecialFxServer>  ref_fx;
	nAutoRef<nGuiServer>		ref_gui;
	nAutoRef<nOdePhysics>		ref_pz;

	// Uncomment if you are using the java server
	// nAutoRef<nJavaServer>		ref_java;

    nAutoRef<n3DNode> ref_scene;
    nAutoRef<n3DNode> ref_camera;
    nAutoRef<n3DNode> ref_lookat;

    float sleep;
    bool grid;
    bool stop_requested;    

	bool visualize;

    int mouse_old_x, mouse_old_y;
    int mouse_cur_x, mouse_cur_y;
    int mouse_rel_x, mouse_rel_y;

    int timeChannel;
    int globalTimeChannel;

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nObserver2();
    virtual ~nObserver2();

    virtual void  SetSleep(float);
    virtual float GetSleep(void);
    virtual void  SetGrid(bool);
    virtual bool  GetGrid(void);

	// Visualize the CD system
	virtual void  SetVisualize(bool);
	virtual bool  GetVisualize();

    virtual bool Start(void);
    virtual void Stop(void);

    virtual void StartSingleStep(void);
    virtual bool RenderSingleStep(void);

    void RenderFrame(float time);

private:
    bool trigger(void);
    void get_mouse_input(nInputServer *);
    void handle_input(void);
    void render_grid(nGfxServer *, matrix44&);
    void render_node(n3DNode *, matrix44&, float);
    void place_camera(matrix44&, matrix44&, float);
    void render_frame(float time);
};
//-------------------------------------------------------------------
#endif
