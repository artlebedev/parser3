/** @file
	Parser: SMTP sender.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: smtp.C,v 1.11 2002/07/31 14:31:02 paf Exp $

	Parts of the code here is based upon an early gensock and blat
*/

#include "pa_exception.h"
#include "smtp.h"

SMTP::SMTP(Pool& pool, const String& aorigin_string) : Pooled(pool),
	origin_string(aorigin_string) {
    the_socket  = 0;
    in_index    = 0;
    out_index   = 0;
    in_buffer   = 0;
    in_buffer_total  = 0;
    out_buffer_total = 0;

    in_buffer  = (char *)malloc(SOCKET_BUFFER_SIZE);
    out_buffer = (char *)malloc(SOCKET_BUFFER_SIZE);

    last_winsock_error = 0;
}


// ---------------------------------------------------------------------------
void SMTP:: 
ConnectToHost(const char *hostname, const char *service)
{
    struct sockaddr_in	sa_in;
    int			        our_port;

    if( !ResolveService(service, &our_port) )
    {
        if( !ResolveHostname(hostname, &sa_in) )
        {
			sa_in.sin_family = AF_INET;
            sa_in.sin_port   = our_port;

            if( !GetAndSetTheSocket(&the_socket) )
            {
				if( !GetConnection(the_socket, &sa_in) )
                {
                    MiscSocketSetup(the_socket, &fds, &timeout);

                    return;
                }
            }
        }
    }

    CloseConnect();

    throw Exception("smtp.connect",
		&origin_string,
		"connect to %s:%s failed", 
			hostname, service);
}

//---------------------------------------------------------------------------
// returns 0 if all is OK
int SMTP:: 
GetBuffer(int wait)
{
    int             retval;
    int             bytes_read = 0;
    unsigned long   ready_to_read = 0;

    // Use select to see if data is waiting...
    FD_ZERO(&fds);
    FD_SET(the_socket, &fds);

    // if wait is set, we are polling, return immediately
    if( wait ) 
    {
        timeout.tv_sec = 0;
    }
    else 
    {
        timeout.tv_sec = 30;
    }

    if( SOCKET_ERROR == (retval = select(0, &fds, NULL, NULL, &timeout)) )
    {
        //char    what_error[256];
        int     error_code = WSAGetLastError();

        if( error_code == WSAEINPROGRESS && wait ) 
        {
            return WAIT_A_BIT;
        }

		/*
        wsprintf(what_error,
	            "GetBuffer() unexpected error from select: %d",
	            error_code);

        ShowError(what_error);
		*/
    }

    // if we don't want to wait
    if( !retval && wait ) 
    {
        return WAIT_A_BIT;
    }

    // we have data waiting...
    bytes_read = recv(the_socket,
					 in_buffer,
					 SOCKET_BUFFER_SIZE,
					 0);

    // just in case.
    if( 0 == bytes_read )
    {
        // connection terminated (semi-) gracefully by the other side
        return WSAENOTCONN;
    }

    if( SOCKET_ERROR == bytes_read ) 
    {
        //char    what_error[256];
        int     ws_error = WSAGetLastError();

        switch( ws_error ) 
        {
            // all these indicate loss of connection (are there more?)
            case WSAENOTCONN:
            case WSAENETDOWN:
            case WSAENETUNREACH:
            case WSAENETRESET:
            case WSAECONNABORTED:
            case WSAECONNRESET:
                return WSAENOTCONN;

            case WSAEWOULDBLOCK:
                return WAIT_A_BIT;

            default:
                /*wsprintf(what_error,
		                "GetBuffer() unexpected error: %d",
		                ws_error);
                ShowError(what_error);
				*/
				break;
        }
    }

    // reset buffer indices.
    in_buffer_total = bytes_read;
    in_index = 0;

    return 0;
}

//---------------------------------------------------------------------------
// returns 0 if all is OK
int SMTP:: 
GetChar(int wait, char *ch)
{
    int         retval = 0;

    if( in_index >= in_buffer_total )
    {
        if( 0 != (retval = GetBuffer(wait)) )
            return retval;
    }
    *ch = in_buffer[in_index++];

    return 0;
}

//----------------------------------------------------------------------
int SMTP::
get_line( void )
{
	char    ch = '.';
	char    in_data[MAXOUTLINE];
	char    *index;

	index = in_data;

	while( ch != '\n' )
	{
		if( 0 != GetChar(0, &ch) )
		{
			return -1;
		}
		else
		{
			*index = ch;
			index++;
		}
	}

	if( in_data[3] == '-' ) 
		return get_line();
	else {
		char *error_pos;
		return strtol(in_data, &error_pos, 0);
	}
}

//---------------------------------------------------------------------------
// returns 0 if all is OK
void SMTP:: 
SendLine(const char *data, unsigned long length)
{
    int         num_sent;

    FD_ZERO(&fds);
    FD_SET(the_socket, &fds);

    timeout.tv_sec = 30;

    while( length > 0 ) 
    {
        if( SOCKET_ERROR == select(0, NULL, &fds, NULL, &timeout) ) 
            throw Exception("smtp.execute",
				&origin_string,
		        "connection::put_data() unexpected error from select: %d",
					WSAGetLastError());

        num_sent = send(the_socket,
		                data,
		                length > 1024 ? 1024 : (int)length,
		                0);

        if( SOCKET_ERROR == num_sent ) 
        {
            int  ws_error = WSAGetLastError();
            
            switch( ws_error ) 
            {
	            // this is the only error we really expect to see.
                case WSAENOTCONN:
	                return;

	            // seems that we can still get a block
                case WSAEWOULDBLOCK:
	                break;

                default:
					throw Exception("smtp.execute",
						&origin_string,
		                "connection::put_data() unexpected error from send(): %d",
							ws_error);
            }
        }
        else 
        {
            length -= num_sent;
            data += num_sent;
        }
    }
}

//---------------------------------------------------------------------------
// returns 0 if all is OK
void SMTP:: 
SendBuffer(const char *data, unsigned long length)
{
    DWORD             retval = 0;
    unsigned int    sorta_sent = 0;

    while( length )
    {
        if( (out_index + length) < SOCKET_BUFFER_SIZE ) 
        {
            // we won't overflow, simply copy into the buffer
            memcpy(out_buffer + out_index, data, length);
            out_index += length;
            length = 0;
        }
        else 
        {
            unsigned int orphaned_chunk = SOCKET_BUFFER_SIZE - out_index;

            // we will overflow, handle it
            memcpy(out_buffer + out_index, data, orphaned_chunk);

            // send this buffer...
            SendLine(out_buffer, SOCKET_BUFFER_SIZE); 

            length -= orphaned_chunk;
            out_index = 0;
            data += orphaned_chunk;
        }
    }
}

//---------------------------------------------------------------------------
// returns 0 if all is OK
void SMTP:: 
FlushBuffer()
{
    SendLine(out_buffer, out_index);
	out_index = 0;
}

//---------------------------------------------------------------------------
BOOL SMTP:: 
CloseConnect()
{
    if( closesocket(the_socket) == SOCKET_ERROR )
        return FALSE;

    return TRUE;
}

//----------------------------------------------------------------------
void SMTP::
SendSmtpError(const char * message)
{
	SendLine("QUIT\r\n", 6);
	CloseConnect();

	throw Exception("smtp.execute",
		&origin_string,
		"failed: %s", message);
}

//----------------------------------------------------------------------
// returns 0 if all is OK
// returns 20, 21, 22, 23, 24, 25 if SendBuffer() fails
// returns 26 if FlushBuffer() fails
void SMTP::
transform_and_send_edit_data(const char * editptr )
{
	const char      *index;
	const char      *header_end;
	char            previous_char = 'x';
	unsigned int    send_len;
	BOOL            done = 0;

	send_len = lstrlen(editptr);
	index = editptr;

	header_end = strstr(editptr, "\r\n\r\n");

	while( !done )
	{
		// room for extra char for double dot on end case
		while( (unsigned int)(index - editptr) < send_len )
		{
			switch( *index )
			{
				case '.':
					if( previous_char == '\n' )
                    {
					    /* send _two_ dots... */
    					SendBuffer(index, 1);
                    }
	  				SendBuffer(index, 1);
					break;

				case '\r':
					// watch for soft-breaks in the header, and ignore them
					if( index < header_end && (strncmp(index, "\r\r\n", 3) == 0) )
						index += 2;
					else
						if( previous_char != '\r' )
							SendBuffer(index, 1);
					// soft line-break (see EM_FMTLINES), skip extra CR */
					break;
				default:
					SendBuffer(index, 1);
					break;
			}
			previous_char = *index;
			index++;
		}

		if( (unsigned int)(index - editptr) == send_len )
            done = 1;
	}

	// this handles the case where the user doesn't end the last
	// line with a <return>

	if( editptr[send_len-1] != '\n' )
		SendBuffer("\r\n.\r\n", 5);
	else
		SendBuffer(".\r\n", 3);
	/* now make sure it's all sent... */
	FlushBuffer();
}

//----------------------------------------------------------------------
// returns 0 if all is OK
// returns 16 if any get_line()'s fail
// returns 20, 21, 22, 23, 24, 25, 26 if transform_and_send_edit_data() fails
void SMTP::
send_data(const char * message)
{
	transform_and_send_edit_data(message);
	if( 250 != get_line() )
		SendSmtpError("Message not accepted by server");
}

//----------------------------------------------------------------------
// returns 0 if all is OK
// returns 50, 51, 52 if fails
void SMTP::
open_socket( const char *server, const char *service )
{
	ConnectToHost(server, service);

    if( gethostname(my_hostname, sizeof(my_hostname)) )
		throw Exception("smtp.connect",
			&origin_string,
			"lookup of '%s' failed", my_hostname);
}

//----------------------------------------------------------------------
// returns 0 if all is OK
// returns 50, 51, 52 if open_socket() fails
// returns 10, 11, 12, 13, 14, 15 if any get_line()'s fail
void SMTP::
prepare_message(char *from, char *to, const char *server, const char *service)
{
	char	out_data[MAXOUTLINE];
	char	*ptr;
	int		len;
	int		startLen;

	open_socket(server, service);

	if( 220 != get_line() )
		SendSmtpError("SMTP server error");

	wsprintf(out_data, "HELO %s\r\n", my_hostname );
	SendLine(out_data, lstrlen(out_data) );

	if( 250 != get_line() )
	    SendSmtpError("SMTP server error");

	wsprintf(out_data, "MAIL From: <%s>\r\n", from);
	SendLine(out_data, lstrlen(out_data) );

	if( 250 != get_line() )
		SendSmtpError("The mail server doesn't like the sender name, have you set your mail address correctly?");

	// do a series of RCPT lines for each name in address line
	for( ptr = to; *ptr; ptr += len + 1 )
	{
		// if there's only one token left, then len will = startLen,
		// and we'll iterate once only
		startLen = lstrlen(ptr);
		if( (len = strcspn(ptr, " ,\n\t\r")) != startLen )
		{
			ptr[len] = '\0';			// replace delim with NULL char
			while( strchr (" ,\n\t\r", ptr[len+1]) )	// eat white space
				ptr[len++] = '\0';
		}

		wsprintf(out_data, "RCPT To: <%s>\r\n", ptr);
		SendLine(out_data, lstrlen(out_data) );

		if( 250 != get_line() )
			throw Exception("smtp.execute",
				&origin_string,
				"The mail server doesn't like the name %s. Have you set the 'To: ' field correctly?", 
					ptr);

		if( len == startLen )	// last token, we're done
			break;
	}

	wsprintf(out_data, "DATA\r\n");
	SendLine(out_data, lstrlen(out_data));

	if( 354 != get_line() )
		SendSmtpError("Mail server error accepting message data");
}


static char *lsplit(char *string, char delim) {
    if(string) {
		char *v=strchr(string, delim);
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return 0;
}
static char *rsplit(char *string, char delim) {
    if(string) {
		char *v=strrchr(string, delim);
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return NULL;	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// returns 0 if all is OK
// returns 1 if MakeSmtpHeader() fails
// returns 10, 11, 12, 13, 14, 15, 50, 51, 52 if prepare_message() fails
void SMTP::
Send(const char *server, const char *service, const char *msg, char *from, char *to)
{
    prepare_message( from, to, server, service);

    send_data(msg);

	SendLine("QUIT\r\n", 6 );
	CloseConnect();
}
