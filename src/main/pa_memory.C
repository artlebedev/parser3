/** @file
	Parser: memory reference counting classes.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_sapi.h"
#include "pa_common.h"

volatile const char * IDENT_PA_MEMORY_C="$Id: pa_memory.C,v 1.10 2013/07/23 15:39:10 moko Exp $" IDENT_PA_MEMORY_H;

void *pa_fail_alloc(const char* what, size_t size) {
#ifdef PA_DEBUG_DISABLE_GC
	SAPI::die("out of memory (in pa_fail_alloc)");
#else
	SAPI::die("out of memory: failed to %s %u bytes. "
		"heap_used=%u, heap_free=%u, bytes_since_gc=%u, total_bytes=%u",
		what, size,
		GC_get_heap_size(),
		GC_get_free_bytes(),
		GC_get_bytes_since_gc(),
		GC_get_total_bytes()
		);
#endif
	// never reached
	return 0;
}
