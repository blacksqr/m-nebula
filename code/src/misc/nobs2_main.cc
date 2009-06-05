#define N_IMPLEMENTS nObserver2
//-------------------------------------------------------------------
//  nobs_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelserver.h"
#include "misc/nconserver.h"
#include "input/ninputserver.h"
#include "kernel/nscriptserver.h"
#include "node/n3dnode.h"
#include "misc/nobserver2.h"
#include "misc/nparticleserver.h"
#include "misc/nspecialfxserver.h"

// Uncomment if you are using the gui server
// #include "gui/nguiserver.h"

#include "nemesis/node_physics.h"

// Uncomment if you are using the java server
// #include "script/njavaserver.h"
nNebulaScriptClass(nObserver2, "nroot");
//-------------------------------------------------------------------
//  nObserver2()
//  20-Jul-99   floh    created
//-------------------------------------------------------------------
nObserver2::nObserver2()
         :  ref_gs(kernelServer, this),
            ref_is(kernelServer, this),
            ref_ss(kernelServer, this),
            ref_sg(kernelServer, this),
            ref_chn(kernelServer, this),
            ref_fx(kernelServer, this),
            ref_con(kernelServer, this),
            ref_ps(kernelServer, this),
            ref_scene(kernelServer, this),
            ref_camera(kernelServer, this),
            ref_lookat(kernelServer, this),
			ref_gui(kernelServer, this),
			ref_pz(kernelServer, this )
// Uncomment if you are using the java server
// #include "script/njavaserver.h"
			//,ref_java(kernelServer, this)
{
    this->ref_gs     = "/sys/servers/gfx";
    this->ref_is     = "/sys/servers/input";
    this->ref_ss     = "/sys/servers/script";
    this->ref_sg     = "/sys/servers/sgraph2";
    this->ref_chn    = "/sys/servers/channel";
    this->ref_fx     = "/sys/servers/specialfx";
    this->ref_con    = "/sys/servers/console";
    this->ref_ps     = "/sys/servers/particle";
    this->ref_fx     = "/sys/servers/specialfx";
    this->ref_scene  = "/usr/scene";
    this->ref_camera = "/usr/camera";
    this->ref_lookat = "/usr/lookat";
	this->ref_gui    = "/sys/servers/gui";
	this->ref_pz     = "/sys/servers/physics";

	// Uncomment if you are using the java server
// 	this->ref_java   = "/sys/servers/java";

    this->sleep = 0.02f;
    this->grid  = true;
    this->stop_requested = false;

	this->visualize = false;

    this->mouse_old_x = 0;
    this->mouse_old_y = 0;
    this->mouse_cur_x = 0;
    this->mouse_cur_y = 0;
    this->mouse_rel_x = 0;
    this->mouse_rel_y = 0;

    this->timeChannel       = 0;
    this->globalTimeChannel = 0;

    this->ref_ss->InitIpc("nebula");
}

//-------------------------------------------------------------------
//  ~nObserver2()
//  20-Jul-99   floh    created
//-------------------------------------------------------------------
nObserver2::~nObserver2()
{
    if (this->ref_ss.isvalid())
    {
        this->ref_ss->KillIpc();
    }
}

//-------------------------------------------------------------------
//  SetVisualize()
//  10-Feb-02   whitegold  created
//-------------------------------------------------------------------
void nObserver2::SetVisualize(bool s)
{
    this->visualize = s;
}

//-------------------------------------------------------------------
//  GetVisualize()
//  10-Feb-02   whitegold  created
//-------------------------------------------------------------------
bool nObserver2::GetVisualize(void)
{
    return this->visualize;
}

//-------------------------------------------------------------------
//  SetSleep()
//  20-Jul-99   floh    created
//-------------------------------------------------------------------
void nObserver2::SetSleep(float s)
{
    this->sleep = s;
}

//-------------------------------------------------------------------
//  GetSleep()
//  20-Jul-99   floh    created
//-------------------------------------------------------------------
float nObserver2::GetSleep(void)
{
    return this->sleep;
}

//-------------------------------------------------------------------
//  SetGrid()
//  20-Jul-99   floh    created
//-------------------------------------------------------------------
void nObserver2::SetGrid(bool b)
{
    this->grid = b;
}

//-------------------------------------------------------------------
//  GetGrid()
//  20-Jul-99   floh    created
//-------------------------------------------------------------------
bool nObserver2::GetGrid(void)
{
    return this->grid;
}

//-------------------------------------------------------------------
//  Start()
//  20-Jul-99   floh    created
//-------------------------------------------------------------------
bool nObserver2::Start(void)
{
    kernelServer->ts->EnableFrameTime();
    kernelServer->ts->ResetTime();

    this->timeChannel       = this->ref_chn->GenChannel("time");
    this->globalTimeChannel = this->ref_chn->GenChannel("gtime");

    while (this->trigger());

    this->stop_requested = false;
    return true;
}

//-------------------------------------------------------------------
//  Stop()
//  20-Jul-99   floh    created
//-------------------------------------------------------------------
void nObserver2::Stop(void)
{
    this->stop_requested = true;
    kernelServer->ts->DisableFrameTime();
}

//-------------------------------------------------------------------
//  StartSingleStep()
//  19-Dec-99   floh    created
//-------------------------------------------------------------------
void nObserver2::StartSingleStep(void)
{
    kernelServer->ts->ResetTime();

    this->timeChannel       = this->ref_chn->GenChannel("time");
    this->globalTimeChannel = this->ref_chn->GenChannel("gtime");

    this->stop_requested = false;
}

//-------------------------------------------------------------------
//  RenderSingleStep()
//  19-Dec-99   floh    created
//-------------------------------------------------------------------
bool nObserver2::RenderSingleStep(void)
{
    return this->trigger();
}

//-------------------------------------------------------------------
//  render_grid()
//  21-Jul-99   floh    created
//-------------------------------------------------------------------
void nObserver2::render_grid(nGfxServer *gs, matrix44& ivwr)
{
    nRState rs;
    rs.Set(N_RS_TEXTUREHANDLE,0);  gs->SetState(rs);
    rs.Set(N_RS_LIGHTING,N_FALSE); gs->SetState(rs);
    gs->SetMatrix(N_MXM_MODELVIEW,ivwr);
    gs->Begin(N_PTYPE_LINE_LIST);
    gs->Rgba(0.5f,0.5f,1.0f,1.0f);
    float p;
    for (p=-25.0f; p<=+25.0f; p+=1.0f) 
    {
        gs->Coord(p, 0.0f, -25.0f);
        gs->Coord(p, 0.0f, +25.0f);
        gs->Coord(-25.0f, 0.0f, p);
        gs->Coord(+25.0f, 0.0f, p);
    }
    gs->End();
    rs.Set(N_RS_LIGHTING,N_TRUE); gs->SetState(rs);
}

//-------------------------------------------------------------------
//  render()
//  Rendert die uebergebene Node in den Scenegraph.
//  21-Jul-99   floh    created
//  24-Nov-99   floh    + Support fuer gtime Channel
//  18-Jun-01   floh    + converted to new sgraph/channel handling
//-------------------------------------------------------------------
void nObserver2::render_node(n3DNode *n, matrix44& m, float time)
{
    nSpecialFxServer *fx = NULL;
    if (this->ref_fx.isvalid()) 
    {
        fx = this->ref_fx.get();
    }

    this->ref_chn->SetChannel1f(this->timeChannel, time);
    this->ref_chn->SetChannel1f(this->globalTimeChannel, time);

    nSceneGraph2* sg = this->ref_sg.get();
    if (sg->BeginScene(m))
    {
        // trigger specialfx server
        if (fx) fx->Begin();
        sg->Attach(n, 0);
        if (fx) fx->End(sg);
        sg->EndScene(true);
    }
}

//-------------------------------------------------------------------
//  render_frame()
//  Render a single frame.
//  01-May-00   floh    created
//-------------------------------------------------------------------
void nObserver2::render_frame(float time)
{
    nGfxServer   *gs = this->ref_gs.get();
    nConServer   *con = this->ref_con.get();

    // Input-Events auswerten, Kamera ausrichten
    this->handle_input();

    // Rendering
    if (gs->BeginScene()) 
    {
        matrix44 ivwr,identity;

        // trigger particle server (needs timestamp)
        if (this->ref_ps.isvalid()) 
        {
            this->ref_ps->Trigger();
        }

        
        this->render_node(this->ref_camera.get(), identity, time);
        ivwr = this->ref_camera->GetM();
        gs->SetMatrix(N_MXM_VIEWER, ivwr);
        ivwr.invert_simple();
        this->render_node(this->ref_lookat.get(), ivwr, time);
        this->render_node(this->ref_scene.get(), ivwr, time);

        if (this->grid) this->render_grid(gs, ivwr);

//  Uncomment if you're using the gui server
//		if(this->ref_gui.isvalid())
//			this->ref_gui->Render();	

		// Collision Detection system -- This will probably need
		// to be expanded to include more than 1 kind of context
		// but this works for now.
		if (this->ref_pz.isvalid() )
		{
   			if (visualize)
				this->ref_pz->GetDefaultContext()->Visualize( gs );
			this->ref_pz->GetDefaultContext()->Collide();
		}


        con->Render();                        
        gs->EndScene();
        this->ref_is->FlushEvents();
        if (this->sleep > 0.0) n_sleep(this->sleep);
    }
}

//-------------------------------------------------------------------
//  trigger()
//  20-Jul-99   floh    created
//  20-Dec-99   floh    Console-Server wird jetzt mit im Input-
//                      Server getriggert (Input-Recording!)
//  26-Jan-00   floh    GetFrameTime()
//  28-Apr-00   floh    no longer depends on particle server
//-------------------------------------------------------------------
bool nObserver2::trigger(void)
{
    // die Server triggern
    if (this->stop_requested)     return false;
    if (!this->ref_gs->Trigger()) return false;
    if (!this->ref_ss->Trigger()) return false;
    float time = (float) kernelServer->ts->GetFrameTime();

    kernelServer->Trigger();
    kernelServer->ts->Trigger();
    this->ref_is->Trigger(time);
	
	// Trigger java to process the previous inputs
	// Uncomment if you are using the java server
	// if (this->ref_java.isvalid())
	//	this->ref_java->Trigger();

	// The physics server
	if (this->ref_pz.isvalid())
		this->ref_pz->Trigger();

	// Uncomment if you are using the gui server
	// Trigger the gui to render the new updated frame
	// if (this->ref_gui.isvalid())
	//	this->ref_gui->Trigger();

    this->render_frame(time);
    return true;
}

//-------------------------------------------------------------------
//  RenderFrame()
//  Render a single frame independently from render loop. Useful
//  if display needs to be refreshed "at once" inside a
//  script callback procedure.
//  01-May-00   floh    created
//-------------------------------------------------------------------
void nObserver2::RenderFrame(float time)
{
    this->render_frame(time);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
