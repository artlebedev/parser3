/** @file
	Parser: includes all Configure-d headers

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	when used Configure [HAVE_CONFIG_H] it uses defines from Configure,
	fixed otherwise.
*/

#ifndef PA_CONFIG_INCLUDES_H
#define PA_CONFIG_INCLUDES_H

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#if HAVE_CONFIG_H
#	include "pa_config_auto.h"
#else
#	include "pa_config_fixed.h"
#endif

#ifdef __cplusplus
#	undef	inline
#endif

#ifdef HAVE_ASSERT_H
#	include <assert.h>
#endif

#ifdef HAVE_CTYPE_H
#	include <ctype.h>
#endif

#ifdef HAVE_ERRNO_H
#	include <errno.h>
#endif

#ifdef HAVE_FCNTL_H
#	include <fcntl.h>
#endif

#ifdef HAVE_IO_H
#	include <io.h>
#endif

#ifdef HAVE_PROCESS_H
#	include <process.h>
#endif

#ifdef HAVE_STDARG_H
#	include <stdarg.h>
#endif

#ifdef HAVE_STDDEF_H
#	include <stddef.h>
#endif

#ifdef HAVE_STDIO_H
#	include <stdio.h>
#endif

#ifdef HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif

#if defined(HAVE_UNISTD_H) && !defined(NO_UNISTD_H)
#	include <unistd.h>
#endif

#ifdef HAVE_MATH_H
#	include <math.h>
#endif

#include <stdlib.h>

#ifdef HAVE_STRING_H
#	include <string.h>
#endif

#ifdef HAVE_DIRECT_H
#	include <direct.h>
#endif

#ifdef HAVE_SETJMP_H
#	include <setjmp.h>
#endif

#ifdef HAVE_MEMORY_H
#	include <memory.h>
#endif

#ifdef HAVE_NEW
#include <new>
#endif

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#ifdef HAVE_SYS_LOCKING_H
#include <sys/locking.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#	include <sys/socket.h>
#else
#	if defined(HAVE_WINSOCK_H) && !defined(_MSC_VER) && !defined(CYGWIN)
#		include <winsock.h>
#	endif
#endif

// must go anywhere after winsock [cygwin time.h checks for already inclusion of winsock.h]
#if TIME_WITH_SYS_TIME
#	include <sys/time.h>
#	include <time.h>
#else
#	ifdef HAVE_SYS_TIME_H
#		include <sys/time.h>
#	else
#		include <time.h>
#	endif
#endif


#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
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


#if _MSC_VER
// using this in calls to parent constructors
#pragma warning(disable:4355)   

//#	pragma warning(disable:4065)   // switch statement contains 'default' 
								   // but no 'case' labels
//#	pragma warning(disable:4800)   // (bool)(1&2)

// assignment operator could not be generated
//#pragma warning(disable:4512) 

// identifier was truncated to '255' characters in the debug information
#pragma warning(disable:4786)

// copy constructor could not be generated
#pragma warning(disable:4511)

// assignment operator could not be generated
#pragma warning(disable:4512)

// unreferenced local function has been removed
#pragma warning(disable:4505)

// conditional expression is constant; while(true) OK
#pragma warning(disable:4127)

// assignment within conditional expression if(type var=somethig) OK
#pragma warning(disable:4706)

// memcpy deprecation
#pragma warning(disable:4996)


#if 0
#ifdef _DEBUG
#	define _CRTDBG_MAP_ALLOC
// When the _CRTDBG_MAP_ALLOC flag is defined in the debug version of an application, 
// the base version of the heap functions are directly mapped to their debug versions
#endif
#endif

#endif

#endif
