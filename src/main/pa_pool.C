/** @file
	Parser: pool class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_pool.C,v 1.46 2001/12/15 21:28:21 paf Exp $
*/

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_sapi.h"
#include "pa_charset.h"

#ifdef XML
#include <util/PlatformUtils.hpp>
#endif

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
		throw Exception(0, 0,
			0,
			"no source charset defined yet");
	return *source_charset; 
}

void Pool::set_client_charset(Charset& acharset) { 
	client_charset=&acharset; 
}
Charset& Pool::get_client_charset() { 
	if(!client_charset)
		throw Exception(0, 0,
			0,
			"no client charset defined yet");
	return *client_charset; 
}

#ifdef XML

const char *Pool::transcode_cstr(const XalanDOMString& s) { 
	return get_source_charset().transcode_cstr(s); 
}

String& Pool::transcode(const XalanDOMString& s) { 
	return get_source_charset().transcode(s); 
}

std::auto_ptr<XalanDOMString> Pool::transcode(const String& s) {
	return get_source_charset().transcode(s); 
}


#endif
