/** @file
	Parser: pool class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_POOL_C="$Id: pa_pool.C,v 1.54 2002/08/01 11:26:50 paf Exp $";

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_sapi.h"
#include "pa_charset.h"

Pool::Pool(void *astorage) : 
	fstorage(astorage), fcontext(0), 
	ftotal_allocated(0), ftotal_times(0),
	source_charset(0), client_charset(0)
	{
}

void Pool::fail_alloc(size_t size) const {
	SAPI::die("out of pool memory: failed to allocate %u bytes; "
		"already allocated on pool: %u bytes in %u times", 
		size, 
		ftotal_allocated, ftotal_times);
}

void Pool::fail_register_cleanup() const {
	SAPI::die("failed to register cleanup");
}

void Pool::set_source_charset(Charset& acharset) { 
	source_charset=&acharset; 
}
Charset& Pool::get_source_charset() { 
	if(!source_charset)
		throw Exception(0,
			0,
			"no source charset defined yet");
	return *source_charset; 
}

void Pool::set_client_charset(Charset& acharset) { 
	client_charset=&acharset; 
}
Charset& Pool::get_client_charset() { 
	if(!client_charset)
		throw Exception(0,
			0,
			"no client charset defined yet");
	return *client_charset; 
}

#ifdef XML

const char *Pool::transcode_cstr(xmlChar *s) {
	return get_source_charset().transcode_cstr(s); 
}

String& Pool::transcode(xmlChar *s) {
	return get_source_charset().transcode(s); 
}

const char *Pool::transcode_cstr(GdomeDOMString *s) { 
	return get_source_charset().transcode_cstr(s); 
}

String& Pool::transcode(GdomeDOMString *s) { 
	return get_source_charset().transcode(s); 
}

xmlChar *Pool::transcode_buf2xchar(const char *buf, size_t buf_size) {
	return get_source_charset().transcode_buf2xchar(buf, buf_size); 
}

GdomeDOMString_auto_ptr Pool::transcode(const String& s) {
	return get_source_charset().transcode(s); 
}


#endif
