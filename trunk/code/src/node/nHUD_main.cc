#define N_IMPLEMENTS nHUD

//==============================================================================
//  node/nHUD_main.cc
//  author: gamingrat
//  (C) 2001 
//------------------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "node/nHUD.h"

nNebulaScriptClass(nHUD, "nvisnode");
//------------------------------------------------------------------------------
/**
	n3DNode requires write access to the 1-dimensional lod channel.

	@param  channelSet      the nChannelSet object to update
	2001.10.25  gamingrat    created
	- May-16-2007	Ilya, removed unused refs
*/
nHUD::nHUD() :
    ref_dynvbuf(kernelServer, this),
    fRed(1.0f),
    fGreen(1.0f),
    fBlue(1.0f),
    fAlpha(1.0f),
	fULX(0.0f),
	fULY(0.0f),
	fLRX(1.0f),
	fLRY(1.0f),
    vb_dest(0),
	ref_ibuf(this)
{    
	this->SetHUD(true);
}
//------------------------------------------------------------------------------
/**
*/
nHUD::~nHUD() 
{
    // release index buffer
	release_ref(this->ref_ibuf);
}

//------------------------------------------------------------------------------
/**
*/
void nHUD::Initialize()
{
	if (!this->ref_dynvbuf.IsValid())
	{
		this->InitVBuffer();
	}
}

//------------------------------------------------------------------------------
/**
*/
void nHUD::SetColor(float red, float green, float blue, float alpha)
{    
    this->fRed   = red;
    this->fGreen = green;
    this->fBlue  = blue;
    this->fAlpha = alpha;
}

//------------------------------------------------------------------------------
/**
*/
void nHUD::SetPosition(float ulx, float uly, float lrx, float lry)
{    
    this->fULX   = ulx;
    this->fULY   = uly;
    this->fLRX   = lrx;
    this->fLRY   = lry;
}

//------------------------------------------------------------------------------
/**
	KEY:
	1)  ensure that Attach is not called before the HUD has been
	    completely defined and initialized and parent's Attach has
	    successfully returned
*/
bool nHUD::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);	

    if (nVisNode::Attach(sceneGraph))
    {        
        sceneGraph->AttachVisualNode(this);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
  22-May-2007 Ilya push/pop matrix and identity load removed beacuse 
				of new HUD flag in visnode

	KEY:
	1)  ensure that Compute is not called before the HUD has been
	    completely defined and inititalized and parent's Compute has
	    successfully returned
	
*/
void nHUD::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);    
    nVisNode::Compute(sceneGraph);

    // store the current model-view and projection matrices
//  commented by Ilya, Clockwise,
//  unused GetMatrix, PushMatrix saves it as well
//        this->refGfx->GetMatrix(N_MXM_MODELVIEW, mModelView);
//        this->refGfx->GetMatrix(N_MXM_PROJECTION, mProjection);

    this->vb_dest = this->ref_dynvbuf.Begin(
            this->ref_ibuf.get(), 
            sceneGraph->GetPixelShader(),
            sceneGraph->GetTextureArray());

    this->PlaceHUD();
    this->ColorHUD();
    this->UVHUD();	

    this->ref_dynvbuf.End(4, this->ref_ibuf.get()->GetNumIndices());
}

//------------------------------------------------------------------------------
/**
	2001.10.25  gamingrat      created
*/
void nHUD::InitVBuffer()
{
    //  assert precondition(s)
    n_assert(!this->ref_ibuf.isvalid());

    int iNumberOfVertices = 4;    // 4 vert's per quad (and we have got only one)
    this->ref_dynvbuf.Initialize((N_VT_COORD|N_VT_RGBA|N_VT_UV0), iNumberOfVertices);


    nIndexBuffer *ibuf = this->refGfx->FindIndexBuffer("nHUD_ibuf");
    if (!ibuf)
    {
        ibuf = this->refGfx->NewIndexBuffer("nHUD_ibuf");

        int iNumberOfIndices =  6;     // 6 indices per quad (and there still is only one)
        ibuf->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, iNumberOfIndices);

        int indexIndex  = 0;        
        for (ushort vertexIndex = 0; vertexIndex < 4; vertexIndex += 4)
        {
            //  upper-left triangle of quad
            ibuf->Index(indexIndex++, vertexIndex);
            ibuf->Index(indexIndex++, vertexIndex + 1);
            ibuf->Index(indexIndex++, vertexIndex + 2);

            //  lower-right triangle of quad
            ibuf->Index(indexIndex++, vertexIndex + 2);
            ibuf->Index(indexIndex++, vertexIndex + 3);
            ibuf->Index(indexIndex++, vertexIndex);
        }

        ibuf->End();
    }

    n_assert(ibuf);
    this->ref_ibuf = ibuf;
}

//------------------------------------------------------------------------------
/**
	2001.10.25  gamingrat      created
*/
void nHUD::UVHUD()
{
    //  assert precondition(s)
    n_assert(vb_dest);

    vector2 c1(1.0f, 1.0f);
    vector2 c2(0.0f, 1.0f);
    vector2 c3(0.0f, 0.0f);
    vector2 c4(1.0f, 0.0f);
    
    int iCnt = 0;
    for (int i = 0; i < 4; i += 4, iCnt++)
    {
        // n_assert(iCnt < iNumberOfFlares);

        vb_dest->Uv(i    , 0, c1); // uv upper right
        vb_dest->Uv(i + 1, 0, c2); // uv up
        vb_dest->Uv(i + 2, 0, c3); // uv origin
        vb_dest->Uv(i + 3, 0, c4); // uv right
    }
}

//------------------------------------------------------------------------------
/**
2001.10.25  gamingrat      created
*/
void nHUD::PlaceHUD()
{
    //  assert precondition(s)
    n_assert(vb_dest);

    //float fARAdj = 0.75f;   // adjusts for aspect ratio so HUD is square
    vector3 v(0.0f, 0.0f, 0.0f);

    int iCnt = 0;
    for (int i = 0; i < 4; i += 4, iCnt++)
    {		
		v.x = fLRX;
        v.y = fLRY;
        vb_dest->Coord(i, v);

        v.x = fULX;
        v.y = fLRY;
        vb_dest->Coord(i + 1, v);

        v.x = fULX;
        v.y = fULY;
        vb_dest->Coord(i + 2, v);

        v.x = fLRX;
        v.y = fULY;
        vb_dest->Coord(i + 3, v);
    }
}

//------------------------------------------------------------------------------
/**
	2001.10.25  gamingrat      created
*/
void nHUD::ColorHUD()
{            
    //  assert precondition(s)
    n_assert(vb_dest);

    nColorFormat color_format = vb_dest->GetColorFormat();

    int iCnt = 0;
    for (int i = 0; i < 4; i += 4, iCnt++)
    {
        //n_assert(iCnt < iNumberOfFlares);

        float r = fRed;
        float g = fGreen;
        float b = fBlue;
        float a = fAlpha;

        ulong c;
        if (N_COLOR_RGBA == color_format) 
            c = n_f2rgba(r,g,b,a);
        else
            c = n_f2bgra(r,g,b,a);

        vb_dest->Color(i, c);
        vb_dest->Color(i + 1, c);
        vb_dest->Color(i + 2, c);
        vb_dest->Color(i + 3, c);

    }
}

//------------------------------------------------------------------------------
