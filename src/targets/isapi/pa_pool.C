/** @file
	Parser: ISAPI memory manager impl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_POOL_C="$Id: pa_pool.C,v 1.14 2002/08/01 11:26:53 paf Exp $";

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
