/** @file
	Parser: ISAPI memory manager impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.4 2001/09/15 11:48:41 parser Exp $
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

void Pool::register_cleanup(void (*cleanup) (void *), void *data) {
	if(fstorage)
		static_cast<Pool_storage *>(fstorage)->register_cleanup(cleanup, data);
	else
		Pool& pool=this;
		PTHROW(0, 0,
			0,
			"can not register cleanup on system pool");
}