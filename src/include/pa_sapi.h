/** @file
	Parser: web server api interface object decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sapi.h,v 1.8 2001/04/09 16:04:47 paf Exp $
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
	/// 
	static const char *get_env(Pool& pool, const char *name);
	/// read POST request bytes
	static size_t read_post(Pool& pool, char *buf, size_t max_bytes);
	/// add response header attribute
	static void add_header_attribute(Pool& pool, const char *key, const char *value);
	/// add response header attribute
	static void send_header(Pool& pool);
	/// output body bytes
	static void send_body(Pool& pool, const void *buf, size_t size);
};

#endif
