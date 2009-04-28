#define N_IMPLEMENTS nLuaServer
//--------------------------------------------------------------------
//  nluarun.cc -- Command processing
//
//  Derived from npythonrun.cc by Jason Asbahr
//  Derived from ntclrun.cc by Andre Weissflog
//  (c) 2003  Matthew T. Welker
//
//  nLuaServer is licensed under the terms of the Nebula License
//--------------------------------------------------------------------

#include "lua/nluaserver.h"
#include "kernel/nfileserver2.h"

#ifdef __VC__
    // VC6 warning ignorance
    #pragma warning(push)
    #pragma warning(disable:4800)
#endif

//--------------------------------------------------------------------
//  _lua_tostring(lua_State* L)
//--------------------------------------------------------------------
/**
    @brief _lua_tostring empties the Lua stack and returns a string
    represntation for the contents.
    
    @todo The list handling ought to recurse, but doing so crashes  
    the console if the returned value is a thunk.  rawget doesn't 
    seem to help this any either and it seems that Lua is still
    calling the __index metatable values.
*/
const char* nLuaServer::_lua_tostring( lua_State* L, int bottom )
{
    stl_string& buf = nLuaServer::Instance->output;
    buf.clear();
    while (bottom < lua_gettop(L))
    {
        switch (lua_type(L,-1))
        {
            case LUA_TBOOLEAN:
            {
                if(lua_toboolean(L,-1)) buf.append("true");
                else buf.append("false");
                break;  
            }
            case LUA_TNUMBER:
            case LUA_TSTRING:
            {
                buf.append(lua_tostring(L,-1));
                break;
            }
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA:
            {
                buf.append("<obect>");
                break;
            }
            case LUA_TNIL:
            {
                buf.append("<nil>");
                break;
            }
            case LUA_TTABLE:
            {
                buf.append("{ ");
                lua_pushnil(L);
                lua_gettable(L, -2);

                while (lua_next(L, -2) != 0) //pops a key and pushes key and value
                {
                    if(lua_isstring(L,-1))
                    {
                        buf.append(lua_tostring(L,-1));
                    }
                    lua_pop(L,1);
                    buf.append(" ");
                }
                lua_pop(L,1);
                buf.append(" }");
                break;
            }
            default:
                buf.append("???");
                break;  
        }
        lua_pop(L,-1);
    }
    return buf.c_str();
}

//--------------------------------------------------------------------
// Prompt(char* buf, int size);
//--------------------------------------------------------------------
char *nLuaServer::Prompt(char *buf, int size)
{
    char buffer[1024];
    const char* result;
    buffer[0] = 0;

    this->Run("return call('getfullname')", result);

    if (result)
        n_strcat(buffer, result, size-1);
        
    n_strcat(buffer, ">", size);
    n_strncpy2(buf, buffer, size);
    return buf;
}


//--------------------------------------------------------------------
//  Run(const char *cmd_str, const char *& result)
//--------------------------------------------------------------------
bool nLuaServer::Run(const char *cmd_str, const char*& result)
{
    n_assert(cmd_str);

    bool retval = luaL_dostring(this->L, cmd_str);

    result = _lua_tostring(this->L, 0);
    return !retval;
}


//--------------------------------------------------------------------
//  RunCommand()
//  Non script language specific way to execute script - in theory
//  For reference the first nArg is required to be a string
//  And ARGTYPE_LIST is never allowed on input in any case
//  NOTE:  This is always assumed to be a direct function call
//--------------------------------------------------------------------
/**
    @todo This function is ill defined - Find out if return values
    should be populated in the nCmd object.
*/
bool nLuaServer::RunCommand(nCmd *c) 
{
    c->Rewind();
    int len = c->GetNumInArgs();
    lua_pushstring(this->L, c->In()->GetS());
    lua_gettable(this->L, LUA_GLOBALSINDEX);

    for (int i = 1; i < len; i++)
    {
        nLuaServer::ArgToStack(this->L, c->In());
    }
    if(lua_pcall(this->L, len-1, 0, 0))
    {
        return false;
    }
    return true;
}



//--------------------------------------------------------------------
//  RunScript()
//  This function will allow explicit return statements from the
//  file - and requires it for output.
//--------------------------------------------------------------------
bool nLuaServer::RunScript(const char *fname, const char*& result)
{
    n_assert(fname);
    
    int filesize;
    char *cmdbuf;
    bool retval;
    
    nFileServer2* fileServer = this->kernelServer->GetFileServer2();
    nFile* nfile = fileServer->NewFileObject();
	stl_string path;
	fileServer->MakeAbsoluteMangled(fname, path);    
    if (!nfile->Open(path.c_str(), "r"))
    {
        result = 0;
        return false;
    }
    
    nfile->Seek(0, nFile::END);
    filesize = nfile->Tell();
    nfile->Seek(0, nFile::START);
    
    cmdbuf = (char*)n_malloc(filesize+1);
    nfile->Read(cmdbuf, filesize+1);
    cmdbuf[filesize] = 0;
    
    nfile->Close();
    n_delete nfile;
    
    retval = this->Run(cmdbuf, result);
    n_free(cmdbuf);
    return retval;
}


//--------------------------------------------------------------------
//  Trigger()
//--------------------------------------------------------------------
bool nLuaServer::Trigger(void)
{
    // The Trigger, she does nothing...
    return nScriptServer::Trigger();
}

#ifdef __VC__
    // VC6 thing
    #pragma warning(pop)
#endif

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

