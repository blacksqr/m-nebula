#define N_IMPLEMENTS nOdeNode

//==============================================================================
//  subdir/nOdeNode_init.cc
//  author: Your Name
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

//  includes
#include "nemesis/nodenode.h"
#include "kernel/nkernelserver.h"

//  static member initialization
nClass* nOdeNode::pClass = 0;
nKernelServer* nOdeNode::pKernelServer = 0;

//  global declarations
extern void n_initcmds(nClass*);
extern char* nodenode_version;
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
    nOdeNode::pClass = pClass;
    nOdeNode::pKernelServer = pKernelServer;

    pKernelServer->AddClass("nvisnode", pClass);

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
    nOdeNode::pKernelServer->RemClass(nOdeNode::pClass);
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
void*
n_create()
{
    return new nOdeNode;
}
//==============================================================================
//  2000.mm.dd  your name       created
//------------------------------------------------------------------------------
N_EXPORT
char*
n_version()
{
    return nodenode_version;
}
//------------------------------------------------------------------------------
