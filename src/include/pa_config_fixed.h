/** @file
	Parser: Configure file for autoconf-disabled platforms.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_CONFIG_FIXED_H
#define PA_CONFIG_FIXED_H

#define IDENT_PA_CONFIG_FIXED_H "$Id: pa_config_fixed.h,v 1.79 2013/03/10 19:44:44 moko Exp $"

#define inline  __inline
#define HAVE_ASSERT_H
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
#define HAVE_STRING_H
#define HAVE_DIRECT_H
#define HAVE_SETJMP_H
#define HAVE_MEMORY_H
#define HAVE_LIMITS_H
#define HAVE_SYS_LOCKING_H
#define HAVE_SYS_TYPES_H
#define HAVE_WINSOCK_H
#define HAVE__LOCKING
#define HAVE_TIMEZONE
#define HAVE_DAYLIGHT
#define USE_SMTP
#define PA_LITTLE_ENDIAN
#define PA_WITH_SJLJ_EXCEPTIONS

//types

#ifndef ssize_t
typedef int ssize_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

// disable any execs (file::exec, file::cgi, unix mail:send)
//#define NO_PA_EXECS

// disable stringstream usage
//#define NO_STRINGSTREAM

// has $mail:received
//#define WITH_MAILRECEIVE

//xml-abled parser
#define XML
//#define XML_STATIC
#ifdef XML_STATIC
#	define LIBXML_STATIC
#	define LIBXSLT_STATIC
#	define LIBEXSLT_STATIC
#endif

// otherwise functions in pcre.h will be declared as __declspec(dllimport)
#define PCRE_STATIC

#endif
