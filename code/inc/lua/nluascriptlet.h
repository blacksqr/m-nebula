#ifndef N_LUASCRIPTLET_H
#define N_LUASCRIPTLET_H
//--------------------------------------------------------------------
//  nluaserver.h
//  derived from nscriptlet and friends (C) 2001 RadonLabs GmbH
//
//  (c) 2003  Matthew T. Welker
//
//  nLuaScriptlet is licensed under the terms of the Nebula License
//--------------------------------------------------------------------
/**
    @class nLuaScriptlet
    @ingroup NLuaServerContribModule
    @brief An nScriptlet implementation for Lua
    
    A bargain basement usage of nScriptlets for use in nLuaServer.

   (C) 2003    Matthew Welker
*/

#ifndef N_SCRIPTLET_H
#include "script/nscriptlet.h"
#endif

#undef N_DEFINES
#define N_DEFINES nLuaScriptlet
#include "kernel/ndefdllclass.h"

class nFileServer;
class nScriptServer;
class N_PUBLIC nLuaScriptlet : public nScriptlet
{
public:
    static nKernelServer* kernelServer;

    /// default constructor.
    nLuaScriptlet();
    /// destructor.
    virtual ~nLuaScriptlet();

    /// load scriptlet from a file
    virtual bool ParseFile(const char* file);
};

#endif
