/** @file
	Parser MySQL driver: includes all Configure-d headers

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: config_includes.h,v 1.3 2001/04/26 15:09:07 paf Exp $


	when used Configure [HAVE_CONFIG_H] it uses defines from Configure,
	fixed otherwise.
*/

#if HAVE_CONFIG_H
#	include "config_auto.h"
#else
#	include "config_fixed.h"
#endif

#ifdef HAVE_STRING_H
#	include <string.h>
#endif

#ifdef HAVE_STDIO_H
#	include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#	include <stdlib.h>
#endif
