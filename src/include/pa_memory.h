/** @file
	Parser: memory reference counting classes decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_MEMORY_H
#define PA_MEMORY_H

#define IDENT_PA_MEMORY_H "$Id: pa_memory.h,v 1.43 2021/01/16 15:47:05 moko Exp $"

// include

#include "pa_config_includes.h"
#include <new>

// define destructors use for Array, Hash and VMethodFrame
#define USE_DESTRUCTORS

// std::basic_stringstream used in ^table.csv-string[] is compatible with delete usage check only under Debian 9/10, FreeBSD 12
// #define CHECK_DELETE_USAGE

// forwards

void *pa_fail_alloc(const char* what, size_t size);

// inlines

inline void *pa_malloc(size_t size) {
	if(void *result=GC_MALLOC(size))
		return result;

	return pa_fail_alloc("allocate", size);
}

inline void *pa_malloc_atomic(size_t size) {
	if(void *result=GC_MALLOC_ATOMIC(size))
		return result;

	return pa_fail_alloc("allocate clean", size);
}

/// length may be zero, and this is normal
inline char *pa_strdup(const char* auto_variable_never_null, size_t known_length) {
	if(char *result=static_cast<char*>(GC_MALLOC_ATOMIC(known_length+1))) {
		memcpy(result, auto_variable_never_null, known_length);
		result[known_length]=0;
		return result;
	}

	return static_cast<char*>(pa_fail_alloc("allocate clean", known_length+1));
}

inline char *pa_strdup(const char* auto_variable_never_null) {
	return pa_strdup(auto_variable_never_null, strlen(auto_variable_never_null));
}

inline void pa_free(void *ptr) {
	GC_FREE(ptr);
}

inline void *pa_realloc(void *ptr, size_t size) {
	if(void *result=GC_REALLOC(ptr, size))
		return result;

	return pa_fail_alloc("reallocate to", size);
}

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
	static void *operator new[](size_t size) {
		return pa_malloc(size);
	}
	static void operator delete[](void *ptr) {
		pa_free(ptr);
	}
};

// new(PointerFreeGC)/new(PointerGC) should be used to allocate types not inherited from PA_Allocated

#define PointerFreeGC (true)
#define PointerGC (false)

inline void *operator new[] (size_t size, bool pointer_free) {
	return pointer_free ? pa_malloc_atomic(size) : pa_malloc(size);
}

inline void *operator new (size_t size, bool pointer_free) {
	return pointer_free ? pa_malloc_atomic(size) : pa_malloc(size);
}

/// Base for all Parser classes
typedef PA_Allocated PA_Object;

// defines

#define override
#define rethrow throw

#if defined(_MSC_VER) || (__cplusplus>=201103L)
#define PA_THROW(what)
#else
#define PA_THROW(what) throw(what)
#endif

#if !defined(_MSC_VER) && !defined(FREEBSD1X)
// regular new/delete are disabled from accidental use
// no checks for FreeBSD1X.X due to https://bugs.llvm.org/show_bug.cgi?id=40161 bug

void *new_disabled();
void delete_disabled();

inline void *operator new[] (std::size_t) PA_THROW(std::bad_alloc){ return new_disabled(); }
inline void operator delete[](void *) throw(){ delete_disabled(); }
inline void operator delete[](void *, std::size_t) throw(){ delete_disabled(); }

inline void *operator new(std::size_t) PA_THROW(std::bad_alloc){ return new_disabled(); }
#ifdef CHECK_DELETE_USAGE
inline void operator delete(void *) throw(){ delete_disabled(); }
#endif

#ifndef PA_DEBUG_DISABLE_GC
// other regular allocators as disabled from accidental use as well

void *calloc_disabled();
void *malloc_disabled();
void *realloc_disabled();
void free_disabled();
char *strdup_disabled();

inline void *calloc(size_t) { return calloc_disabled(); }
inline void *malloc(size_t) { return malloc_disabled(); }
inline void *realloc(void *, size_t) { return realloc_disabled(); }
inline void free(void *) { free_disabled(); }
inline char *strdup(const char*, size_t){ return strdup_disabled(); }
#endif // PA_DEBUG_DISABLE_GC

#endif // _MSC_VER

void pa_gc_collect(bool forced=false);
void pa_gc_set_free_space_divisor(int);

#endif
