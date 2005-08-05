/** @file
	Parser: memory reference counting classes decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_MEMORY_H
#define PA_MEMORY_H

static const char * const IDENT_MEMORY_H="$Date: 2005/08/05 13:02:59 $";

// include

#include "pa_config_includes.h"

/// to debug backtrace pointers switch this on:
#ifdef _DEBUG
#define GC_DEBUG
#endif
#include "gc.h"

// defines

//#define PA_DEBUG_GC_MEMORY

#ifdef PA_DEBUG_GC_MEMORY
void* pa_gc_malloc(size_t size);
void* pa_gc_malloc_atomic(size_t size);
void* pa_gc_realloc(void* ptr, size_t size);
void pa_gc_free(void* ptr);
#else
inline void* pa_gc_malloc(size_t size) { return GC_MALLOC(size); }
inline void* pa_gc_malloc_atomic(size_t size) { return GC_MALLOC_ATOMIC(size); }
inline void* pa_gc_realloc(void* ptr, size_t size) { return GC_REALLOC(ptr, size); }
inline void pa_gc_free(void* ptr) { GC_FREE(ptr); }
#endif


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
	size_t known_length=(helper_length?helper_length:strlen(auto_variable_never_null));

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

//{@ these operators are disabled, one should explicitely specify either new(UseGC) or new(PointerFreeGC)
inline void *operator new(size_t) { abort(); } // disabled
inline void *operator new[] (size_t) { abort(); } // disabled
//}@

#define UseGC ((int)1)
#define PointerFreeGC (true)

//{@ Array-oriented
inline void *operator new[] (size_t size, int) { // UseGC
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
inline void *operator new (size_t size, int) { // UseGC
	return pa_malloc(size);
}
inline void *operator new (size_t size, bool) { // PointerFreeGC
	return pa_malloc_atomic(size);
}
inline void operator delete(void *ptr) {
	pa_free(ptr);
}
//}@

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
	static void *malloc(size_t size) {
		return pa_malloc(size);
	}
	static void *malloc_atomic(size_t size) {
		return pa_malloc_atomic(size);
	}
	static char *strdup(const char* auto_variable_never_null, size_t helper_length=0) {
		return pa_strdup(auto_variable_never_null, helper_length);
	}
	static void free(void *ptr) {
		pa_free(ptr);
	}
	static void *realloc(void *ptr, size_t size) {
		return pa_realloc(ptr, size);
	}

private: // disabled from accidental use

	/// use malloc/malloc_atomic instead [GC clears result of those]
	static void *calloc(size_t size);

};

/// Those who want their destructor called during finalization, must derive from this class [also]
class PA_Cleaned {
#ifndef PA_DEBUG_DISABLE_GC
	static void cleanup( void* obj, void* displ ) {
	    ((PA_Cleaned*) ((char*) obj + (ptrdiff_t) displ))->~PA_Cleaned();
	}

public:
	PA_Cleaned() {
		GC_finalization_proc oldProc;
		void* oldData;
		void* base = GC_base( (void *) this );
		if (0 != base)  {
			// Don't call the debug version, since this is a real base address.
			GC_register_finalizer_ignore_self( 
				base, (GC_finalization_proc)cleanup, (void*) ((char*) this - (char*) base), 
				&oldProc, &oldData );
			if (0 != oldProc) {
				GC_register_finalizer_ignore_self( base, oldProc, oldData, 0, 0 );
			}
		}
	}

	virtual ~PA_Cleaned() {
	    GC_REGISTER_FINALIZER_IGNORE_SELF( GC_base(this), 0, 0, 0, 0 );
	}
#endif
};

/// Base for all Parser classes
typedef PA_Allocated PA_Object;

// defines

#define override
#define rethrow throw


#endif
