#define N_IMPLEMENTS nOdeHingeJoint

//==============================================================================
//  subdir/nOdeHingeJoint_init.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_hinge_joint.h"
#include "kernel/nkernelserver.h"

//  static member initialization
nClass* nOdeHingeJoint::pClass = 0;
nKernelServer* nOdeHingeJoint::pKernelServer = 0;

//  global declarations
extern void n_initcmds(nClass*);
extern char* nodehingejoint_version;
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
    nOdeHingeJoint::pClass = pClass;
    nOdeHingeJoint::pKernelServer = pKernelServer;

    pKernelServer->AddClass("node_ball_joint", pClass);

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
    nOdeHingeJoint::pKernelServer->RemClass(nOdeHingeJoint::pClass);
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
void*
n_create()
{
    return new nOdeHingeJoint;
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
char*
n_version()
{
    return nodehingejoint_version;
}
//------------------------------------------------------------------------------
