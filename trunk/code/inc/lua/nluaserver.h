#ifndef N_LUASERVER_H
#define N_LUASERVER_H
//--------------------------------------------------------------------
//  nluaserver.h
//
//  (c) 2003  Matthew T. Welker & Vadim Macagon
//
//  nLuaServer is licensed under the terms of the Nebula License
//--------------------------------------------------------------------
/** 
    @class nLuaServer
    @ingroup NLuaServerContribModule

    @brief Lua 5 wrapper for Nebula

    Implements an nScriptServer that runs Lua 5, extended
    by default with a few Nebula specific commands.
    
    (C) 2003  Matthew T. Welker & Vadim Macagon
*/
#include <string.h>

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_SCRIPTSERVER_H
#include "kernel/nscriptserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nLuaServer
#include "kernel/ndefdllclass.h"

// Lua requires it's headers to be explicitly extern'd
extern "C" {
    #include "lua/lua.h"
    #include "lua/lauxlib.h"
    #include "lua/lualib.h"
}

/* compatibility code */
#define lua_baselibopen	luaopen_base
#define lua_tablibopen	luaopen_table
#define lua_iolibopen	luaopen_io
#define lua_strlibopen	luaopen_string
#define lua_mathlibopen	luaopen_math
#define lua_dblibopen	luaopen_debug


#define lua_boxpointer(L,u) \
	(*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))

#define lua_unboxpointer(L,i)	(*(void **)(lua_touserdata(L, i)))


//--------------------------------------------------------------------
class nFileServer2;
class nLuaServer : public nScriptServer {
public:
    static nClass *local_cl;
    static nKernelServer *kernelServer;
    nAutoRef<nFileServer2> refFileServer;
    static nLuaServer *Instance;
    stl_string classCacheName;
    stl_string thunkStoreName;

public:
    nLuaServer();
    virtual ~nLuaServer();
 
    virtual bool Run(const char *, const char*&);
    virtual bool RunScript(const char *, const char*&);
    virtual bool RunCommand(nCmd *);
    virtual char *Prompt(char *, int);
 
    virtual nFile* BeginWrite(const char* filename, nRoot* obj);  
    virtual bool WriteComment(nFile *, const char *);
    virtual bool WriteBeginNewObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteBeginNewObjectCmd(nFile *, nRoot *, nRoot *, nCmd *);
    virtual bool WriteBeginSelObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteCmd(nFile *, nCmd *);
    virtual bool WriteEndObject(nFile *, nRoot *, nRoot *);
    virtual bool EndWrite(nFile *);
 
    virtual bool Trigger(void);

    virtual lua_State* GetContext();
    
    // manipulate _nebthunks table
    static void RemoveThunk(lua_State*, void*);
    static void AddThunk(lua_State*, void*);
    static void FindThunk(lua_State*, void*);
    
    static void AddClassToCache(lua_State*, nClass*);
    static bool ThunkNebObject(lua_State*, nRoot*);
    static nRoot* UnpackThunkRoot( lua_State*, int );
    
    static void InArgsToStack(lua_State*, nCmd*);
    static void OutArgsToStack(lua_State*, nCmd*, bool);
    static bool StackToInArgs(lua_State*, nCmd*);
    static bool StackToOutArgs(lua_State*, nCmd*);
    static void ListArgToTable(lua_State*, nArg*, bool);
    
private:
    friend class nLuaScriptlet;

    static void ArgToStack( lua_State*, nArg* );
    static bool StackToArg( lua_State*, nArg*, int index );
    const char* _lua_tostring( lua_State*, int );
    void reg_globalfunc(lua_CFunction, const char*);
    void write_select_statement(nFile *, nRoot *, nRoot *);

    long indent_level;
    char indent_buf[N_MAXPATH];
    lua_State* L;   
    bool echo;
    bool selgrab;
    stl_string output;
};


//--------------------------------------------------------------------
#endif
