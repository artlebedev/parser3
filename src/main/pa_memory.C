/** @file
	Parser: memory reference counting classes.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_MEMORY_C="$Date: 2005/08/09 08:14:52 $";

#include "pa_sapi.h"
#include "pa_common.h"

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


#ifdef PA_DEBUG_GC_MEMORY

const size_t HEADTAIL_SIZE=4;

static size_t debug_size(size_t user_size) {
	return user_size+HEADTAIL_SIZE*2;
}

const int BEFORE_MARK=0xBEF0BEF0;
const int AFTER_MARK=0xAFEEAFEE;
const char PAD_MARK='\xAD';

static void* fill_return_user(void* aptr, size_t pure_size) {
	char* ptr=(char*)aptr;
	memcpy(ptr, &BEFORE_MARK, HEADTAIL_SIZE);
	memcpy(ptr+pure_size-HEADTAIL_SIZE, &AFTER_MARK, HEADTAIL_SIZE);

	// pAD
	size_t raw_size=GC_size(aptr);
	for(size_t i=pure_size; i<raw_size; i++)
		ptr[i]=PAD_MARK;

	//if(ptr>=(char*)0x01c357e40 && ptr<=((char*)0x01c357e4+100))
		//printf("valid:0x%p\n", ptr);

	return ptr+HEADTAIL_SIZE;
}
static void* check_return_debug(void* auser_ptr) {
	char* user_ptr=(char*)auser_ptr;
	char* ptr=user_ptr-HEADTAIL_SIZE;

	size_t raw_size=GC_size(ptr);
	assert(raw_size!=0);

	size_t pure_size=raw_size;
	while(ptr[pure_size-1]==PAD_MARK)
		pure_size--;

	assert(memcmp(ptr, &BEFORE_MARK, HEADTAIL_SIZE)==0);
	assert(memcmp(ptr+pure_size-HEADTAIL_SIZE, &AFTER_MARK, HEADTAIL_SIZE)==0);

	return ptr;
}


void* pa_gc_malloc(size_t size) {
	size=debug_size(size);
	return fill_return_user(GC_MALLOC(size), size);
}

void* pa_gc_malloc_atomic(size_t size) {
	size=debug_size(size);
	return fill_return_user(GC_MALLOC_ATOMIC(size), size);
}

void* pa_gc_realloc(void* user_ptr, size_t size) {
	GC_is_visible(user_ptr);

	//printf("realloc: 0x%p -> %u\n", ptr, size);
	size=debug_size(size);
	return fill_return_user(
		GC_realloc(
			check_return_debug(user_ptr), 
			size),
		size);
}
void pa_gc_free(void* ptr) {
	// ignore free
}


#endif
