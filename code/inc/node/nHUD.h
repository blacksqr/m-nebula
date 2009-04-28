//==============================================================================
//  node/nHUD.h
//  author: gamingrat
//  (C) 2001 
//------------------------------------------------------------------------------

#ifndef N_NHUD_H
#define N_NHUD_H

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_SPECIALFXSERVER_H
#include "misc/nspecialfxserver.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#undef N_DEFINES
#define N_DEFINES nHUD

#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nHUD : public nVisNode
{
public:
    nHUD();
    virtual ~nHUD();
    
    static nKernelServer *kernelServer;

    virtual bool SaveCmds(nPersistServer*);
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*);  
	virtual void Initialize();

    virtual void SetColor(float red, float grn, float blu, float alpha);
    virtual void SetPosition(float ulx, float uly, float lrx, float lry);

protected:
    virtual void InitVBuffer();
    virtual void PlaceHUD();
    virtual void ColorHUD();
    virtual void UVHUD();
    
protected:    
    
    nDynVertexBuffer	ref_dynvbuf;
    nRef<nIndexBuffer>  ref_ibuf;
    nVertexBuffer*		vb_dest;	
    
    float fRed, fBlue, fGreen, fAlpha;
	float fULX, fULY, fLRX, fLRY;
};

#endif
