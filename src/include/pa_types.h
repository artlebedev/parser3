/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_types.h,v 1.13 2001/03/14 08:50:01 paf Exp $
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif

#ifdef WIN32
#define open _open
#define close _close
#define read _read
#define stat _stat


#if _MSC_VER
#pragma warning(disable:4355)   // disable C4355 warning about using this
	                            // in calls to parent constructors
#endif


#endif

// for now: used in Exception::fcomment
#define MAX_STRING 5*0x400
#define MAX_NUMBER 40

#define STATIC_NAME "STATIC"

#undef uint
typedef unsigned int uint;

#ifndef NO_STRING_ORIGIN
struct Origin {
	const char *file;  // macros file name | load file name | sql query text
	uint line; // file line no | record no
};
#endif

#endif
