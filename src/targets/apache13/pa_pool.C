/** @file
	Parser: Apache module memory manager impl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_POOL_C="$Date: 2003/01/21 15:51:16 $";

#include "httpd.h"

#include "pa_pool.h"

void *Pool::real_malloc(size_t size, int place) {
	return ap_palloc((pool*)fstorage, size);
}

void *Pool::real_calloc(size_t size) {
	return ap_pcalloc((pool*)fstorage, size);
}

bool Pool::real_register_cleanup(void (*cleanup) (void *), void *data) {
	/* NB any code which invokes register_cleanup ... directly
	 * is a critical section which should be guarded by block_alarms() and
	 * unblock_alarms() ...
	 */

	ap_block_alarms();

	ap_register_cleanup((pool*)fstorage, data,
		cleanup,
		ap_null_cleanup);

	ap_unblock_alarms();

	return true;
}
