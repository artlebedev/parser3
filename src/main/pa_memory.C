/** @file
	Parser: memory reference counting classes.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_sapi.h"
#include "pa_common.h"

volatile const char * IDENT_PA_MEMORY_C="$Id: pa_memory.C,v 1.15 2017/11/15 22:48:58 moko Exp $" IDENT_PA_MEMORY_H;

void *pa_fail_alloc(const char* what, size_t size) {
#ifdef PA_DEBUG_DISABLE_GC
	SAPI::die("out of memory (in pa_fail_alloc)");
#else
	SAPI::die("out of memory: failed to %s %u bytes. heap_used=%u, heap_free=%u, bytes_since_gc=%u, total_bytes=%u",
		what, size, GC_get_heap_size(), GC_get_free_bytes(), GC_get_bytes_since_gc(), GC_get_total_bytes());
#endif
	// never reached
	return 0;
}

#ifdef _MSC_VER
extern "C" void *pa_fail_alloc(const char* what);
void *pa_fail_alloc(const char* what) {
	SAPI::die("fatal memory error: %s. heap_used=%u, heap_free=%u, bytes_since_gc=%u, total_bytes=%u",
		what, GC_get_heap_size(), GC_get_free_bytes(), GC_get_bytes_since_gc(), GC_get_total_bytes());
	// never reached
	return 0;
}
#endif
