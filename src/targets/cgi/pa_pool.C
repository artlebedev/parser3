/** @file
	Parser: CGI memory manager impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.5 2001/05/15 10:48:53 parser Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"

size_t malloc_total=0, calloc_total=0;
size_t malloc_times=0, calloc_times=0;

void *Pool::real_malloc(size_t size) {
	malloc_total+=size;
	malloc_times++;
	return ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
	calloc_total+=size;
	calloc_times++;
	return ::calloc(size, 1);
}
