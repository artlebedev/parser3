/*
	Parser: socks initialization/finalization.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_socks.C,v 1.1 2001/04/07 14:23:34 paf Exp $
*/

#ifdef WIN32
#include <winsock.h>
#else
//#include <netinet/tcp.h>
//#include <arpa/nameser.h>
//#include <netdb.h>
//#include <signal.h>
#endif

#include "pa_socks.h"
#include "pa_exception.h"


/* ************************ Common stuff *********************** */

#ifdef WIN32

WSADATA wsaData;

void init_socks(Pool& pool) {
	WORD wVersionRequested;
	int err; 
	wVersionRequested = MAKEWORD( 1, 1 ); 
	//wVersionRequested = MAKEWORD( 2, 2 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
	    /* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */    
		PTHROW(0, 0,
			0,
			"can not WSAStartup, err=%d", 
				err);
	} 
}

void done_socks() {
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

#else

void init_socks(Pool& pool) {}
void done_socks() {}

#endif
