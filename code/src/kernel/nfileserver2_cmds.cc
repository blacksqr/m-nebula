#define N_IMPLEMENTS nFileServer2
#define N_KERNEL
//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfileserver2.h"
#include "kernel/npersistserver.h"

static void n_setassign(void* slf, nCmd* cmd);
static void n_getassign(void* slf, nCmd* cmd);
static void n_manglepath(void* slf, nCmd* cmd);
static void n_fileexists(void* slf, nCmd* cmd);
static void n_filesize(void* slf, nCmd* cmd);

static void n_setdevmode(void* slf, nCmd* cmd);
static void n_getdevmode(void* slf, nCmd* cmd);


//------------------------------------------------------------------------------
/**
    @scriptclass
    nfileserver2
    
    @superclass
    nroot

    @classinfo
    da nu fileserver 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setassign_ss", 'SASS', n_setassign);
    clazz->AddCmd("s_getassign_s",  'GASS', n_getassign);
    clazz->AddCmd("s_manglepath_s", 'MNGP', n_manglepath);
    clazz->AddCmd("b_fileexists_s", 'FEXT', n_fileexists);
	clazz->AddCmd("i_filesize_s",	'FSZE', n_filesize);
    clazz->AddCmd("v_setdevmode_b", 'SDVM', n_setdevmode);
    clazz->AddCmd("b_getdevmode_v", 'GDVM', n_getdevmode);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setassign

    @input
    s assign name
    s path

    @output
    v

    @info
    defines an assign with the specified name and links it to the specified
	path
*/
static
void
n_setassign(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetAssign(s0, s1);
}
//------------------------------------------------------------------------------
/**
    @cmd
    getassign

    @input
    s assign name

    @output
	s path

    @info
	siehe setassign
*/
static
void
n_getassign(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetS(self->GetAssign(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    fileexists

    @input    
	s - path

    @output
    b - success

    @info
    checks file existence in the path
*/
static
void
n_fileexists(void* slf, nCmd* cmd) {
    nFileServer2* self = (nFileServer2*) slf;
    const char* s = cmd->In()->GetS();    
    cmd->Out()->SetB(self->FileExists(s));
}

//------------------------------------------------------------------------------
/**
    @cmd
    filesize

    @input    
	s - path

    @output
    i - size, -1 if file not exists

    @info
    returns file size, -1 if file not exists
*/
static
void
n_filesize(void* slf, nCmd* cmd) {
    nFileServer2* self = (nFileServer2*) slf;
    const char* s = cmd->In()->GetS();    
    cmd->Out()->SetI(self->FileSize(s));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdevmode

    @input
    b devmode	

    @output
    v

    @info
    set fileserver2 to developer mode, in developer mode file server will check first
    existance of file in the real file system.
*/
static
void n_setdevmode(void* slf, nCmd* cmd) {
    nFileServer2* self = (nFileServer2*) slf;    
    self->SetDevMode(cmd->In()->GetB());
    
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdevmode

    @input
    v 

    @output
    b devmode

    @info
    returns true if fileserver2  in developer mode
*/
static
void n_getdevmode(void* slf, nCmd* cmd) {
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetB(self->GetDevMode());
}

//------------------------------------------------------------------------------
/**
    @cmd
    manglepath

    @input
    s(UnmangledPath)

    @output
    s(MangledPath)

    @info
    Convert a path with assigns into a native absolute path.
*/
static
void
n_manglepath(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
	stl_string path;
	self->ManglePath(cmd->In()->GetS(), path);
    cmd->Out()->SetS(path.c_str());
}
