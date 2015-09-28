/** @file
	Parser: includes all Configure-d headers

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	when used Configure [HAVE_CONFIG_H] it uses defines from Configure,
	fixed otherwise.
*/

#ifndef PA_CONFIG_INCLUDES_H
#define PA_CONFIG_INCLUDES_H

#if HAVE_CONFIG_H
# include "pa_config_auto.h"
#else
# include "pa_config_fixed.h"
#endif

// AC_INCLUDES_DEFAULT

#ifdef HAVE_STDIO_H
# include <stdio.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_STDDEF_H
# include <stddef.h>
#endif

#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

// other includes

#ifdef HAVE_ASSERT_H
# include <assert.h>
#endif

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#ifdef HAVE_CTYPE_H
# include <ctype.h>
#endif

#ifdef HAVE_MATH_H
# include <math.h>
#endif

#ifdef HAVE_PROCESS_H
# include <process.h>
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_SETJMP_H
# include <setjmp.h>
#endif

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#ifdef HAVE_DIRENT_H
# include <dirent.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#ifdef HAVE_IO_H
# include <io.h>
#endif

#ifdef HAVE_SYS_FILE_H
# include <sys/file.h>
#endif

#ifdef HAVE_SYS_LOCKING_H
# include <sys/locking.h>
#endif

#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#ifdef HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#endif

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif


#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#ifdef HAVE_NETDB_H
# include <netdb.h>
#endif


#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

// math function replacements

#ifdef __cplusplus

#ifndef HAVE_TRUNC
inline double trunc(double param) { return param > 0? floor(param) : ceil(param); }
#endif

#ifndef HAVE_ROUND
inline double round(double param) { return floor(param+0.5); }
#endif

#ifndef HAVE_SIGN
inline double sign(double param) { return param > 0 ? 1 : ( param < 0 ? -1 : 0 ); }
#endif

#if !defined(max)
inline int max(int a, int b) { return a>b?a:b; }
inline int min(int a, int b){ return a<b?a:b; }
inline size_t max(size_t a, size_t b) { return a>b?a:b; }
inline size_t min(size_t a, size_t b){ return a<b?a:b; }
#endif

#endif

#ifdef WIN32
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL // __thread // multithreading support required only for apache2 module
#endif

#pragma clang diagnostic ignored "-Wparentheses"       // if(a=b)
#pragma clang diagnostic ignored "-Winline-new-delete" // replacement operator cannot be declared 'inline'

#if _MSC_VER

#pragma warning(disable:4355)  // using this in calls to parent constructors

#pragma warning(disable:4291)  // no matching operator delete found; memory will not be freed if initialization throws an exception

#pragma warning(disable:4786)  // identifier was truncated to '255' characters in the debug information

#pragma warning(disable:4511)  // copy constructor could not be generated

#pragma warning(disable:4512)  // assignment operator could not be generated

#pragma warning(disable:4505)  // unreferenced local function has been removed

#pragma warning(disable:4127)  // conditional expression is constant; while(true) OK

#pragma warning(disable:4706)  // assignment within conditional expression if(type var=somethig) OK

#pragma warning(disable:4996)  // memcpy deprecation

#pragma warning(disable:4702)  // unreachable code

#endif

/// to debug backtrace pointers switch this on:
#if defined(_DEBUG) || defined(PA_RELEASE_ASSERTS)
//// this switches on straightforward realloc implementation, bad for profiling
//// note: this is incompatible with -DGC_ASSERTIONS, which can be switched ON in win32/gc/NT_THREADS_MAKEFILE
#define GC_DEBUG
#endif

#endif
