/**	@file
	Parser: socks initialization/finalization.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_socks.C,v 1.12 2002/02/08 07:27:48 paf Exp $
*/

#define NO_UNISTD_H
#include "pa_config_includes.h"

#ifndef HAVE_SYS_SELECT_H
#include "pa_exception.h"
#include "pa_socks.h"

#include <winsock.h>

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
		throw Exception(0, 0,
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

#include "pa_pool.h"

void init_socks(Pool& pool) {}
void done_socks() {}

#endif
