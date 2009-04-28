#define N_IMPLEMENTS nOdeHinge2Joint

//==============================================================================
//  subdir/nOdeHingeJoint_init.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/node_hinge2_joint.h"
#include "kernel/nkernelserver.h"

//  static member initialization
nClass* nOdeHinge2Joint::pClass = 0;
nKernelServer* nOdeHinge2Joint::pKernelServer = 0;

//  global declarations
extern void n_initcmds(nClass*);
extern char* nodehinge2joint_version;
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
    nOdeHinge2Joint::pClass = pClass;
    nOdeHinge2Joint::pKernelServer = pKernelServer;

    pKernelServer->AddClass("node_hinge_joint", pClass);

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
    nOdeHinge2Joint::pKernelServer->RemClass(nOdeHinge2Joint::pClass);
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
void*
n_create()
{
    return new nOdeHinge2Joint;
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
char*
n_version()
{
    return nodehinge2joint_version;
}
//------------------------------------------------------------------------------
