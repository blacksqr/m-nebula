#define N_IMPLEMENTS nOdePhysics

//==============================================================================
//  subdir/nOdePhysics_init.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_physics.h"
#include "kernel/nkernelserver.h"

//  static member initialization
nClass* nOdePhysics::pClass = 0;
nKernelServer* nOdePhysics::pKernelServer = 0;
int nOdePhysics::unique = 0;
double nOdePhysics::myLastTime = 0.0;

//  global declarations
extern void n_initcmds(nClass*);
extern char* nodephysics_version;
extern "C" bool N_EXPORT n_init(nClass*, nKernelServer*);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void* n_create(void);
extern "C" N_EXPORT char* n_version(void);

//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
bool
n_init(nClass* pClass, nKernelServer* pKernelServer)
{
    nOdePhysics::pClass = pClass;
    nOdePhysics::pKernelServer = pKernelServer;

    pKernelServer->AddClass("ncollideserver", pClass);

    n_initcmds(pClass);
    return true;
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
void
n_fini()
{
    nOdePhysics::pKernelServer->RemClass(nOdePhysics::pClass);
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
void*
n_create()
{
    return new nOdePhysics;
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
char*
n_version()
{
    return nodephysics_version;
}
//------------------------------------------------------------------------------
