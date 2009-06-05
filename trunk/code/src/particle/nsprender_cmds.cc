#define N_IMPLEMENTS nSpriteRender
//-------------------------------------------------------------------
//  nsprender_cmds.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "particle/nspriterender.h"
//------------------------------------------------------------------------------
/**
    @scriptclass
    nspriterender

    @superclass
    nprender

    @classinfo
    Render particles as viewer aligned sprites. 
*/
  
void n_initcmds(nClass *cl)
{
	cl->BeginCmds();
	cl->EndCmds();
}
