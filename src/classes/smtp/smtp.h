/** @file
	Parser: SMTP sender decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: smtp.h,v 1.3 2001/04/26 14:55:20 paf Exp $
*/

#include <winsock.h>

#include "pa_pool.h"
#include "pa_string.h"

//////////////////////////////////////////////////////////////////////////////

#define SOCKET_BUFFER_SIZE	512

#define ERR_SENDING_DATA		4002
#define ERR_NOT_A_SOCKET		4010
#define ERR_CLOSING			4012
#define WAIT_A_BIT			4013

#define MAXOUTLINE 255

//////////////////////////////////////////////////////////////////////////////

/// SIMPLE MAIL TRANSPORT PROTOCOL Win32 realization
class SMTP : public Pooled {
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

	const String& origin_string;

public:
	SMTP(Pool& pool, const String& aorigin_string);

    // smtp.C
	void	Send(const char *, const char *, const char *,  char *, char *);
	BOOL	MakeSmtpHeader(char *, char *, char *, char *);
	void	prepare_message(char *, char *,  const char *, const char *);
	void	open_socket(const char *, const char *);
	int		get_line(void);
	void	SendSmtpError(const char *);
	void	transform_and_send_edit_data(const char *);
	void	send_data(const char *);
	void	ConnectToHost(const char *, const char *);
	int		GetBuffer(int);
	int		GetChar(int, char *);
	void	SendLine(const char *, unsigned long);
	void	SendBuffer(const char *, unsigned long);
	void	FlushBuffer();
	BOOL	CloseConnect();

    // comms.C
    int      IsAddressARawIpaddress(const char *);
    int      ResolveService(const char *, int *);
    int      ResolveHostname(const char *, struct sockaddr_in *);
    int      GetAndSetTheSocket(SOCKET *);
    int      GetConnection(SOCKET, struct sockaddr_in *);
    void     MiscSocketSetup(SOCKET, fd_set *, struct timeval *);
};

