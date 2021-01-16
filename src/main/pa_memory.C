/** @file
	Parser: memory reference counting classes.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_threads.h"

volatile const char * IDENT_PA_MEMORY_C="$Id: pa_memory.C,v 1.17 2021/01/16 15:47:05 moko Exp $" IDENT_PA_MEMORY_H;

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

int pa_free_space_divisor = 0;

void pa_gc_collect(bool forced){
#ifndef PA_DEBUG_DISABLE_GC
    int divisor = pa_free_space_divisor; // as it can change during collect in multithreaded enviroment
    if(!divisor) GC_enable();
    if(!divisor || forced) GC_gcollect();
    if(!divisor) GC_disable();
#endif
}

void pa_gc_set_free_space_divisor(int divisor){
#ifndef PA_DEBUG_DISABLE_GC
	if(divisor != pa_free_space_divisor){
		SYNCHRONIZED;
		if(pa_free_space_divisor){
			if(!divisor) GC_disable();
		} else {
			if(divisor) GC_enable();
		}
		if(divisor)
			GC_set_free_space_divisor(divisor);
		pa_free_space_divisor = divisor;
	}
#endif
}
