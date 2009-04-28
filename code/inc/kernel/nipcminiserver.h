#ifndef N_IPCMINISERVER_H
#define N_IPCMINISERVER_H
//------------------------------------------------------------------------------
/**
    A nIpcServer creates one nIpcMiniServer for each connecting client.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_NODE_H
#include "util/nnode.h"
#endif

#include "kernel/nsession.h"


//------------------------------------------------------------------------------
class nIpcServer;
class nThread;
class nFile;
class nIpcMiniServer : public nNode
{
public:
    /// constructor
    nIpcMiniServer(nIpcServer* _server);
    /// destructor
	//virtual ~nIpcMiniServer();
    /// listen on socket and wait for new client
    bool Listen();
    /// ignore the new client for any reason
    void Ignore();
    /// check for and pull incoming messages, call this frequently!
    bool Poll();
    /// send a string message to the client
    bool Send(const char* msg, bool backline_only = false);
	/// completely send a data buffer to the client
	bool SendData(const char* buf, int sz, bool backline_only = false);	
	/// set script answering mode
	void SetQuiet(bool b ) { this->quiet = b; }
	/// get script answering mode
	bool GetQuiet() const { return this->quiet; }
	/// returns true in case socket is valid
	bool IsValid() const { return (this->session.connected());}
	/// return true if this server for a back line
	bool IsBackLine() const { return this->backline; }

private: // 
    friend class nIpcServer;

private: // file operations
	/// open file by command received over ip, returns handle if success, -1 if failed
	char* ft_open(const char* fname, const char* mode);

	/// close file by hanlde received over ip, returns FT_DONE if success, FT_FAIL if failed
	char* ft_close(const char* handle);	

	/// write to nFile specified by handle size of bytes of data came overe ip
	char* ft_write(const char* handle, int size);	

	/// read from nFile specified by handle size of bytes of data came overe ip
	char* ft_read(const char* handle, int size);	
	
	/// check if file defined by hanlde is open, returns FT_TRUE if open, FT_FALSE if not and FT_FAIL if failed
	char* ft_isopen(const char* handle);

	/// set position in file defined by hanlde received over ip, returns FT_TRUE if done, FT_FALSE if not and FT_FAIL if protocol failed
	char* ft_seek(const char* handle, int offset, const char* origin);	

	/// get position in file defined by hanlde received over ip, returns position in ASCII if done, FT_FAIL if failed
	char* ft_tell(const char* handle);

	/// check the existance of file with given name, returns FT_TRUE if exists, FT_FALSE if not 
	char* ft_exists(const char* name);
	
	static nFile* strHandle2nFile(const char*);
     
	nSession	session;	
    nIpcServer* server;             // my mother server
    int id;                                      
    SOCKET		srvrSocket;    
    struct		sockaddr_in clientAddr;    
	char		answer[N_MAXPATH*2];
	
	bool		quiet;
	bool		exit_on_disconnect;
	
public:
	bool		backline;
};

//------------------------------------------------------------------------------
#endif
