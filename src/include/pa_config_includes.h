/** @file
	Parser: includes all Configure-d headers

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_config_includes.h,v 1.5 2001/04/26 15:09:05 paf Exp $


	when used Configure [HAVE_CONFIG_H] it uses defines from Configure,
	fixed otherwise.
*/

#if HAVE_CONFIG_H
#	include "pa_config_auto.h"
#else
#	include "pa_config_fixed.h"
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
