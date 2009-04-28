#ifndef N_SESSION
#define N_SESSION
//------------------------------------------------------------------------------
//  nsession.h
//  (C) 2007 Insightec, Inc -by Ilya Kliot
//------------------------------------------------------------------------------

#include "mathlib/vector.h"
#ifdef _NEBULA_DEVICE_
#include "util/nstl.h"
#else
#include "stl_types.h"
#define	n_printf TRACE
#endif

#if __XBxX__
#   include "xbox/nxbwrapper.h"
#elif defined (WIN32) || defined(__WIN32__)
#   ifndef _INC_WINDOWS
#		include <windows.h>
#   endif
#	ifndef _WINSOCKAPI_
#		include <winsock2.h>
#   endif
#	ifdef _WINSOCK2API_
#		include <ws2tcpip.h>
#	endif
#else
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   include <netdb.h>
#endif

#include <sys/types.h>
#include <sys/timeb.h>

#ifndef N_DEBUG_H
#include <assert.h>
#define n_assert	assert
#endif

//#define	SESSION_DEBUG

#define  N_TRANSPORT_BUFSIZE  16384
#define  N_MESSAGE_BUFSIZE  500
#define	 N_MAX_ADDR 512

//------------------------------------------------------------------------------
/**
	class nWinsokWrapper	
	wraps basic winsock operations like send and receive for data and strings

  created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
class nSession : public char_buffer {
public:
	nSession() : transfer_socket(INVALID_SOCKET),
		error(0), last_converasion(0), blocking(true) 
	{ 
		this->resize(N_TRANSPORT_BUFSIZE);
		memset(&this->peer_addr, 0, sizeof(sockaddr_in));
		memset(&this->local_addr, 0, sizeof(sockaddr_in));
	}

	virtual ~nSession() { this->close("~nSession():"); }

	bool fill_peer_addr(const char* host, int port);

	/// assign socket
	bool set(SOCKET s);

	/// check if something arrived
	int check_arrival();

	/// returns size of data received, 0 if no data in transfer_socket, -1 if transfer_socket closed
	int receive_data(int size = 0, char* buffer = 0, int buf_size = 0);

	/// returns length of string received
	int receive_string(bool block = false);	

	/// wrap regular send for data
	int send_data(const char* buffer, int buf_size);

	/// send until whole size not sent
	bool complete_send(const char* buffer, int size);	

	/// wrap regular send for strings
	bool send_string(const char* str);

	/// check for transfer_socket errors
	int check_error(int skip = 0);

	/// return state of the socket
	bool connected() const { return this->transfer_socket != INVALID_SOCKET; }

	/// close transfer_socket
	void close(const char* msg = 0);

	/// set session int blocking (non-blocking) mode
	void set_block_mode(bool block);

	/// return true if session is in blocking mode
	bool get_block_mode() const { return this->blocking; }

	/// returns last conversation time
	__int64 get_last_conversation_time() const { return this->last_converasion; }

	/// returns current time in milliseconds,
	__int64 get_milli() const
	{
		_timeb tm;
		_ftime(&tm);	
		return tm.time*1000 + tm.millitm;
	}	

	/// TCP socket
	SOCKET	transfer_socket;	

	const char* get_peer_name() const { return this->peer_name.c_str();	}
	int get_peer_port() const { return ntohs(peer_addr.sin_port); }
	
	sockaddr_in	peer_addr;
	sockaddr_in	local_addr;
	
protected:

	bool blocking;

	stl_string peer_name;	

	/// close transfer_socket
	void close_socket(SOCKET& s, const char* msg = 0);		

	__int64 last_converasion;

	int error;	
};

//------------------------------------------------------------------------------
/**
	check if something arrived
	returns:
	size of delivery,
	0 if nothing came
	SOCKET_ERROR if failure
	created 25-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
int nSession::check_arrival() 
{	
	if (this->transfer_socket != INVALID_SOCKET)
	{
		u_long arrived_size = 0;
		int n_errno = 0;

#if defined(WIN32) || defined(__WIN32__)
		int res = ioctlsocket(this->transfer_socket, FIONREAD , &arrived_size);		
#elif defined(__LINUX__) || defined(__MACOSX__)
#error IMPLEMNT ME
#endif
		if (res)
		{
			n_errno = this->check_error();
		}

		if (!n_errno && arrived_size)
		{
			return (int)arrived_size;
		}

		if (!this->get_block_mode())
		{
			char check;
			res = recv(this->transfer_socket, &check, 1, MSG_PEEK);		
#if defined(WIN32) || defined(__WIN32__)
			n_errno = this->check_error(WSAEWOULDBLOCK);			
#elif defined(__LINUX__) || defined(__MACOSX__)
			n_errno = this->check_error(EWOULDBLOCK);						
#else
#error IMPLEMENT ME
#endif	
			if (0 == res) 
			{
				this->close("nSession::check_arrival connection failed() recv == 0 error != 0:");
				return SOCKET_ERROR;
			}
		}
				
		// no data pending, return immediately
		if (n_errno) 
		{
			this->close("nSession::check_arrival connection failed() error != 0:");
			return SOCKET_ERROR;
		}
	}
	
	return 0;
}

//------------------------------------------------------------------------------
/**
	set up peer address
	created 15-Jul-2007 Insightec, Inc -by Ilya Kliot
*/
inline
bool nSession::fill_peer_addr(const char* host, int port)
{
	if (!host)
		host = "localhost";
	
	this->peer_name = host;
	struct hostent* he = gethostbyname(host);

	if (!he) 
	{
        n_printf("nSession::fill_addr unknown host!");
        return false;
    }    
	
    this->peer_addr.sin_family = AF_INET;
    this->peer_addr.sin_port   = htons((short)port);
    this->peer_addr.sin_addr.S_un.S_addr = inet_addr(host);
    this->peer_addr.sin_addr   = *((struct in_addr *)he->h_addr);
    return true;
}

//------------------------------------------------------------------------------
/**
	assign socket
	created 25-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
bool nSession::set(SOCKET s)
{
	n_assert("nSession::set transfer_socket assigned and not released" && this->transfer_socket == INVALID_SOCKET);
	if ( s != INVALID_SOCKET )
	{			
		this->transfer_socket = s;
		memset(&this->peer_addr, 0, sizeof(sockaddr_in));
		memset(&this->local_addr, 0, sizeof(sockaddr_in));
		int addr_len = sizeof(sockaddr_in);
		getsockname(s, (sockaddr *)&this->local_addr, &addr_len);

#ifdef _WINSOCK2API_
		if (!this->peer_name.size())
		{			
			addr_len = sizeof(sockaddr_in);
			getpeername(s, (sockaddr *)&this->peer_addr, &addr_len);
			char peerName[N_MAX_ADDR];
			char servInfo[N_MAX_ADDR];
			getnameinfo((sockaddr *)&this->peer_addr, sizeof(sockaddr), 
				peerName, N_MAX_ADDR, 
				servInfo, N_MAX_ADDR, 
				NI_NUMERICSERV);

			this->peer_name = peerName;
		}
#endif
		
		this->last_converasion = this->get_milli();
		return true;
	}
	else
		return false;		
}

//------------------------------------------------------------------------------
/**
	returns: 
		size of data received , 
		0 if no data in transfer_socket, 
		-1 if transfer_socket closed

	created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
int nSession::receive_data(int size, char* buffer, int buf_size)
{	
	if (this->transfer_socket != INVALID_SOCKET) 
	{
		if (!buffer) 
		{
			buffer = this->get_buffer();
			buf_size = this->size();
		}

		if (!size || size > buf_size) 
			size = buf_size;

		n_assert(size);

		// do a non-blocking recv
		int len = recv(this->transfer_socket, buffer, size, 0);

		if (len < 0) // probably no data received, but may be an error occurred
		{
			// an error, filter out the would block error

#if defined(WIN32) || defined(__WIN32__)
			int n_errno = this->check_error(WSAEWOULDBLOCK);			
#elif defined(__LINUX__) || defined(__MACOSX__)
			int n_errno = this->check_error(EWOULDBLOCK);						
#else
#error IMPLEMENT ME
#endif
			// no data pending, return immediately
			if (!n_errno) 
				return 0;	
			else
			{
				this->close("nSession::receive_data() len < 0 error != 0:");
				return SOCKET_ERROR;
			}
		} 
		else if (len > 0) // data received
		{
			this->last_converasion = this->get_milli();
			return len; 			
		}
		else // 0
		{
			this->check_error();			
			this->close("nSession::receive_data() len = 0:");
			return SOCKET_ERROR;	
		}
	} 	
	return SOCKET_ERROR;
}

//------------------------------------------------------------------------------
/**
	returns length of string received
	
	created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
int nSession::receive_string(bool block)
{		
	int buf_sz = this->size();
	char* buf = this->get_buffer();
	memset(buf, 0xff, buf_sz);
	buf[buf_sz-1] = 0;
	int len = this->receive_data();
	if (len > 0 || (block && !this->blocking))
	{	
		while (this->get_buffer()[len-1] && !this->error)
		{
			if (len >= buf_sz)
			{
				this->resize(buf_sz + N_TRANSPORT_BUFSIZE);
				n_printf("nSession::receive_string():warning: receive buffer overloaded, resizing to %d\n", this->size());
				buf_sz = this->size();
				buf = this->get_buffer();					
			}

			int rcv_len = this->receive_data(buf_sz - len, buf + len, buf_sz - len);

			if (rcv_len > 0)
				len += rcv_len;			
			else if (rcv_len < 0)
				return SOCKET_ERROR;			
		}
	} 			

#ifdef SESSION_DEBUG
	if (len > 0)
		if (len < 250)
			n_printf("nSession::receive_string()->received:%dbytes, %s\n", len, this->get_buffer());
		else
			n_printf("nSession::receive_string()->received:%dbytes, very long string\n", len);

#endif
	
	return len;	
}

//------------------------------------------------------------------------------
/**
	just wrap send 
	return number of bytes was sent

	created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
int nSession::send_data(const char* buffer, int buf_size)
{
	if (this->transfer_socket != INVALID_SOCKET)
	{
		int res = send(this->transfer_socket, buffer, buf_size, 0);
		this->last_converasion = this->get_milli();
		if (res == SOCKET_ERROR)
		{
			this->check_error();
			this->close("nSession::send_data() res==SOCKET_ERROR:");
		}
		return res;
	}
	return SOCKET_ERROR;	
}

//------------------------------------------------------------------------------
/**
	sends data until whole size is not transferred 
	return false if transfer_socket error occurred on the way

	created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
bool nSession::complete_send(const char* buffer, int size)
{		
	int total_sent = 0;
	while (total_sent < size) 
	{
		int sent = this->send_data(buffer + total_sent, size - total_sent);
		if (sent > 0) 
		{
			total_sent += sent;
		} 
		else if (sent == SOCKET_ERROR)
		{			
			return false;
		} 
	}
	return true;
}

//------------------------------------------------------------------------------
/**
	sends string, wraps complete_send
	return false if transfer_socket error occurred on the way

	created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
bool nSession::send_string(const char* str)
{
	if (str)
	{
#ifdef SESSION_DEBUG
		if (strlen(str) < 250)
			n_printf("nSession::send_string()->%s\n", str);
		else
			n_printf("nSession::send_string()->very long string\n");

#endif
		return this->complete_send(str, strlen(str) + 1);
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------
/**
	Close the session

	created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
void nSession::close(const char* msg)
{	
	this->close_socket(this->transfer_socket, msg);
	this->error = 0;
}

//------------------------------------------------------------------------------
/**
	Close the given socket.

	created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
void nSession::close_socket(SOCKET& s, const char* msg)
{	
	if (s != INVALID_SOCKET) 
	{		
		if (msg)
			n_printf(msg);		
		n_printf(">>>>>>>>nSession::close\n");
		shutdown(s, 2);
#if defined(WIN32) || defined(__WIN32__)
		closesocket(s);
#elif defined(__LINUX__) || defined(__MACOSX__)
		close(s);
#endif
		s = INVALID_SOCKET;		
	}	
}

//------------------------------------------------------------------------------
/**
	return last error and prints error msg in case of error

	created 18-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
int nSession::check_error(int skip)
{	
#if defined(WIN32) || defined(__WIN32__)
	int n_errno = WSAGetLastError();	
#elif defined(__LINUX__) || defined(__MACOSX__)
	int n_errno = errno;	
#else
#error IMPLEMENT ME
#endif
	if (n_errno == skip)
		n_errno = 0;

	if (n_errno)
	{
		this->error = n_errno;
		char msg[256];
		sprintf(msg,"nSession::check_error transfer_socket error %d\n", n_errno);
		n_printf(msg);
	}

	return n_errno;
}

//------------------------------------------------------------------------------
/**
	set session int blocking (non-blocking) mode
	created 25-Jun-2007 Insightec, Inc -by Ilya Kliot
*/
inline
void nSession::set_block_mode(bool block) 
{
	if (this->transfer_socket != INVALID_SOCKET)
	{	
		// put the socket into non-blocking mode
#if defined(WIN32) || defined(__WIN32__)
		u_long mode = (block ? 0 : 1);
		int res = ioctlsocket(this->transfer_socket, FIONBIO, &mode);
		if (res)
		{
			this->check_error();
		}
#elif defined(__LINUX__) || defined(__MACOSX__)
		int flags;
		flags = fcntl(this->transfer_socket, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(this->transfer_socket, F_SETFL, flags);
#endif
		this->blocking = block;
	}
}

#endif