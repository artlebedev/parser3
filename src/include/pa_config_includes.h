/** @file
	Parser: includes all Configure-d headers

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_config_includes.h,v 1.18 2002/02/13 13:50:49 paf Exp $


	when used Configure [HAVE_CONFIG_H] it uses defines from Configure,
	fixed otherwise.
*/

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

#ifdef HAVE_SYS_TIME_H
#	include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#	include <time.h>
#endif

#ifdef HAVE_STDLIB_H
#	include <stdlib.h>
#endif

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
