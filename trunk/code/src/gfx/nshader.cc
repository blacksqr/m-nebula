#define N_IMPLEMENTS nShader
//------------------------------------------------------------------------------
// nshader.cc
//  (C) 2007 Insightec Ltd, -- I.Kliot
//------------------------------------------------------------------------------
#include "gfx/nshader.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nShader, "nroot");

bool nShader::Load(const char* fileName, nShaderType _type) 
{
	this->type = _type;
	return this->file2Buffer(fileName);
}

bool nShader::file2Buffer(const char* fname) {
	nFileServer2* fs = this->kernelServer->GetFileServer2();

	if (fs->FileExists(fname)) {
		nFile* file = fs->NewFileObject();

		file->Open(fname, "rb");		
		
		unsigned long fsize = file->Size();		

		this->data.resize(fsize);
		file->Read(data.get_buffer(), fsize); 
		file->Close();
		delete file;
		return true;
	}
	return false;
}