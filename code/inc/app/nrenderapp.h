#ifndef N_RENDERAPP_H
#define N_RENDERAPP_H
//------------------------------------------------------------------------------
/**
    @class nRenderApp

    @brief a brief description of the class

    a detailed description of the class

    (C)	2009	plushy
*/
#ifndef N_APPLICATION_H
#include "app/napplication.h"
#endif

#undef N_DEFINES
#define N_DEFINES nRenderApp
#include "kernel/ndefdllclass.h"
class nGfxServer;
class nChannelServer;
class nSceneGraph2;
class nShadowServer;
class n3DNode;

//------------------------------------------------------------------------------
class N_PUBLIC nRenderApp : public nApplication
{
public:
    /// constructor
    nRenderApp();
    /// destructor
    virtual ~nRenderApp();
    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

protected:	
	virtual bool initialize();
	virtual bool begin();
	virtual void end();
	virtual bool process_command_line();

protected:
	stl_string gfx_class;
	stl_string mode;

public:
	nAutoRef<nGfxServer>	refGfx;
	nAutoRef<nSceneGraph2>	refSG;
	nAutoRef<nChannelServer> refChannel;
	nAutoRef<nShadowServer> refShadow;
	nAutoRef<n3DNode>	 refScene;
	
	bool grid;	
};
//------------------------------------------------------------------------------
#endif

