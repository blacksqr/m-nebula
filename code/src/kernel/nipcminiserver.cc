#define N_IMPLEMENTS nIpcMiniServer
#define N_KERNEL
//------------------------------------------------------------------------------
//  nipcminiserver.cc
//  (C) 2002 RadonLabs GmbH
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
#include "kernel/ntimeserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfiletransfer.h"

//------------------------------------------------------------------------------
/**
	List of system commands
	created 24-Sep-2008 Insightec, Inc -by Ilya Kliot

	"~close" - close session (not in use) success answer "~ok" 
	"~quit" - quit server success answer "~ok"
	"~wait" - server lock on receive - server will receive data until "~continue" will be sent 
	"~continue" - server unlock
	"~quiet" - server will not send script response to commands
	"~loud" - server will send script response to commands
	"~failexit" - server will quit on current session failure
	"~version" - server will answer by version number					
	"~keepalive" - server will send "~keepalive" back 
	"~backline" - turn this session to backline
*/

//#define USE_TIMEOUT
//------------------------------------------------------------------------------
/**
    Checks if an incoming message is available, if yes, reads the message
    and adds a new nMsgNode to the parent nIpcServer's message list.

    If no message is pending, just do nothing.

    Handles handshakes and close requests internally.

    @return     false if socket has been closed
*/
bool
nIpcMiniServer::Poll() 
{
	int wait = 0;
	int res = this->session.check_arrival();
	bool connected = this->session.connected();

	if (!connected)
	{
		if (this->exit_on_disconnect)
		{
			n_printf("%s socket failure, exit by request\n", (this->IsBackLine() ? "Backline" : "Main"));
			nRoot::ks->SetQuitRequest();
		}
		return true;
	}

	if ( res == 0)
		return true;	
	
#ifdef USE_TIMEOUT
	long start = this->session.get_milli();
#endif
	do 
	{	
		int len = this->session.receive_string();
		connected = (len >= 0);		

		if (len > 0) // normal case: a message was received!
		{	
#ifdef USE_TIMEOUT
			start = this->session.get_milli();
#endif
			char* current_str = this->session.get_buffer();
			int processed_len = 0;			
			while (processed_len < len)
			{
				int current_len = strlen(current_str);
				if (current_str[0] == '~') // check for special case command
				{			
					const char* result = 0;
//					stl_string messages;
					char syscmd[N_MAXPATH];
					strcpy(syscmd, current_str);

					char *cmd = strtok(syscmd, " \t~");
					if (strcmp(cmd, "handshake") == 0) 
					{
						// client checks for right portname
						char *pname = strtok(NULL," \t~");
						if (pname) 
						{
							if (strcmp(pname, this->server->pname.c_str()) == 0) 
							{
								result = "~true";
							}
							else
							{
								// ooops, wrong port, shut down connection
								result    = "~false";
								connected = false;
							}
						} 
						else 
						{
							n_error("nIpcMiniServer::n_miniserver_tfunc(): ~handshake command broken!");
						}
					} 
					else if (strcmp(cmd, "close") == 0) 
					{
						n_printf("Close request received on %s connection.\n", (this->IsBackLine() ? "backline" : "main"));
						result    = "~ok";
						connected = false;
					}  
					else if (strcmp(cmd, "quit") == 0) 
					{
						n_printf("Quit request received on %s connection.\n", (this->IsBackLine() ? "backline" : "main"));
						result = "~ok";
						nRoot::ks->SetQuitRequest();
					}  
					else if (strcmp(cmd, "wait") == 0)
					{
						wait++;
#ifdef _DEBUG_IPC
						n_printf("Remote server locked %d\n", wait);
#endif						
					}
					else if (strcmp(cmd, "continue") == 0)
					{
						wait--;
#ifdef _DEBUG_IPC
						n_printf("Remote server unlocked %d\n", wait);
#endif						
					}
					else if (strcmp(cmd, "quiet") == 0)
					{
						this->SetQuiet(true);
						result = "~ok";
					}
					else if (strcmp(cmd, "failexit") == 0)
					{
						this->exit_on_disconnect = true;
						result = "~ok";
					}
					else if (strcmp(cmd, "loud") == 0)
					{
						this->SetQuiet(false);
						result = "~ok";
					}
					else if (strcmp(cmd, "version") == 0)
					{
						nEnv* version = (nEnv*)nRoot::ks->Lookup("/sys/var/version");
						result = version->GetS();
					}
					else if (strcmp(cmd, "keepalive") == 0)
					{						
						this->session.send_string("~keepalive");
					}
					else if (strcmp(cmd, "backline") == 0)
					{
						this->backline = true;
						result = "~ok";						
					}
					else // other commands - probably file operations
					{ 
						
						if (strcmp(cmd, FT_SRV_EXISTS) == 0) // exists
						{ 
							char* fname = strtok(0, " \t~"); // get the name
							result = this->ft_exists(fname);                    
						} 
						else if (strcmp(cmd, FT_SRV_OPEN) == 0)  // fopen
						{					
							char* fname = strtok(0, " \t~"); // get the name
							char* mode = strtok(0, " \t~");	 // get the mode
							result = this->ft_open(fname, mode);
						} 
						else
						{
							const char* handle = strtok(0, " \t~");
							if (strcmp(cmd, FT_SRV_CLOSE) == 0) // fclose
							{ 						
								result = this->ft_close(handle);
							} 
							else if (strcmp(cmd, FT_SRV_WRITE) == 0) // fwrite
							{ 						
								int fsize = atoi(strtok(0, " \t~"));
								result = this->ft_write(handle, fsize);
							} 
							else if (strcmp(cmd,FT_SRV_READ) == 0) // fread
							{ 						
								long read_size = atoi(strtok(0, " \t~"));					
								result = this->ft_read(handle, read_size);					
							} 
							else if (strcmp(cmd, FT_SRV_ISOPEN) == 0) // isopen
							{ 						
								result = this->ft_isopen(handle);
							} 
							else if (strcmp(cmd, FT_SRV_SEEK) == 0) // fseek
							{ 						
								int	offset = atoi(strtok(0, " \t~"));
								char* origin = strtok(0, " \t~");
								result = this->ft_seek(handle, offset, origin);                    
							} 
							else if (strcmp(cmd, FT_SRV_TELL) == 0) // ftell
							{ 						
								result = this->ft_tell(handle);
							} 				
						}				
					}		
					// return answer to special case command
					if (result) 
					{
						this->session.send_string(result);
					} 					
				}             
				else // normal message, add to msg list and emit signal
				{
					nMsgNode *nd = n_new nMsgNode(current_str, current_len+1);
					nd->SetPtr((void *)this->id);
					this->server->msgList.Lock();
					this->server->msgList.AddTail(nd);
					this->server->msgList.Unlock();
					this->server->msgList.SignalEvent();				
				}				
				current_str += current_len + 1;
				processed_len += current_len + 1;				
			} // process lines
		} // received 
		else if (wait)
		{
			n_sleep(0.0);
		}
	} while (
		wait && connected
#ifdef USE_TIMEOUT
		&& this->session.get_milli() - start < 1000
#endif
		); 
	
	if (!connected)
	{
		if (this->exit_on_disconnect)
		{
			n_printf("%s socket failure, exit by request\n", (this->IsBackLine() ? "Backline" : "Main"));
			nRoot::ks->SetQuitRequest();
		}
		return true;
	}

    return connected;
}

//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
    28-May-99   floh    + memleak fixed if nobody connected to the
                          minimserver
*/
nIpcMiniServer::nIpcMiniServer(nIpcServer *_server) : 
	exit_on_disconnect(false),
	quiet(false),
	backline(false)
{
    n_assert(_server);

    this->server = _server;
    this->id     = this->server->numMiniServers++;
    this->srvrSocket = _server->sock; 	

    n_memset(&(this->clientAddr), 0, sizeof(this->clientAddr));

    this->server->miniServerList.Lock();
    this->server->miniServerList.AddTail(this);
    this->server->miniServerList.Unlock();	
}


//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
    31-Oct-98   floh    divided into Listen(), Accept() und Ignore()
*/
bool nIpcMiniServer::Listen()
{
#ifdef __GLIBC__
    unsigned int addrLen;
#else
    int addrLen;
#endif
    bool retval = false;
    
    // wait for a client...
    int lsres = 0;
    if ((lsres = listen(this->srvrSocket, 5)) != -1) 
    {
        addrLen = sizeof(this->clientAddr);        

        if (this->session.set(accept(this->srvrSocket, (struct sockaddr *)&(this->clientAddr), &addrLen))) 
        {
            n_printf("client %d: connection accepted.\n", (int) this);

            this->session.set_block_mode(false);
 
            retval = true;
        } 
        else 
        {
			lsres = this->session.check_error();
            n_printf("nIpcMiniServer::Listen(): accept() failed!, %d\n", lsres);
			
        }
    } 
    else 
    {   	
		lsres = this->session.check_error();
        n_printf("nIpcMiniServer::Listen(): listen() failed! %d", lsres);
    }

    return retval;
}          

//------------------------------------------------------------------------------
/**
    31-Oct-98   floh    created
*/
void 
nIpcMiniServer::Ignore()
{
	n_printf("nIpcMiniServer::Ignore() called\n");
	this->session.close("nIpcMiniServer::Ignore():");
}



//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
*/
bool 
nIpcMiniServer::Send(const char* msg, bool backline_only)
{
    n_assert(msg);		
	if (backline_only)
	{
		if (!this->backline)
			return false;		
	}	
		
	bool res = this->session.send_string(msg);	
	return res;
}

//------------------------------------------------------------------------------
/**
	completely send a data buffer to the client
	created 28-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
bool 
nIpcMiniServer::SendData(const char* buf_ptr, int size, bool backline_only)
{
	n_assert(buf_ptr);	
	if (backline_only)
	{
		if (!this->backline)
			return false;		
	}

	int sent = 0;
	bool connected = true;
	while (sent < size && connected) 
	{
		int res = this->session.send_data(buf_ptr + sent, size - sent);
		if (res > 0 ) 
		{
			sent += res;
		} 
		else if (res == SOCKET_ERROR) 
		{
			connected = false;
			n_printf("nIpcMiniServer::SendData() failed \n");
			break;
		} 
	}	
	return connected;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
