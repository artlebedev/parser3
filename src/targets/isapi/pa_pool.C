/** @file
	Parser: ISAPI memory manager impl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_POOL_C="$Date: 2003/01/21 15:51:17 $";

//#include <stdlib.h>

#include "pa_pool.h"
#include "pool_storage.h"

void *Pool::real_malloc(size_t size, int place) {
	return fstorage?
		static_cast<Pool_storage *>(fstorage)->malloc(size): 0;
}

void *Pool::real_calloc(size_t size) {
	return fstorage?
		static_cast<Pool_storage *>(fstorage)->calloc(size): 0;
}

bool Pool::real_register_cleanup(void (*cleanup) (void *), void *data) {
	return fstorage!=0 && static_cast<Pool_storage *>(fstorage)->register_cleanup(cleanup, data);
}
