#define N_IMPLEMENTS nLuaScriptlet
//--------------------------------------------------------------------
//  nluaserver.cc
//  derived from nscriptlet and friends (C) 2001 RadonLabs GmbH
//
//  (c) 2003  Matthew T. Welker
//
//  nLuaScriptlet is licensed under the terms of the Nebula License
//--------------------------------------------------------------------

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#ifndef N_LUASCRIPTLET_H
#include "lua/nluascriptlet.h"
#endif

nNebulaClass(nLuaScriptlet, "nscriptlet");

nLuaScriptlet::nLuaScriptlet()
{
    // empty
}

nLuaScriptlet::~nLuaScriptlet()
{
    // empty
}

bool
nLuaScriptlet::ParseFile(const char* file)
{
    // For now this should simply load the
    // contents of the file entire into
    // the buffer.  This can be optimized
    // later in a number of ways.
    nFile* nfile = this->refFile->NewFileObject();
    if (nfile->Open(file, "r"))
    {
        int nsize;
        nfile->Seek(0, nFile::END);
        nsize = nfile->Tell();
        nfile->Seek(0, nFile::START);
        this->script = (char*)n_malloc(nsize+1);
        this->script[nsize] = 0;
        nfile->Read(this->script, nsize);
        nfile->Close();
        n_delete nfile;
        return true;
    }
    n_printf("%s was unable to open the file: %s\n", this->GetName(), file);
    n_delete nfile;
    return false;
}

