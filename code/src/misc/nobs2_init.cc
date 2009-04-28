#define N_IMPLEMENTS nObserver2
//-------------------------------------------------------------------
//  nobserver.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nobserver2.h"

nClass *nObserver2::local_cl  = NULL;
nKernelServer *nObserver2::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nObserver2_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nObserver2::local_cl = cl;
    nObserver2::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nObserver2::local_cl);
    else n_error("Could not open superclass 'nroot'.\n");
    
    n_initcmds(cl);
	
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nObserver2::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nObserver2::local_cl);
        nObserver2::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nObserver2;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nObserver2_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
