/** @file
	Parser: CGI memory manager impl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_pool.C,v 1.37 2002/02/20 09:13:08 paf Exp $
*/

#include "pa_pool.h"
#include "pool_storage.h"

#define DEBUG_STRING_APPENDS_VS_EXPANDS

#ifdef DEBUG_STRING_APPENDS_VS_EXPANDS
unsigned long total_alloc_size=0;
#endif

void *Pool::real_malloc(size_t size, int place) {
#ifdef DEBUG_STRING_APPENDS_VS_EXPANDS
	total_alloc_size+=size;
#endif

	return fstorage?
		static_cast<Pool_storage *>(fstorage)->malloc(size): ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
#ifdef DEBUG_STRING_APPENDS_VS_EXPANDS
	total_alloc_size+=size;
#endif

	return fstorage?
		static_cast<Pool_storage *>(fstorage)->calloc(size): ::calloc(size, 1);
}

bool Pool::real_register_cleanup(void (*cleanup) (void *), void *data) {
	return fstorage!=0 && static_cast<Pool_storage *>(fstorage)->register_cleanup(cleanup, data);
}
