/** @file
	Parser: Configure file for autoconf-disabled platforms.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_config_fixed.h,v 1.33 2001/12/13 11:09:46 paf Exp $


	tested only with MS Visual C++ V6
*/

#define inline  __inline
#define HAVE_STDARG_H
#define HAVE_PROCESS_H
#define HAVE_FCNTL_H
#define HAVE_SYS_STAT_H
#define HAVE_IO_H
#define HAVE_STDDEF_H
#define HAVE_STDIO_H
#define HAVE_ERRNO_H
#define HAVE_CTYPE_H
#define HAVE_MATH_H
#define HAVE_TIME_H
#define HAVE_STDLIB_H
#define HAVE_STRING_H
#define HAVE_DIRECT_H
#define HAVE_SETJMP_H
#define HAVE_MEMORY_H
#define HAVE_SYS_LOCKING_H
#define HAVE_SYS_TYPES_H
#define HAVE__LOCKING

#define USE_SMTP
#define XML
#define DB2
#define HAVE_PRAGMA_PACK
