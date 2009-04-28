#define N_IMPLEMENTS nMeshNode
//------------------------------------------------------------------------------
//  nmeshnode_main.cc
//  (C) 2000..2001 RadonLabs GmbH -- A.Weissflog
//------------------------------------------------------------------------------
#include "node/nmeshnode.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nindexbuffer.h"
#include "gfx/nn3dmeshloader.h"
#include "gfx/nnvxmeshloader.h"
#include "shadow/nshadowcaster.h"
#include "kernel/nfileserver2.h"

nNebulaScriptClass(nMeshNode, "nvisnode");

//------------------------------------------------------------------------------
/**
*/
nMeshNode::nMeshNode() :
    refShadowServer(kernelServer, this),
    refFileServer(kernelServer, this),
    refShadowCaster(this),
    ref_vb(this),
    ref_ibuf(this),
    readonly(false),
    castShadows(false)
{
    this->refShadowServer = "/sys/servers/shadow";
    this->refFileServer   = "/sys/servers/file2";
}

//------------------------------------------------------------------------------
/**
*/
nMeshNode::~nMeshNode()
{
    // release optional shadow caster
	release_ref(this->refShadowCaster);
	// release vertex buffer
	release_ref(this->ref_vb);
	// release index buffer
	release_ref(this->ref_ibuf);
}

//------------------------------------------------------------------------------
/**
    Get pointer to embedded index buffer object.

    @return     pointer to index buffer object, NULL if loading mesh
                has failed
*/
nIndexBuffer*
nMeshNode::GetIndexBuffer()
{
    if (!this->ref_ibuf.isvalid()) {
        if (!this->loadMesh()) return NULL;
    }
    return this->ref_ibuf.get();
}


//------------------------------------------------------------------------------
/**
    Get pointer to embedded vertex buffer object.

    @return     pointer to vertex buffer object, NULL if loading mesh
                has failed
*/
nVertexBuffer*
nMeshNode::GetVertexBuffer()
{
    if (!this->ref_vb.isvalid()) {
        if (!this->loadMesh()) return NULL;
    }
    return this->ref_vb.get();
}

//------------------------------------------------------------------------------
/**
    Set readonly status. Meshes which are never rendered need to be set to
    "readonly". Readonly meshes are for instance input meshes for mesh 
    interpolation, mixing or skinning.

    @param      b       readonly flag
*/
void
nMeshNode::SetReadOnly(bool b)
{
    this->readonly = b;
}

//------------------------------------------------------------------------------
/**
    Get the current readonly status.

    @return             readonly flag
*/
bool
nMeshNode::GetReadOnly()
{
    return this->readonly;
}

//------------------------------------------------------------------------------
/**
    Defines if this object should cast shadows. Must be defined BEFORE
    the mesh is loaded, toggling the flag will have no effect unless the 
    mesh is reloaded.

    @param      b       true if shadow casting, default is false
*/
void
nMeshNode::SetCastShadow(bool b)
{
    this->castShadows = b;
}

//------------------------------------------------------------------------------
/**
    Return shadow casting state.

    @return     shadow casting flag   
*/
bool
nMeshNode::GetCastShadow()
{
    return this->castShadows;
}

//------------------------------------------------------------------------------
/**
    Preload the mesh data.
*/
void
nMeshNode::Preload()
{
    // file name set?
    if (this->rsrc_path.GetPath())
    {
        // (re)-load mesh on demand
        if (!this->ref_vb.isvalid())
        {
            this->loadMesh();
        }
    }
    nVisNode::Preload();
}

//------------------------------------------------------------------------------
/**
    Set filename of mesh file (flattened Wavefront obj file).

    @param  fname       filename of mesh file
*/
void 
nMeshNode::SetFilename(const char *fname)
{
    n_assert(fname);  

	// invalidate current mesh
	// release optional shadow caster
	release_ref(this->refShadowCaster);
	// release vertex buffer
	release_ref(this->ref_vb);
	// release index buffer
	release_ref(this->ref_ibuf);


    // set new mesh file name, reload will happen in next frame
    this->rsrc_path.Set(this->refFileServer.get(), fname);
}

//------------------------------------------------------------------------------
/**
    Get the filename of mesh file.

    @return     filename of mesh file (can be NULL).
*/
const char *
nMeshNode::GetFilename(bool AbsolutPath)
{    
    if ( AbsolutPath )
        return this->rsrc_path.GetAbsPath();
    else
    return this->rsrc_path.GetPath();
}

//------------------------------------------------------------------------------
/**
    Attach mesh node to scene.

    @param  sceneGraph      pointer to scene graph object
    @return                 true if success
*/
bool 
nMeshNode::Attach(nSceneGraph2 *sceneGraph)
{
    if (nVisNode::Attach(sceneGraph)) {
        // don't attach to scene if we are a readonly mesh
        if (!this->readonly)
        {
            sceneGraph->AttachMeshNode(this);
        }
        return true;
    } 
    return false;
}

//------------------------------------------------------------------------------
/**
    Update state and "render" mesh by calling nSceneGraph2::SetVertexBuffer()
    and nSceneGraph2::SetIndexBuffer().

     - 14-Mar-03    orpheus   now checks to make sure mesh is loaded

    @param  sceneGraph      pointer to scene graph object
*/
void
nMeshNode::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    // file name set?
    if (this->rsrc_path.GetPath())
    {
        // (re)-load mesh on demand
        if (!this->ref_vb.isvalid())
        {
            if (!this->loadMesh())
            {
                return;
            }
        }

        // "render" mesh
        sceneGraph->SetVertexBuffer(this->ref_vb.get());
        sceneGraph->SetIndexBuffer(this->ref_ibuf.get());
        if (this->castShadows && this->refShadowCaster.isvalid())
        {
            sceneGraph->SetShadowCaster(this->refShadowCaster.get());
        }
    }
}

//------------------------------------------------------------------------------
/**
	Load mesh using specified meshloader from the file defined by SetFilename().

	- 09-Dec-07   Ilya, created
	@return     true if success
*/
bool 
nMeshNode::load_mesh(nMeshLoader* loader)
{
	bool retval = false;
	if (loader)
	{	
		loader->Begin(this->refGfx.get(), this->refShadowServer.get(), this->readonly);
		const char* fileName = this->rsrc_path.GetAbsPath();
		if (loader->Load(fileName))
		{
			this->ref_vb          = loader->ObtainVertexBuffer();
			this->ref_ibuf        = loader->ObtainIndexBuffer();
			this->refShadowCaster = loader->ObtainShadowCaster();
			retval = true;
		}
		loader->End();
	}
	return retval;
}

//------------------------------------------------------------------------------
/**
    Load mesh from the file defined by SetFilename().

     - 30-Aug-01   floh    now uses external mesh loader class
     - 14-Mar-03   orpheus clears filename on failed load rather than crash
	 - 09-Dec-07   Ilya add usage of load_mesh
    @return     true if success
*/
bool 
nMeshNode::loadMesh(void)
{
    // use a mesh loader object to create the required objects
    bool retval = false;
    const char* fileName = this->rsrc_path.GetAbsPath();
	nMeshLoader* loader = 0;
    // select a mesh loader object based on file extension
    if (strstr(fileName, ".n3d"))
    {
        // a n3d file
        loader = new nN3dMeshLoader(kernelServer);		
    }
    else if (strstr(fileName, ".nvx"))
    {
        // a nvx file
		loader = new nNvxMeshLoader(kernelServer);
    }
	if (loader)
	{
		retval = this->load_mesh(loader);
		delete loader;
	}	

    if (!retval)
    {
        n_printf("nMeshNode: Failed to load mesh '%s'!\n", fileName);
        this->rsrc_path.Clear();
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

