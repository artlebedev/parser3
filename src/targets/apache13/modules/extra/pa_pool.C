/** @file
	Parser: Apache module memory manager impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.8 2001/09/15 13:20:22 parser Exp $
*/

//#include <stdlib.h>

#include "httpd.h"

#include "pa_pool.h"

void *Pool::real_malloc(size_t size) {
	return ap_palloc((pool*)fstorage, size);
}

void *Pool::real_calloc(size_t size) {
	return ap_pcalloc((pool*)fstorage, size);
}

void Pool::real_register_cleanup(void (*cleanup) (void *), void *data) {
	ap_register_cleanup((pool*)fstorage, data,
		cleanup,
		ap_null_cleanup);
}