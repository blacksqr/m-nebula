#define N_IMPLEMENTS nAnimNode
//-------------------------------------------------------------------
//  nanimnode.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "gfx/nchannelset.h"
#include "node/nanimnode.h"

nNebulaScriptClass(nAnimNode, "nvisnode");
//--------------------------------------------------------------------
//  nAnimNode()
//  25-Jan-99   floh    created
//--------------------------------------------------------------------
nAnimNode::nAnimNode() : 
    repType(N_REPTYPE_LOOP),
    localChannelIndex(0),
    scale(1.0f)
{
    strcpy(this->channelName, "time");
}

//--------------------------------------------------------------------
//  ~nAnimNode()
//  25-Jan-99   floh    created
//--------------------------------------------------------------------
nAnimNode::~nAnimNode()
{ }

//--------------------------------------------------------------------
//  AttachChannels()
//  05-Jun-01   floh    created
//--------------------------------------------------------------------
void nAnimNode::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);
    this->localChannelIndex = chnSet->AddChannel(this->refChannel.get(), this->channelName);
    nVisNode::AttachChannels(chnSet);
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

    
