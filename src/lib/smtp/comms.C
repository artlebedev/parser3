/** @file
	Parser: SMTP sender impl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "smtp.h"

volatile const char * IDENT_COMMS_C="$Id: comms.C,v 1.13 2020/12/15 17:10:33 moko Exp $";

// ---------------------------------------------------------------------------
int SMTP:: 
IsAddressARawIpaddress(const char*  string)
{
    while( *string ) 
    {
        if( !isdigit((unsigned char)*string) ) 
        {
            return 0;
        }
        string++;
    }
    return 1;
}

//---------------------------------------------------------------------------
///@bug getservbyname is not reenterant
int SMTP:: 
ResolveService(const char* service, u_short *our_port)
{
    struct servent  *serventry = NULL;

    if( IsAddressARawIpaddress(service) ) 
    {
        char * tail;

        *our_port = (u_short)strtol(service, &tail, 10);
        
        if( tail == service ) 
        {
            return WSAEPROTONOSUPPORT;
        } 
        else 
        {
            *our_port = htons(*our_port);
        }
    } 
    else 
    {
        serventry = getservbyname(service, "tcp");
    
        if( serventry )
            *our_port = serventry->s_port;
        else 
        {
#ifdef _MSC_VER
            int retval = WSAGetLastError();
            if( (retval == WSANO_DATA) || (retval == WSANO_RECOVERY) ) 
            {
                return WSAEPROTONOSUPPORT;
            } 
            else 
            {
	            return (retval - 5000);
            }
#else 
		return WSAEPROTONOSUPPORT;
#endif
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
/// @bug gethostbyname is not reenterant
int SMTP:: 
ResolveHostname(const char* hostname, struct sockaddr_in *sa_in)
{
    struct hostent *hostentry = NULL;
    unsigned long 	    ip_address;

    if( (ip_address = inet_addr(hostname)) != INADDR_NONE ) 
    {
        sa_in->sin_addr.s_addr = ip_address;
    }
    else 
    {
        if( (hostentry = gethostbyname(hostname)) == NULL ) 
        {
            return WSAHOST_NOT_FOUND;
        }

        sa_in->sin_addr.s_addr = *(long *)hostentry->h_addr;
    }

    return 0;
}

//---------------------------------------------------------------------------
int SMTP:: 
GetAndSetTheSocket(SOCKET *the_socket)
{
    if( INVALID_SOCKET == (*the_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP/*был 0, вложенно не работал*/)) )
    {
		return WSAESOCKTNOSUPPORT;
    }

	// To enable SO_DONTLINGER (that is, disable SO_LINGER) 
	// l_onoff should be set to zero and setsockopt should be called
	linger dont_linger={0,0};
    setsockopt(*the_socket, SOL_SOCKET, SO_LINGER, (const char *)&dont_linger, sizeof(dont_linger));

    return 0;
}

//---------------------------------------------------------------------------
int SMTP:: 
GetConnection(SOCKET the_socket, struct sockaddr_in *sa_in)
{

    if( connect(the_socket,
			                    (struct sockaddr *)sa_in,
			                    sizeof(struct sockaddr_in))<0
      ) 
    {
	int     retval = 0;
        switch( (retval = WSAGetLastError()) )
        {
            case WSAEWOULDBLOCK:
                break;

            case WSAECONNREFUSED:
                return WSAECONNREFUSED;

            default:
                //wsprintf(message, "unexpected error %d from winsock\n", retval);
                //ShowError(message);
                return WSAHOST_NOT_FOUND;
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
void SMTP:: 
MiscSocketSetup(SOCKET soc, fd_set *fds, struct timeval *timeout)
{
#ifdef _MSC_VER
    unsigned long   ioctl_blocking = 1;
    ioctlsocket(soc, FIONBIO, &ioctl_blocking);
#endif

    FD_ZERO(fds);
    FD_SET(soc, fds);

    timeout->tv_sec = 30;
    timeout->tv_usec = 0;
}

//---------------------------------------------------------------------------
