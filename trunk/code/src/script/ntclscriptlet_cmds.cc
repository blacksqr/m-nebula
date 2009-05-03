#define N_IMPLEMENTS nTclScriptlet
#include "script/ntclscriptlet.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
//  © 2001 Radon Labs GmbH

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntclscriptlet

    @superclass
    nscriptlet

    @classinfo
    Tcl specific scriptlet class.
*/

void n_initcmds(nClass* cl)
{
	cl->BeginCmds();
	cl->EndCmds();
}
