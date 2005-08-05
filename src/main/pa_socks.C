/**	@file
	Parser: socks initialization/finalization.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_SOCKS_C="$Date: 2005/08/05 13:03:02 $";

#define NO_UNISTD_H
#include "pa_config_includes.h"

#ifdef WIN32
#include "pa_exception.h"
#include "pa_socks.h"
#include "pa_string.h"

// in cygwin: for pascal
// in MSVC: for everything
#include <windows.h>

#ifdef CYGWIN
// WSADATA
#define WSADESCRIPTION_LEN	256
#define WSASYS_STATUS_LEN	128
typedef struct WSAData {
	WORD	wVersion;
	WORD	wHighVersion;
	char	szDescription[WSADESCRIPTION_LEN+1];
	char	szSystemStatus[WSASYS_STATUS_LEN+1];
	unsigned short	iMaxSockets;
	unsigned short	iMaxUdpDg;
	char * 	lpVendorInfo;
} WSADATA;
typedef WSADATA *LPWSADATA;

int PASCAL WSAStartup(WORD,LPWSADATA);
int PASCAL WSACleanup(void);
#endif

WSADATA wsaData;

void pa_socks_init() {
	WORD wVersionRequested;
	int err; 
	wVersionRequested = MAKEWORD( 1, 1 ); 
	//wVersionRequested = MAKEWORD( 2, 2 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
	    /* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */    
		throw Exception(0,
			0,
			"can not WSAStartup, err=%d", 
				err);
	} 
}

void pa_socks_done() {
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */ 
	if ( LOBYTE( wsaData.wVersion ) == 2 ||
        HIBYTE( wsaData.wVersion ) == 2 ) {
		WSACleanup( );
		return; 
	} 
}

const char* pa_socks_strerr(int no) {
	char buf[MAX_STRING];
	buf[0]=0;
	size_t error_size=FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		no,
		0, // Default language
		(LPTSTR) &buf,
		sizeof(buf),
		NULL 
	);
	if(error_size>3) // ".\r\n"
		buf[error_size-3]=0;
	return buf[0]? pa_strdup(buf): "unknown error";
}
int pa_socks_errno() {
	return WSAGetLastError();
}

#else

void pa_socks_init() {}
void pa_socks_done() {}

const char* pa_socks_strerr(int no) {
	return strerror(no);
}
int pa_socks_errno() {
	return errno;
}

#endif
