/** @file
	Parser: memory reference counting classes decls.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_MEMORY_H
#define PA_MEMORY_H

#define IDENT_PA_MEMORY_H "$Id: pa_memory.h,v 1.21 2015/04/06 22:27:26 moko Exp $"

// include

#include "pa_config_includes.h"
#include "gc.h"

// define destructors use for Array, Hash and VMethodFrame
#define USE_DESTRUCTORS

inline void* pa_gc_malloc(size_t size) { return GC_MALLOC(size); }
inline void* pa_gc_malloc_atomic(size_t size) { return GC_MALLOC_ATOMIC(size); }
inline void* pa_gc_realloc(void* ptr, size_t size) { return GC_REALLOC(ptr, size); }
inline void pa_gc_free(void* ptr) { GC_FREE(ptr); }

// forwards

void *pa_fail_alloc(const char* what, size_t size);

// inlines

inline void *pa_malloc(size_t size) {
	if(void *result=pa_gc_malloc(size))
		return result;

	return pa_fail_alloc("allocate", size);
}

inline void *pa_malloc_atomic(size_t size) {
	if(void *result=pa_gc_malloc_atomic(size))
		return result;

	return pa_fail_alloc("allocate clean", size);
}

/// @a length may be null, which mean "autocalc it"
inline char *pa_strdup(const char* auto_variable_never_null, size_t helper_length=0) {
	size_t known_length= helper_length ? helper_length : strlen(auto_variable_never_null);

	size_t size=known_length+1;
	if(char *result=static_cast<char*>(pa_gc_malloc_atomic(size))) {
		memcpy(result, auto_variable_never_null, known_length);
		result[known_length]=0;
		return result;
	}

	return static_cast<char*>(pa_fail_alloc("allocate clean", size));
}

inline void pa_free(void *ptr) {
	pa_gc_free(ptr);
}

inline void *pa_realloc(void *ptr, size_t size) {
	if(void *result=pa_gc_realloc(ptr, size))
		return result;

	return pa_fail_alloc("reallocate to", size);
}

#define PointerFreeGC (true)

//{@ Array-oriented
inline void *operator new[] (size_t size) {
	return pa_malloc(size);
}
inline void *operator new[] (size_t size, bool) { // PointerFreeGC
	return pa_malloc_atomic(size);
}
inline void operator delete[] (void *ptr) {
	pa_free(ptr);
}
//}@

//{@ Structure-oriented
inline void *operator new(size_t size) {
	return pa_malloc(size);
}
inline void *operator new (size_t size, bool) { // PointerFreeGC
	return pa_malloc_atomic(size);
}
inline void operator delete(void *ptr) {
	pa_free(ptr);
}
//}@

// disabled from accidental use

void *calloc_disabled();
void *malloc_disabled();
void *realloc_disabled();
void free_disabled();
char *strdup_disabled();

inline void *calloc(size_t){ return calloc_disabled(); }
inline void *malloc(size_t){ return malloc_disabled(); }
inline void *realloc(void *, size_t){ return realloc_disabled(); }
inline void free(void *){ free_disabled(); }
inline char *strdup(const char*, size_t){ return strdup_disabled(); }

/// memory allocation/dallocation goes via pa_malloc/pa_free.
class PA_Allocated {
public:
	/// the sole: instances allocated using our functions
	static void *operator new(size_t size) { 
		return pa_malloc(size);
	}
	static void operator delete(void *ptr) {
		pa_free(ptr);
	}
};

/// Base for all Parser classes
typedef PA_Allocated PA_Object;

// defines

#define override
#define rethrow throw

#endif
