/** @file
	Parser: Apache module memory manager impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.6 2001/04/26 15:09:10 paf Exp $
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
