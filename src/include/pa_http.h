/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_HTTP_H
#define PA_HTTP_H

#define IDENT_PA_HTTP_H "$Id: pa_http.h,v 1.9 2013/03/10 23:32:36 misha Exp $"

#include "pa_vstring.h"
#include "pa_vint.h"
#include "pa_vhash.h"
#include "pa_vtable.h"
#include "pa_socks.h"

#ifdef WIN32
#include <windows.h>

#ifdef CYGWIN
typedef u_int SOCKET;
extern "C" int PASCAL FAR closesocket(SOCKET);
#endif

#else // WIN32
#define closesocket close
#endif

#ifndef DOXYGEN
struct File_read_http_result {
	char *str; size_t length;
	HashStringValue* headers;
}; 
#endif

Table* parse_cookies(Request& r, Table *cookies);

char *pa_http_safe_header_name(const char *name);

File_read_http_result pa_internal_file_read_http(Request& r, 
						const String& file_spec, 
						bool as_text,
						HashStringValue *options=0,
						bool transcode_text_result=true);

#endif
