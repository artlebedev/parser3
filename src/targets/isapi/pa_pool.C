/** @file
	Parser: ISAPI memory manager impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.7 2001/09/26 10:32:26 parser Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"
#include "pool_storage.h"

void *Pool::real_malloc(size_t size) {
	return fstorage?
		static_cast<Pool_storage *>(fstorage)->malloc(size): ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
	return fstorage?
		static_cast<Pool_storage *>(fstorage)->calloc(size): ::calloc(size, 1);
}

bool Pool::real_register_cleanup(void (*cleanup) (void *), void *data) {
	return fstorage!=0 && static_cast<Pool_storage *>(fstorage)->register_cleanup(cleanup, data);
}