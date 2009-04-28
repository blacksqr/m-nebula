#define N_IMPLEMENTS nIpcClient
#define N_KERNEL
//------------------------------------------------------------------------------
//  nipcclient.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nipcclient.h"
#include "kernel/nsession.h"

//------------------------------------------------------------------------------
/**
*/
nIpcClient::nIpcClient() :
    serverPortNum(0)    
{
#   ifdef __WIN32__
       struct WSAData wsa_data;
       WSAStartup(0x101, &wsa_data);
#   endif

    n_memset(&(this->serverAddr), 0, sizeof(this->serverAddr));
	this->session = new nSession;
}

//------------------------------------------------------------------------------
/**
*/
nIpcClient::~nIpcClient()
{
    delete this->session;

#   ifdef __WIN32__
    WSACleanup();
#   endif
}

//------------------------------------------------------------------------------
/**
    Der uebergebene Name hat folgenden Aufbau:
  
    [hostname:]portname 

    Hostname ist optional und nur notwendig, wenn
    sich die Adresse auf einem anderen Rechner befindet.
    Statt eines Namens kann auch eine TCP/IP Adresse
    angegeben sein.
    "portname" bezeichnet den Namen des Ports, der auf
    "hostname" angesprochen werden soll.
  
    Die Routine fuellt die [server_addr] Struktur in
    this aus.
*/
bool 
nIpcClient::FillServerAddr(const char* name)
{
    n_assert(name);

    char nameBuf[128];
    char *hostname = NULL;
    char *portname = NULL;
    char *tmp;
    struct hostent *he;
    
    // split host and port name
    n_strncpy2(nameBuf, name, sizeof(nameBuf));
    if ((tmp = strchr(nameBuf, ':'))) 
    {
        *tmp = 0;
        hostname = nameBuf;
        portname = tmp + 1;
    } else {
        hostname = "localhost";
        portname = nameBuf;
    }

    this->serverHostName = hostname;
    this->serverPortName = portname;
    n_assert(hostname[0] != 0);
    n_assert(portname[0] != 0);
    
    // Hostname und initiale Portnummer eintragen
    he = gethostbyname(hostname);
    if (!he) {
        n_printf("nIpcClient(): unknown host!");
        return false;
    }
    this->serverPortNum = this->GetPortNumFromName(portname);
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port   = htons(this->serverPortNum);
#if defined(__WIN32__)
    this->serverAddr.sin_addr.S_un.S_addr = inet_addr(hostname);
#elif defined(__LINUX__)
    this->serverAddr.sin_addr.s_addr = inet_addr(hostname);
#endif
    this->serverAddr.sin_addr   = *((struct in_addr *)he->h_addr);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
short 
nIpcClient::GetPortNumFromName(const char *portName)
{
    n_assert(portName);
    short pnum = ((short)hash(portName, N_SOCKET_PORTRANGE)) + N_SOCKET_MIN_PORTNUM;
    return pnum;
} 

//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
    31-Oct-98   floh    falls ein Port nicht zuhoerte, wurde kein
                        neuer Socket aufgemacht, was dazu fuehrte,
                        dass sich die Routine in einer Endlosschleife
                       aufhing, weil keine Connection mehr geklappt
                        hatte.
*/
bool 
nIpcClient::Connect(const char* portName)
{
    n_assert(this->session->transfer_socket == INVALID_SOCKET);
    bool retval = false;
   
    // resolve portname
    if (this->FillServerAddr(portName)) 
    {
        int res;
        char msg_buf[128];
        bool connected = false;
        bool error     = false;
        int num_retries = 0;
        
        while ((!connected) && (!error) && (num_retries < 4)) 
        {    
            // falls Retry, alten Socket killen
            if (this->session->connected()) 
            {
                this->session->close("Connection retry\n");
                num_retries++;
            }
            
            // einen neuen Socket initialisieren                
            SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
            if (s == INVALID_SOCKET) 
            {
                n_printf("nIpcClient::nIpcClient(): could not create socket!");
                return false;       
            }

            n_printf("nIpcClient: trying %s:%s, port %d...\n",
                      this->serverHostName.c_str(), this->serverPortName.c_str(),
                      this->serverPortNum);

            res = connect(s, (struct sockaddr *) &(this->serverAddr),
                          sizeof(struct sockaddr));
            if (res != -1) 
            {
				this->session->set(s);

                // eine Verbindung steht, richtiger Port?
                sprintf(msg_buf, "~handshake %s", this->serverPortName.c_str());
				error = !this->session->send_string(msg_buf);                
            
                // warte auf Antwort...
                if (!error) 
                {
                    n_printf("nIpcClient: sending handshake... ");
                    res = this->session->receive_string();
                    if ((res == -1)||(res == 0)) 
                    {
                        n_printf("nIpcClient::Connect(): ~handshake recv() failed!\n");
                        error = true;
                    } 
                    else 
                    {                    
                        // positive Antwort?
                        if (strcmp(this->session->get_buffer(), "~true") == 0) 
                        {
                            connected = true;
                            n_printf("accepted.\n");
                        } 
                        else 
                        {
                            // falscher Portname, neue probieren...
                            this->serverPortNum++;
                            this->serverAddr.sin_port = htons(this->serverPortNum);
                            n_printf("wrong portname.\n");
                        }
                    }
                } 
                else 
                {
                    n_printf("nIpcClient::Connect(): ~handshake send() failed!");
                    error = true;
                }
            } 
            else 
            {
                // auf diesem Port hoert niemand zu, naechste Portnummer...
                this->serverPortNum++;
                this->serverAddr.sin_port = htons(this->serverPortNum);
            }
        }
        if (connected) 
        {
            n_printf("nIpcClient: connected.\n");
            retval = true;
        } 
        else 
        {
            n_printf("nIpcClient: Could not connect to a valid server!\n");
            this->session->close();
        }
    }
    return retval;
}                                                                  
        
//------------------------------------------------------------------------------
/**
*/
void 
nIpcClient::Disconnect()
{
    n_assert(this->session->transfer_socket != INVALID_SOCKET);    
    
    // sende ~close Befehl
    bool error = !this->session->send_string("~close");
    if (!error) 
    {
        // Antwort abholen        
        int res = this->session->receive_string();
        if ((res == -1) || (res == 0)) 
        {
            n_printf("nIpcClient::Disconnect(): recv(~close) failed!\n");    
        }
    } 
    else 
    {
        n_printf("nIpcClient::Disconnect(): send(~close) failed!\n");
    } 

    // kill the socket
    this->session->close();
}

//------------------------------------------------------------------------------
/**
*/
int nIpcClient::ReceiveEvent()
{
	if (this->session->check_arrival())
	{
		long buf_size = this->session->size();
		char* buf = this->session->get_buffer();
		n_memset(buf, 0, buf_size);
		
		static const long tag_size = sizeof(long)*2;
		int total_received = this->session->receive_data();		

		while (total_received < tag_size)
		{
			int received = this->session->receive_data(buf_size - total_received, buf, buf_size - total_received);
			if (received > 0)
				total_received += received;
		}
		//n_printf("msg checked\n");
		long* lptr = (long*)this->session->get_buffer();		
		long msg_tag = ntohl(*(lptr++));
		long size = ntohl(*(lptr++));

		while (total_received < size + tag_size)
		{
			int received = this->session->receive_data(buf_size - total_received, buf, buf_size - total_received);
			if (received > 0)
				total_received += received;
		}

		long msg_sn = ntohl(*(lptr++));
		long post_time = ntohl(*(lptr++));
		long msgt = ntohl(*(lptr++));
		long param1 = ntohl(*(lptr++));
		long param2 = ntohl(*(lptr++));

		long delivery = (long)(this->session->get_milli() - post_time);
		n_printf("msg# %d delivered in %d milli. msg_tag=%d; msg_type=%d; param1=%d; param2=%d\n", 
				msg_sn, delivery, msg_tag, msgt, param1, param2);		
		
		return total_received;
	}
	else
		return 0;	
}


//------------------------------------------------------------------------------
/**
*/
nMsgNode*
nIpcClient::SendMsg(void* buf, int size)
{
    n_assert(buf);
    n_assert(size > 0);

    nMsgNode *nd = NULL;
    
	bool error = !this->session->send_data((const char*)buf, size);
    
    if (error) 
    {
        n_printf("nIpcClient::SendMsg(): send() failed!");
    } 
    else 
    {
        // wait for an answer...
        int res = this->session->receive_string();
        if ((res == -1) || (res == 0)) 
        {
            n_printf("nIpcClient::SendMsg(): recv() failed!\n");
        } 
        else if (res > 0) 
        {
            // baue eine nMsgNode mit dem Ergebnis
            nd = new nMsgNode(this->session->get_buffer(), res);
        }
    }
    return nd;
}

//------------------------------------------------------------------------------
/**
*/
void 
nIpcClient::FreeReplyMsgNode(nMsgNode *nd)
{
    delete nd;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nIpcClient::GetHostName() const
{
    return this->serverHostName.empty() ? 0 : this->serverHostName.c_str();
}

//------------------------------------------------------------------------------
/**
*/
const char*
nIpcClient::GetPortName() const
{
    return this->serverPortName.empty() ? 0 : this->serverPortName.c_str();
}

