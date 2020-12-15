/** @file
	Parser: SMTP sender decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#define IDENT_SMTP_H "$Id: smtp.h,v 1.15 2020/12/15 17:10:33 moko Exp $"


#include "pa_string.h"

#ifdef _MSC_VER

#include <windows.h>

#else //_MSC_VER

typedef char CHAR;
typedef u_int SOCKET;
#define closesocket close
inline int WSAGetLastError() { return errno; }

#ifdef EPROTONOSUPPORT
#	define WSAEPROTONOSUPPORT EPROTONOSUPPORT
#else
#	define WSAEPROTONOSUPPORT (10000)
#endif

#ifdef ESOCKTNOSUPPORT
#	define WSAESOCKTNOSUPPORT ESOCKTNOSUPPORT
#else
#	define WSAESOCKTNOSUPPORT (10001)
#endif

#ifdef ENOTCONN
#	define WSAENOTCONN ENOTCONN
#else
#	define WSAENOTCONN (10002)
#endif

#ifdef ESHUTDOWN
#	define WSAENETDOWN ESHUTDOWN
#else
#	define WSAENETDOWN (10003)
#endif

#ifdef EHOSTUNREACH
#	define WSAENETUNREACH EHOSTUNREACH
#else
#	define WSAENETUNREACH (10004)
#endif

#ifdef ENETRESET
#	define WSAENETRESET ENETRESET
#else
#	define WSAENETRESET (10005)
#endif

#ifdef ECONNABORTED
#	define WSAECONNABORTED ECONNABORTED
#else
#	define WSAECONNABORTED (10006)
#endif

#ifdef ECONNRESET
#	define WSAECONNRESET ECONNRESET
#else
#	define WSAECONNRESET (10007)
#endif

#ifdef EWOULDBLOCK
#	define WSAEWOULDBLOCK EWOULDBLOCK
#else
#	define WSAEWOULDBLOCK (10008)
#endif

#ifdef ECONNREFUSED
#	define WSAECONNREFUSED ECONNREFUSED
#else
#	define WSAECONNREFUSED (10009)
#endif

#define WSAHOST_NOT_FOUND (10010)

#ifndef INADDR_NONE
#	define INADDR_NONE ((unsigned long) -1)
#endif

#ifndef INVALID_SOCKET
#	define INVALID_SOCKET (SOCKET)(~0)
#endif

#endif //_MSC_VER

//////////////////////////////////////////////////////////////////////////////

#define SOCKET_BUFFER_SIZE	512

#define ERR_SENDING_DATA		4002
#define ERR_NOT_A_SOCKET		4010
#define ERR_CLOSING			4012
#define WAIT_A_BIT			4013

/// must be >=SOCKET_BUFFER_SIZE, thanks to Lev Walkin <vlm@netli.com> for pointing that out
#define MAXOUTLINE (SOCKET_BUFFER_SIZE*2)

//////////////////////////////////////////////////////////////////////////////

/// SIMPLE MAIL TRANSPORT PROTOCOL Win32 realization
class SMTP: public PA_Object {
    char            *in_buffer;
    char            *out_buffer;
    unsigned int	in_index;
    unsigned int	out_index;
    unsigned int	in_buffer_total;
    unsigned int	out_buffer_total;
    unsigned int	last_winsock_error;
    fd_set	        fds;
    struct timeval	timeout;

    SOCKET	the_socket;

	char	my_hostname[1024];

    CHAR    ServerProtocol[100];
    CHAR    RemoteAddress[100];
    CHAR    RemoteHost[100];
    CHAR    RemoteUser[100];
    CHAR    HttpAccept[100];
    CHAR    HttpUserAgent[256];

    CHAR    FirstName[100];
    CHAR    LastName[100];
    CHAR    WebUse[100];
    CHAR    EMail[100];
    CHAR    HomePage[100];
    CHAR    text[500];

public:
	SMTP();
	override ~SMTP();

    // smtp.C
	void	Send(const char* , const char* , const char* ,  char *, char *);
	bool	MakeSmtpHeader(char *, char *, char *, char *);
	void	prepare_message(char *, char *,  const char* , const char* );
	void	open_socket(const char* , const char* );
	int		get_line(void);
	void	SendSmtpError(const char* );
	void	transform_and_send_edit_data(const char* );
	void	send_data(const char* );
	void	ConnectToHost(const char* , const char* );
	int		GetBuffer(int);
	int		GetChar(int, char *);
	void	SendLine(const char* , unsigned long);
	void	SendBuffer(const char* , unsigned long);
	void	FlushBuffer();
	bool	CloseConnect();

    // comms.C
    int      IsAddressARawIpaddress(const char* );
    int      ResolveService(const char* , u_short *);
    int      ResolveHostname(const char* , struct sockaddr_in *);
    int      GetAndSetTheSocket(SOCKET *);
    int      GetConnection(SOCKET, struct sockaddr_in *);
    void     MiscSocketSetup(SOCKET, fd_set *, struct timeval *);
};
