/** @file
	Parser: SMTP sender impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: comms.C,v 1.3 2001/09/21 14:46:09 parser Exp $"; 

#include <winsock.h>

#include "smtp.h"

// ---------------------------------------------------------------------------
int SMTP:: 
IsAddressARawIpaddress(const char * string)
{
    while( *string ) 
    {
        if( !isdigit(*string) ) 
        {
            return 0;
        }
        string++;
    }
    return 1;
}

//---------------------------------------------------------------------------
int SMTP:: 
ResolveService(const char* service, int *our_port)
{
    struct servent FAR  *serventry = NULL;
    int                 retval = 0;

    if( IsAddressARawIpaddress(service) ) 
    {
        char * tail;

        *our_port = (int)strtol(service, &tail, 10);
        
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
        serventry = getservbyname(service, (LPSTR)"tcp");
    
        if( serventry )
            *our_port = serventry->s_port;
        else 
        {
            retval = WSAGetLastError();
            if( (retval == WSANO_DATA) || (retval == WSANO_RECOVERY) ) 
            {
                return WSAEPROTONOSUPPORT;
            } 
            else 
            {
	            return (retval - 5000);
            }
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
int SMTP:: 
ResolveHostname(const char* hostname, struct sockaddr_in *sa_in)
{
    struct hostent FAR *hostentry = NULL;
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

        sa_in->sin_addr.s_addr = *(long far *)hostentry->h_addr;
    }

    return 0;
}

//---------------------------------------------------------------------------
int SMTP:: 
GetAndSetTheSocket(SOCKET *the_socket)
{
    int not = 0;

    if( INVALID_SOCKET == (*the_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP/*был 0, вложенно не работал*/)) )
    {
		//return h_errno;
		return WSAESOCKTNOSUPPORT;
    }

    setsockopt(*the_socket, SOL_SOCKET, SO_DONTLINGER, (char *)&not, sizeof(not));

    return 0;
}

//---------------------------------------------------------------------------
int SMTP:: 
GetConnection(SOCKET the_socket, struct sockaddr_in *sa_in)
{
    int     retval = 0;
    //char    message[512];

    if( SOCKET_ERROR == connect(the_socket,
			                    (struct sockaddr *)sa_in,
			                    sizeof(struct sockaddr_in) )
      ) 
    {
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
    unsigned long   ioctl_blocking = 1;

    ioctlsocket(soc, FIONBIO, &ioctl_blocking);

    FD_ZERO(fds);
    FD_SET(soc, fds);

    timeout->tv_sec = 30;
    timeout->tv_usec = 0;
}

//---------------------------------------------------------------------------
