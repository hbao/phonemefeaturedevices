#include <e32svr.h>
#include <ES_SOCK.H>
#include <in_sock.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <libc/string.h>
#include <pcsl_network.h>
#include <pcsl_print.h>
#include <OS_Symbian.hpp>

//#define _DEBUG_PCSL_SOCKETS_

#ifdef _DEBUG_PCSL_SOCKETS_
#include <stdio.h>
class RDebugPrinter
{
public:
	static void DebugPrint(const char* aMsg) {pcsl_print(aMsg);pcsl_print("\n");}
	static void DebugPrint(const char* aMsg,int aVal) 
		{
			char buffer[255];
			sprintf(buffer,"%S %d\n",aMsg,aVal);
			pcsl_print(buffer);
		}
	
};
#define DEBUGPRINT(_XX) {RDebugPrinter::DebugPrint(_XX);}
#define DEBUGPRINT1(_XX,_YY) {RDebugPrinter::DebugPrint(_XX,_YY);}
#else
#define DEBUGPRINT(_XX)
#define DEBUGPRINT1(_XX,_YY)
#endif

extern RSocketServ gServ;
/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_open_start(unsigned char *ipBytes,int port,void **pHandle,void **pContext)
{
	int ret = PCSL_NET_CONNECTION_NOTFOUND;
	DEBUGPRINT("pcsl_socket_open_start");
	
	MSocket* socket = static_cast<MApplication*>(Dll::Tls())->OpenSocket();
	if(socket)
	{
		*pHandle = socket;
		TUint32 addr = *(TUint32*)ipBytes;
		TInetAddr add(addr,port);
		ret = socket->Connect(add);
		if(ret == PCSL_NET_IOERROR)
		{
			socket->Close();
		}
	}
    return ret;
}


/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_open_finish(
	void *handle,
	void *context)
{
	int ret = PCSL_NET_IOERROR;
	int status;
	DEBUGPRINT("pcsl_socket_open_finish");
    MSocket* socket = static_cast<MSocket*>(handle);
	if(socket)
	{
		TUint32 addr = 0;
		TInetAddr add(addr,0);
		status = socket->Connect(add);
		if( status == 0)
		{
			ret = PCSL_NET_SUCCESS;
		}
		else if(status == 1)
		{
			ret = PCSL_NET_WOULDBLOCK;
		}
		else
		{
			socket->Close();
		}
	}
	return ret;
}
      

/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_read_start(
	void *handle,
	unsigned char *pData,
	int len,  
	int *pBytesRead,
	void **pContext)
{
	int ret = PCSL_NET_IOERROR;
	DEBUGPRINT("pcsl_socket_read_start");
	if(handle)
	{	
		int read;
		TPtr8 data(pData, len);
		ret = static_cast<MSocket*>(handle)->Read(data,pBytesRead);
	}	
	return ret;
}


/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_read_finish(
	void *handle,
	unsigned char *pData,
	int len,
	int *pBytesRead,
	void *context)
{
	DEBUGPRINT("pcsl_socket_read_finish");
    return pcsl_socket_read_start(handle,pData,len,pBytesRead,&context);
}


/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_write_start(
	void *handle,
	char *pData,
	int len,
	int *pBytesWritten,
	void **pContext)
{
	int ret = PCSL_NET_IOERROR;
	DEBUGPRINT("pcsl_socket_write_start");
	if(handle)
	{
		TPtr8 data((unsigned char*)pData,len,len);
		ret = static_cast<MSocket*>(handle)->Send(data,pBytesWritten);
	}
	return ret;
}


/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_write_finish(
	void *handle,
	char *pData,
	int len,
	int *pBytesWritten,
	void *context)
{
	DEBUGPRINT("pcsl_socket_write_finish");
    return pcsl_socket_write_start(handle,pData,len,pBytesWritten,&context);
}

/**
 * See pcsl_socket.h for definition.
 *
 * Note that this function NEVER returns PCSL_NET_WOULDBLOCK. Therefore, the 
 * finish() function should never be called and does nothing.
 */
int pcsl_socket_close_start(
    void *handle,
    void **pContext)
{
	DEBUGPRINT("pcsl_socket_close_start\n");
	if(handle)
	{
		MSocket* socket = static_cast<MSocket*>(handle);
		socket->Close();
	}
	return PCSL_NET_SUCCESS;
}

/**
 * See pcsl_socket.h for definition.
 *
 * Since the start function never returns PCSL_NET_WOULDBLOCK, this
 * function should never be called.
 */
int pcsl_socket_close_finish(
	void *handle,
	void *context)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_socket_close_finish !!!!!\n");
    return PCSL_NET_INVALID;
}

/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_available(void *handle, int *pBytesAvailable)
{
	int ret = PCSL_NET_IOERROR;
	int available;
	if(handle)
	{	
		available = static_cast<MSocket*>(handle)->Available();
		if(available >= 0)
		{
			*pBytesAvailable = available;
			ret = PCSL_NET_SUCCESS;
		}
	}
	DEBUGPRINT1("pcsl_socket_available",ret);
    return ret;
}

/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_shutdown_output(void *handle) 
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_socket_shutdown_output !!!!!\n");
    return PCSL_NET_SUCCESS;
}

/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_getlocaladdr(
	void *handle,
	char *pAddress)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_socket_getlocaladdr !!!!!\n");
    return PCSL_NET_IOERROR;
}


/**
 * See pcsl_socket.h for definition.
 */
int pcsl_socket_getremoteaddr(
	void *handle,
	char *pAddress)
{
	int address;
	in_addr ipAddress;
	DEBUGPRINT("pcsl_socket_getremoteaddr\n");

	int ret = PCSL_NET_IOERROR;
	if(handle)
	{	
		ipAddress.s_addr = static_cast<MSocket*>(handle)->GetIPv4Address();
		if(ipAddress.s_addr != 0)
		{
			strcpy(pAddress, inet_ntoa(ipAddress));
			ret = PCSL_NET_SUCCESS;
		}
	}
    return ret;	
}

