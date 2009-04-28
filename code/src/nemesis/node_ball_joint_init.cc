#define N_IMPLEMENTS nOdeBallJoint

//==============================================================================
//  subdir/nOdeBallJoint_init.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_ball_joint.h"
#include "kernel/nkernelserver.h"

//  static member initialization
nClass* nOdeBallJoint::pClass = 0;
nKernelServer* nOdeBallJoint::pKernelServer = 0;

//  global declarations
extern void n_initcmds(nClass*);
extern char* nodeballjoint_version;
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
    nOdeBallJoint::pClass = pClass;
    nOdeBallJoint::pKernelServer = pKernelServer;

    pKernelServer->AddClass("node_joint", pClass);

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
    nOdeBallJoint::pKernelServer->RemClass(nOdeBallJoint::pClass);
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
void*
n_create()
{
    return new nOdeBallJoint;
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
char*
n_version()
{
    return nodeballjoint_version;
}
//------------------------------------------------------------------------------
