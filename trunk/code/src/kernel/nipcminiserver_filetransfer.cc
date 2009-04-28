#define N_IMPLEMENTS nIpcMiniServer
#define N_KERNEL
//------------------------------------------------------------------------------
//  nipcminiserver_filetransfer.cc
//  (C) 2007 by Ilya Kliot Insightec, Inc
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "util/nstl.h"

#if defined(__LINUX__) || defined(__MACOSX__)
#include "fcntl.h"
#endif

#include "kernel/nipcserver.h"
#include "kernel/nthread.h"
#include "kernel/nipcminiserver.h"
#include "kernel/nroot.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"

#include "kernel/nfiletransfer.h"
#include "kernel/nsession.h"


//------------------------------------------------------------------------------
/**
    Converts handle in ascii format to nFile pointer
	@return nFile* pointer to nFile class instance
*/
nFile* nIpcMiniServer::strHandle2nFile(const char* shandle)
{	
	if (shandle) 
	{
		nFileServer2* fs = nRoot::ks->GetFileServer2();
		nFile* f = fs->GetFileByIndex(atoi(shandle));		
		return f;		
	}
	return 0;
}


//------------------------------------------------------------------------------
/**
    Opens file by command received over ip, 
	@return handle >0 if success, -1 if failed
*/
char* nIpcMiniServer::ft_open(const char* fname, const char* mode)
{
	char* result = "-1";
	
	if (fname && mode) 
	{
		nFileServer2* fs = nRoot::ks->GetFileServer2();
		nFile* f = fs->NewFileObject();

		if (f && f->Open(fname, mode)) 
		{
			sprintf(this->answer, "%d", f->GetIndex());
			result = this->answer;
		} 
		else 
		{
			n_printf("failed to open file %s with mode %s over socket\n", fname, mode);
		}
	} 
	else 
	{
		n_printf("Failed to open file over socket because of name %s and mode %s\n", 
			(fname ? fname : "not specified"), (mode ? mode : "not specified"));
	}
	return result;
}


//------------------------------------------------------------------------------
/**
    Close file by hanlde received over ip
	@return FT_DONE if success, FT_FAIL if failed	
*/
char* nIpcMiniServer::ft_close(const char* handle)
{
	char* result = FT_FAIL;
	if (handle) 
	{
		nFile* f = nIpcMiniServer::strHandle2nFile(handle);						
		if (f) 
		{ 
			result = FT_DONE;
			f->Close();
			delete f;
		}
	}	
	return result;
}

//------------------------------------------------------------------------------
/**
    Write to nFile specified by handle size of bytes of data came overe ip
	
	@return FT_DONE if success, FT_FAIL if failed	
*/
char* nIpcMiniServer::ft_write(const char* handle, int fsize) 
{
	char* result = FT_DONE;
	nFile* f = nIpcMiniServer::strHandle2nFile(handle);
	

	if (!(f && fsize)) 
	{
		result = FT_FAIL;
	}
	else if (f->IsOpen())
	{
		
		this->session.send_string(FT_GO);
#ifdef FILE_TRANSFER_PROFILE
		ft_profiler profiler;
		n_printf(profiler.begin(fsize));
#endif
		int total_received = 0;
		
		static byte_buffer fbuffer;
		if (!fbuffer.size())
			fbuffer.resize(FT_PACKET_SIZE);
		
		int cur = f->Tell();
		f->Seek(fsize, nFile::CURRENT);
		f->Seek(cur, nFile::START);
		
		while (total_received < fsize) 
		{							
			int received =  this->session.receive_data(fsize - total_received, (char*)fbuffer.get_buffer(), FT_PACKET_SIZE);
			
			if (received > 0)  
			{
#ifdef FILE_TRANSFER_PROFILE
				profiler.step(received);
#endif
				total_received += received;
				f->Write((char*)fbuffer.get_buffer(), received);
			} 
			else if (received < 0)  
			{
				result = FT_FAIL;				
				n_printf("failed to receive file %d \n", f->GetIndex());
				break;
			}
		}
		//fbuffer.clear();

#ifdef FILE_TRANSFER_PROFILE
		n_printf(profiler.end(result));
#endif
	} 
	else 
	{
		n_printf("file is not open\n");
		result = FT_FAIL;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
    Read from nFile specified by handle size of bytes of data came overe ip
	
	@return FT_DONE if success, FT_FAIL if failed	
*/
char* nIpcMiniServer::ft_read(const char* handle, int read_size) 
{
	char* result = 0;
	nFile* f = nIpcMiniServer::strHandle2nFile(handle);

	if (!(f && read_size)) 
	{
		return FT_FAIL;
	}
	else if (f->IsOpen())
	{
		int cur_pos = f->Tell();
		f->Seek(0, nFile::END);
		int end_pos = f->Tell();
		f->Seek(cur_pos, nFile::START);

		int frest = end_pos - cur_pos;
		if (frest <= 0) 
		{
			return FT_EOF;
		} 
		else 
		{
			if (frest < read_size)
			{
				read_size = frest;
			}
			sprintf(this->answer, "%d", read_size);
			this->session.send_string(this->answer);
			this->session.receive_string();

			if (strcmp(this->session.get_buffer(), FT_GO))
			{
				return FT_FAIL;
			}
			
#ifdef FILE_TRANSFER_PROFILE
			ft_profiler profiler;
			n_printf(profiler.begin(read_size));
#endif		
			int total_sent = 0;
			static byte_buffer fbuffer;
			if (!fbuffer.size())
				fbuffer.resize(FT_PACKET_SIZE);
			bool connected = true;

			while ((total_sent < read_size) && connected) 
			{
				int rest = read_size - total_sent;
				int to_sent = n_min(rest, FT_PACKET_SIZE);

				f->Read(fbuffer.get_buffer(), to_sent);
				
				int sent = 0;

				while (sent < to_sent) 
				{							
					int res = this->session.send_data((char*)fbuffer.get_buffer() + sent, to_sent - sent);
					if (res > 0 ) 
					{
						sent += res;
#ifdef FILE_TRANSFER_PROFILE
						profiler.step(res);
#endif
					} 
					else if (res == SOCKET_ERROR) 
					{
						result = FT_FAIL;
						connected = false;
						n_printf("failed to send file %d \n", f->GetIndex());
						break;
					} 
				}
				total_sent += sent;
			}
#ifdef FILE_TRANSFER_PROFILE
			n_printf(profiler.end(result));
#endif
		} // not eof
	} // got f and size                    
	else 
	{
		n_printf("file is not open\n");
		result = FT_FAIL;
	}
	return result;
}


//------------------------------------------------------------------------------
/**
    Check if file is open by hanlde received over ip
	
	@return FT_TRUE if open, FT_FALSE if not and FT_FAIL if failed
*/
char* nIpcMiniServer::ft_isopen(const char* handle)
{
	char* result = FT_FAIL;
	nFile* f = nIpcMiniServer::strHandle2nFile(handle);
	if (f)  
	{
		if (f->IsOpen())
			result = FT_TRUE;
		else
			result = FT_FALSE;
	} 	
	return result;
}


//------------------------------------------------------------------------------
/**
    Set position in file defined by hanlde received over ip

	@return FT_TRUE if done, FT_FALSE if not and FT_FAIL if protocol failed	
*/
char* nIpcMiniServer::ft_seek(const char* handle, int offset, const char* origin)
{
	char* result = FT_FAIL;
	nFile* f = nIpcMiniServer::strHandle2nFile(handle);					
	if (f && origin)  
	{
		if (f->IsOpen())
		{
			int res = -1;
			if (strcmp(origin, FT_SEEK_START) == 0) 
				res = (f->Seek(offset, nFile::START) ? 1 : 0);
			else if (strcmp(origin, FT_SEEK_END) == 0) 
				res = (f->Seek(offset, nFile::END) ? 1 : 0);
			else if (strcmp(origin, FT_SEEK_CURRENT) == 0) 
				res = (f->Seek(offset, nFile::CURRENT) ? 1 : 0);							

			if (res == 1)
				result = FT_TRUE;
			else if (res == 0)
				result = FT_FALSE;
			else 
				result = FT_FAIL;														
		}
		else
			result = FT_FAIL;
	} 	
	return result;
}

//------------------------------------------------------------------------------
/**
    Get position in file defined by handle received over ip
	
	@return position in ASCII if done, FT_FAIL if failed	
*/
char* nIpcMiniServer::ft_tell(const char* handle)
{
	char* result = FT_FAIL;
	nFile* f = nIpcMiniServer::strHandle2nFile(handle);
	if (f) 
	{
		sprintf(this->answer, "%d", f->Tell());
		result = this->answer;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
    Check the existance of file with given name
	
	@return FT_TRUE if exists, FT_FALSE if not 
*/	
char* nIpcMiniServer::ft_exists(const char* name)
{
	char* result = FT_FALSE;
	nFileServer2* fs = nRoot::ks->GetFileServer2();

	if (fs && fs->FileExists(name))
		result = FT_TRUE;
	return result;	
}
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
