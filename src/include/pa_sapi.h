/** @file
	Parser: web server api interface object decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_sapi.h,v 1.19 2002/06/11 12:20:42 paf Exp $
*/

#ifndef PA_SAPI_H
#define PA_SAPI_H

#include "pa_pool.h"
#include "pa_types.h"

class Array;
class String;
class Hash;

/// target web-Server API
struct SAPI {
	/// log error message
	static void log(Pool& pool, const char *fmt, ...);
	/// log error message & die
	static void die(const char *fmt, ...);
	/// environment strings
	static const char *const *environment(Pool& pool);
	/// get environment string
	static const char *get_env(Pool& pool, const char *name);
	/// read POST request bytes
	static size_t read_post(Pool& pool, char *buf, size_t max_bytes);
	/// add response header attribute [but do not send it to client]
	static void add_header_attribute(Pool& pool, const char *key, const char *value);
	/// send collected header attributes to client
	static void send_header(Pool& pool);
	/// output body bytes
	static void send_body(Pool& pool, const void *buf, size_t size);
};

#endif
