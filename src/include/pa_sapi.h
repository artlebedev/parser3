/** @file
	Parser: web server api interface object decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SAPI_H
#define PA_SAPI_H

static const char * const IDENT_SAPI_H="$Date: 2004/02/12 06:46:31 $";

// includes


#include "pa_types.h"
#include "pa_array.h"

// forwards
class SAPI_Info;

/// target web-Server API
struct SAPI {
	/// log error message
	static void log(SAPI_Info& info, const char* fmt, ...);
	/// log error message & exit
	static void die(const char* fmt, ...);
	/// log error message & abort[write core]
	static void abort(const char* fmt, ...);
	/// environment strings
	static const char* const* environment(SAPI_Info& info);
	/// get environment string
	static char* get_env(SAPI_Info& info, const char* name);
	/// read POST request bytes
	static size_t read_post(SAPI_Info& info, char *buf, size_t max_bytes);
	/// add response header attribute [but do not send it to client]
	static void add_header_attribute(SAPI_Info& info, 
		const char* dont_store_key, const char* dont_store_value);
	/// send collected header attributes to client
	static void send_header(SAPI_Info& info);
	/// output body bytes
	static void send_body(SAPI_Info& info, const void *buf, size_t size);
};

#endif
