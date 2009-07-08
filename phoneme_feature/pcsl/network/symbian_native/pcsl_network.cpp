#include <e32std.h>
#include <e32svr.h>
#include <es_sock.h>
#include <in_sock.h>
#include <string.h>

#include <pcsl_network.h>
#include <pcsl_print.h>
#include <os_symbian.hpp>

#ifdef _DEBUGNETWORK_
#define DEBUGPRINT(_AA) pcsl_print(_AA);
#else
#define DEBUGPRINT(_AA)
#endif


/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_init(void) 
{
	int ret = PCSL_NET_SUCCESS;
	DEBUGPRINT("pcsl_network_init\n");
    return ret;
}

#include <stdio.h>

/**
 * See pcsl_network.h for definition.
 * 
 * Note that this function NEVER returns PCSL_NET_WOULDBLOCK. Therefore, the 
 * finish() function should never be called and does nothing.
 */
int pcsl_network_gethostbyname_start(char *hostname,unsigned char *pAddress, int maxLen,int *pLen,void **pHandle,void **pContext)
{
	int ret = PCSL_NET_IOERROR;
	DEBUGPRINT("pcsl_network_gethostbyname_start\n");
	TUint32 add=0;
	MHostResolver* resolver;
	HBufC8* buf = HBufC8::New(strlen(hostname));
	buf->Des().Copy((const unsigned char*)hostname,strlen(hostname));
	HBufC* buf16 = HBufC::New(buf->Des().Length());
	buf16->Des().Copy(buf->Des());
	resolver = static_cast<MApplication*>(Dll::Tls())->HostResolver();
	if(resolver)
	{
		ret = resolver->ResolveHost(*buf16,add);
		if(ret == PCSL_NET_SUCCESS)
		{
			*(TUint32*)pAddress = add;
			*pLen  = 4;
		}
		else if(ret == PCSL_NET_WOULDBLOCK)
		{
			*pHandle = resolver;
		}
	}
	delete buf16;
	delete buf;
    return ret;
}


/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_gethostbyname_finish(
	unsigned char *pAddress,
	int maxLen,
	int *pLen,
    void *handle,
	void *context)
{
	int ret = PCSL_NET_IOERROR;
	DEBUGPRINT("pcsl_network_gethostbyname_finish\n");
    TUint32 add=0;
	MHostResolver* resolver = static_cast<MHostResolver*>(handle);
	ret = resolver->ResolveHost(KNullDesC,add);
	if(ret == PCSL_NET_SUCCESS)
	{
		*(TUint32*)pAddress = add;
		*pLen = 4;
	}
	if(ret != PCSL_NET_WOULDBLOCK)
	{
		resolver->Close();
	}
	return ret;
}


/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_error(
	void *handle)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_error !!!!!\n");
    return 0;
}

/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_getLocalHostName(
    char *pLocalHost)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_getLocalHostName !!!!!\n");
    return PCSL_NET_IOERROR;
}

/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_getLocalIPAddressAsString(
	char *pLocalIPAddress)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_getLocalIPAddressAsString !!!!!\n");
    return PCSL_NET_IOERROR;
}

/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_getHostByAddr_start(
    int ipn,
    char *hostname,
    void **pHandle,
    void **pContext)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_getHostByAddr_start !!!!!\n");
    return PCSL_NET_IOERROR;
}

/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_getHostByAddr_finish(
    int ipn,
    char *hostname,
    void **pHandle,
    void *context)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_getHostByAddr_finish !!!!!\n");
    return PCSL_NET_IOERROR;
}

/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_getlocalport(
	void *handle,
	int *pPortNumber)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_getlocalport !!!!!\n");
    return PCSL_NET_IOERROR;
}
        

/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_getremoteport(
	void *handle,
	int *pPortNumber)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_getremoteport !!!!!\n");
    return PCSL_NET_IOERROR;
}

/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_getsockopt(
	void *handle,
	int flag,
	int *pOptval)
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_getsockopt !!!!!\n");
    return PCSL_NET_IOERROR;
}


/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_setsockopt(
	void *handle,
	int flag,
	int optval)
{
	int ret = PCSL_NET_INVALID;
	DEBUGPRINT("pcsl_network_setsockopt\n");
	if(handle)
	{
		TUint symbOpt=0;
		switch(flag)
		{
		case 0:
			symbOpt = KSoTcpNoDelay;
			// not Symbian option is negative of the Java version!!!
			optval = optval == 0 ? 1 : 0;
			break;
		}
		TInt status = static_cast<MSocket*>(handle)->SetOption(symbOpt,optval);
		if(status == KErrNone)
		{
			ret = PCSL_NET_SUCCESS;
		}
	}
    return ret;
}

/**
 * See pcsl_network.h for definition.
 */
int pcsl_network_addrToString(unsigned char *ipBytes,
        unsigned short** pResult, int* pResultLen) 
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_addrToString!!!!!\n");
    return PCSL_NET_IOERROR;
}

/**
 * See pcsl_network.h for definition.
 */
extern "C" {
void pcsl_add_network_notifier(
    void *handle,
    int event) 
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_add_network_notifier !!!!!\n");
}
/**
 * See pcsl_network.h for definition.
 */
void pcsl_remove_network_notifier(
    void *handle,
    int event) 
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_remove_network_notifier !!!!!\n");
}
}

/**
 * See pcsl_network.h for definition.
 */
unsigned int pcsl_network_htonl(
    unsigned int value) 
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_htonl !!!!!\n");
    return value;
}

/**
 * See pcsl_network.h for definition.
 */
unsigned int pcsl_network_ntohl(
    unsigned int value) 
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_ntohl !!!!!\n");
    return value;
}
    
/**
 * See pcsl_network.h for definition.
 */
unsigned short pcsl_network_htons(
    unsigned short value) 
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_htons !!!!!\n");
    return value;
}

/**
 * See pcsl_network.h for definition.
 */
unsigned short pcsl_network_ntohs(
    unsigned short value) 
{
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_network_ntohs !!!!!\n");
    return value;
}

/**
 * See pcsl_network.h for definition.
 */
char * pcsl_inet_ntoa (void *ipBytes) 

{
    static char buffer[] = {'\0'};
	DEBUGPRINT("!!!!! NOT IMPLEMENTED pcsl_inet_ntoa !!!!!\n");
    return buffer;
}
