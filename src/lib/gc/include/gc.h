/* 
 * Copyright 1988, 1989 Hans-J. Boehm, Alan J. Demers
 * Copyright (c) 1991-1995 by Xerox Corporation.  All rights reserved.
 * Copyright 1996-1999 by Silicon Graphics.  All rights reserved.
 * Copyright 1999 by Hewlett-Packard Company.  All rights reserved.
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */

/*
 * Note that this defines a large number of tuning hooks, which can
 * safely be ignored in nearly all cases.  For normal use it suffices
 * to call only GC_MALLOC and perhaps GC_REALLOC.
 * For better performance, also look at GC_MALLOC_ATOMIC, and
 * GC_enable_incremental.  If you need an action to be performed
 * immediately before an object is collected, look at GC_register_finalizer.
 * If you are using Solaris threads, look at the end of this file.
 * Everything else is best ignored unless you encounter performance
 * problems.
 */
 
#ifndef _GC_H

# define _GC_H

# define __GC
# include <stddef.h>

# ifdef _WIN32_WCE
/* Yet more kluges for WinCE */
#   include <stdlib.h>		/* size_t is defined here */
    typedef long ptrdiff_t;	/* ptrdiff_t is not defined */
# endif

#if defined(__MINGW32__) && defined(_DLL) && !defined(GC_NOT_DLL)
# ifdef GC_BUILD
#   define GC_API __declspec(dllexport)
# else
#   define GC_API __declspec(dllimport)
# endif
#endif

#if (defined(__DMC__) || defined(_MSC_VER)) && (defined(_DLL) && !defined(GC_NOT_DLL) || defined(GC_DLL))
# ifdef GC_BUILD
#   define GC_API extern __declspec(dllexport)
# else
#   define GC_API __declspec(dllimport)
# endif
#endif

#ifndef GC_API
#define GC_API extern
#endif

# if defined(__STDC__) || defined(__cplusplus)
#   define GC_PROTO(args) args
    typedef void * GC_PTR;
#   define GC_CONST const
# else
#   define GC_PROTO(args) ()
    typedef char * GC_PTR;
#   define GC_CONST
#  endif

# ifdef __cplusplus
    extern "C" {
# endif


typedef unsigned long GC_word;
typedef long GC_signed_word;

/* Public procedures */

/* Initialize the collector.  This is only required when using thread-local
 * allocation, since unlike the regular allocation routines, GC_local_malloc
 * is not self-initializing.  If you use GC_local_malloc you should arrange
 * to call this somehow (e.g. from a constructor) before doing any allocation.
 */
GC_API void GC_init GC_PROTO((void));

/* Disable garbage collection.  Even GC_gcollect calls will be          */
/* ineffective.                                                         */
GC_API void GC_disable(void);

/* Try to re-enable garbage collection.  GC_disable() and GC_enable()   */
/* calls nest.  Garbage collection is enabled if the number of calls to */
/* both functions is equal.                                             */
GC_API void GC_enable(void);

/*
 * general purpose allocation routines, with roughly malloc calling conv.
 * The atomic versions promise that no relevant pointers are contained
 * in the object.  The nonatomic versions guarantee that the new object
 * is cleared.  GC_malloc_stubborn promises that no changes to the object
 * will occur after GC_end_stubborn_change has been called on the
 * result of GC_malloc_stubborn. GC_malloc_uncollectable allocates an object
 * that is scanned for pointers to collectable objects, but is not itself
 * collectable.  The object is scanned even if it does not appear to
 * be reachable.  GC_malloc_uncollectable and GC_free called on the resulting
 * object implicitly update GC_non_gc_bytes appropriately.
 */
GC_API GC_PTR GC_malloc GC_PROTO((size_t size_in_bytes));
GC_API GC_PTR GC_malloc_atomic GC_PROTO((size_t size_in_bytes));

/* Explicitly deallocate an object.  Dangerous if used incorrectly.     */
/* Requires a pointer to the base of an object.				*/
/* If the argument is stubborn, it should not be changeable when freed. */
/* An object should not be enable for finalization when it is 		*/
/* explicitly deallocated.						*/
/* GC_free(0) is a no-op, as required by ANSI C for free.		*/
GC_API void GC_free GC_PROTO((GC_PTR object_addr));

/* For compatibility with C library.  This is occasionally faster than	*/
/* a malloc followed by a bcopy.  But if you rely on that, either here	*/
/* or with the standard C library, your code is broken.  In my		*/
/* opinion, it shouldn't have been invented, but now we're stuck. -HB	*/
/* The resulting object has the same kind as the original.		*/
/* If the argument is stubborn, the result will have changes enabled.	*/
/* It is an error to have changes enabled for the original object.	*/
/* Follows ANSI comventions for NULL old_object.			*/
GC_API GC_PTR GC_realloc GC_PROTO((GC_PTR old_object, size_t new_size_in_bytes));

/* Explicitly trigger a full, world-stop collection. 	*/
GC_API void GC_gcollect GC_PROTO((void));

/* We try to make sure that we allocate at least N/GC_free_space_divisor bytes between collections */
GC_API void GC_set_free_space_divisor(GC_word);
GC_API GC_word GC_get_free_space_divisor(void);

/* Return the number of bytes in the heap.  Excludes collector private	*/
/* data structures.  Includes empty blocks and fragmentation loss.	*/
/* Includes some pages that were allocated but never written.		*/
GC_API size_t GC_get_heap_size GC_PROTO((void));

/* Return a lower bound on the number of free bytes in the heap.	*/
GC_API size_t GC_get_free_bytes GC_PROTO((void));

/* Return the number of bytes allocated since the last collection.	*/
GC_API size_t GC_get_bytes_since_gc GC_PROTO((void));

/* Return the total number of bytes allocated in this process.		*/
/* Never decreases.							*/
GC_API size_t GC_get_total_bytes GC_PROTO((void));

#if defined(__sgi) && !defined(__GNUC__) && _COMPILER_VERSION >= 720
#   define GC_ADD_CALLER
#   define GC_RETURN_ADDR (GC_word)__return_address
#endif

#ifdef GC_ADD_CALLER
#  define GC_EXTRAS GC_RETURN_ADDR, __FILE__, __LINE__
#  define GC_EXTRA_PARAMS GC_word ra, GC_CONST char * s, int i
#else
#  define GC_EXTRAS __FILE__, __LINE__
#  define GC_EXTRA_PARAMS GC_CONST char * s, int i
#endif

/* Debugging (annotated) allocation.  GC_gcollect will check 		*/
/* objects allocated in this way for overwrites, etc.			*/
GC_API GC_PTR GC_debug_malloc GC_PROTO((size_t size_in_bytes, GC_EXTRA_PARAMS));
GC_API GC_PTR GC_debug_malloc_atomic GC_PROTO((size_t size_in_bytes, GC_EXTRA_PARAMS));
GC_API void GC_debug_free GC_PROTO((GC_PTR object_addr));
GC_API GC_PTR GC_debug_realloc GC_PROTO((GC_PTR old_object, size_t new_size_in_bytes, GC_EXTRA_PARAMS));

/* Routines that allocate objects with debug information (like the 	*/
/* above), but just fill in dummy file and line number information.	*/
/* Thus they can serve as drop-in malloc/realloc replacements.  This	*/
/* can be useful for two reasons:  					*/
/* 1) It allows the collector to be built with DBG_HDRS_ALL defined	*/
/*    even if some allocation calls come from 3rd party libraries	*/
/*    that can't be recompiled.						*/
/* 2) On some platforms, the file and line information is redundant,	*/
/*    since it can be reconstructed from a stack trace.  On such	*/
/*    platforms it may be more convenient not to recompile, e.g. for	*/
/*    leak detection.  This can be accomplished by instructing the	*/
/*    linker to replace malloc/realloc with these.			*/
GC_API GC_PTR GC_debug_malloc_replacement GC_PROTO((size_t size_in_bytes));
GC_API GC_PTR GC_debug_realloc_replacement GC_PROTO((GC_PTR object_addr, size_t size_in_bytes));

# ifdef GC_DEBUG
#   define GC_MALLOC(sz) GC_debug_malloc(sz, GC_EXTRAS)
#   define GC_MALLOC_ATOMIC(sz) GC_debug_malloc_atomic(sz, GC_EXTRAS)
#   define GC_REALLOC(old, sz) GC_debug_realloc(old, sz, GC_EXTRAS)
#   define GC_FREE(p) GC_debug_free(p)
# else
#   define GC_MALLOC(sz) GC_malloc(sz)
#   define GC_MALLOC_ATOMIC(sz) GC_malloc_atomic(sz)
#   define GC_REALLOC(old, sz) GC_realloc(old, sz)
#   define GC_FREE(p) GC_free(p)
# endif
/* The following are included because they are often convenient, and	*/
/* reduce the chance for a misspecifed size argument.  But calls may	*/
/* expand to something syntactically incorrect if t is a complicated	*/
/* type expression.  							*/
# define GC_NEW(t) (t *)GC_MALLOC(sizeof (t))
# define GC_NEW_ATOMIC(t) (t *)GC_MALLOC_ATOMIC(sizeof (t))


/* GC_set_warn_proc can be used to redirect or filter warning messages.	*/
/* p may not be a NULL pointer.						*/
typedef void (*GC_warn_proc) GC_PROTO((char *msg, GC_word arg));
GC_API void GC_set_warn_proc GC_PROTO((GC_warn_proc p));

/* GC_ignore_warn_proc may be used as an argument for GC_set_warn_proc  */
/* to suppress all warnings (unless statistics printing is turned on).  */
GC_API void GC_ignore_warn_proc(char *, GC_word);

#ifdef __cplusplus
    }  /* end of extern "C" */
#endif

#endif /* _GC_H */
