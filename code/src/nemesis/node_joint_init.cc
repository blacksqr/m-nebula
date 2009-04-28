#define N_IMPLEMENTS nOdeJoint

//==============================================================================
//  subdir/nOdeJoint_init.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_joint.h"
#include "kernel/nkernelserver.h"

//  static member initialization
nClass* nOdeJoint::pClass = 0;
nKernelServer* nOdeJoint::pKernelServer = 0;

//  global declarations
extern void n_initcmds(nClass*);
extern char* nodejoint_version;
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
    nOdeJoint::pClass = pClass;
    nOdeJoint::pKernelServer = pKernelServer;

    pKernelServer->AddClass("nroot", pClass);

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
    nOdeJoint::pKernelServer->RemClass(nOdeJoint::pClass);
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
void*
n_create()
{
    return new nOdeJoint;
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
char*
n_version()
{
    return nodejoint_version;
}
//------------------------------------------------------------------------------
