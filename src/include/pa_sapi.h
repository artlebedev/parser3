/** @file
	Parser: web server api interface object decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sapi.h,v 1.4 2001/04/04 10:50:34 paf Exp $
*/

#ifndef PA_SAPI_H
#define PA_SAPI_H

#include "pa_pool.h"
#include "pa_types.h"

/// target web-Server API
struct SAPI {
	/// get env
	static const char *get_env(Pool& pool, const char *name);
	/// read POST request bytes
	static uint read_post(Pool& pool, char *buf, uint max_bytes);
	/// add response header attribute
	static void add_header_attribute(Pool& pool, const char *key, const char *value);
	/// add response header attribute
	static void send_header(Pool& pool);
	/// output body bytes
	static void send_body(Pool& pool, const void *buf, size_t size);
	/// log error message
	static void log(Pool& pool, const char *fmt, ...);
};

#endif
