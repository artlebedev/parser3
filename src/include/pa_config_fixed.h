/** @file
	Parser: Configure file for autoconf-disabled platforms.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_config_fixed.h,v 1.40 2002/03/11 07:53:16 paf Exp $


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
#define HAVE_PRAGMA_PACK
#define USE_SMTP

// no string origin tracking
//#define NO_STRING_ORIGIN

// disable any execs (file::exec, file::cgi, unix mail:send)
//#define NO_PA_EXECS

// xml-abled parser
#define XML
