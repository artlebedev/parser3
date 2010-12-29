/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_HTTP_H
#define PA_HTTP_H

static const char * const IDENT_HTTP_H="$Date: 2010/12/29 12:17:58 $";

#include "pa_vstring.h"
#include "pa_vint.h"
#include "pa_vhash.h"
#include "pa_vtable.h"
#include "pa_socks.h"

#ifdef CYGWIN
#define _GNU_H_WINDOWS32_SOCKETS
// for PASCAL
#include <windows.h>
// SOCKET
typedef u_int	SOCKET;
int PASCAL closesocket(SOCKET);
#else
#	if defined(WIN32)
#		include <windows.h>
#	else
#		define closesocket close
#	endif
#endif

#ifndef DOXYGEN
struct File_read_http_result {
	char *str; size_t length;
	HashStringValue* headers;
}; 
#endif

char *pa_http_safe_header_name(const char *name);

File_read_http_result pa_internal_file_read_http(Request& r, 
						const String& file_spec, 
						bool as_text,
						HashStringValue *options=0,
						bool transcode_text_result=true);

#endif
