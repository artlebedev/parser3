/** @file
	Parser: Configure file for autoconf-disabled platforms.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	tested only with MS Visual C++ V6
*/

#ifndef PA_CONFIG_FIXED_H
#define PA_CONFIG_FIXED_H

static const char* IDENT_CONFIG_FIXED_H="$Date: 2002/09/13 11:08:18 $";

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
#define HAVE_WINSOCK_H
#define HAVE__LOCKING
#define HAVE_PRAGMA_PACK
#define USE_SMTP
#define PA_WITH_SJLJ_EXCEPTIONS

//gmime
#define off_t _off_t
#define ssize_t int

// no string origin tracking
//#define NO_STRING_ORIGIN

// disable any execs (file::exec, file::cgi, unix mail:send)
//#define NO_PA_EXECS

// has $mail:received
#define WITH_MAILRECEIVE

// xml-abled parser
#define XML

#endif
