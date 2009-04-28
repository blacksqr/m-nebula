#define N_IMPLEMENTS nApplication
//------------------------------------------------------------------------------
//  (C) 2009	plushy
//------------------------------------------------------------------------------
#include "app/napplication.h"
#include "kernel/npersistserver.h"

static void n_addserver(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    napplication
    
    @superclass
    nroot

    @classinfo
	application general flow controller    
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_addserver_ss", 'ADSV', n_addserver);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addserver

    @input
    s s - class name, path

    @output
    v

    @info
    add server (actually any node, but purpose is servers) to application
*/
static void n_addserver(void* slf, nCmd* cmd)
{
    nApplication* self = (nApplication*) slf;
	const char* class_name = cmd->In()->GetS();
	const char* path = cmd->In()->GetS();
	self->AddServer<nRoot>(nApplication::kernelServer, class_name, path);
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nApplication::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'ADSV');

		for (std::list<nRoot*>::iterator i = this->servers.begin(); i != this->servers.end(); i++) 
		{
			nRoot* item = *i;
			cmd->In()->SetS(item->GetClass()->GetName());
			cmd->In()->SetS(item->GetFullName().c_str());	
			ps->PutCmd(cmd);
        }
		

        return true;
    }
    return false;
}

