/** @file
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_types.h,v 1.17 2001/03/19 15:29:38 paf Exp $
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif

#ifdef WIN32
#	if _MSC_VER
#		pragma warning(disable:4355)   // disable C4355 warning about using this
		                               // in calls to parent constructors
#	endif
#endif

#ifdef WIN32
/// path delimiter char, depens on OS
#	define PATH_DELIMITER_CHAR '\\'
/// path delimiter string, depens on OS
#	define PATH_DELIMITER_STRING "\\"
#else
/// path delimiter char, depens on OS
#	define PATH_DELIMITER_CHAR '/'
/// path delimiter char, depens on OS
#	define PATH_DELIMITER_STRING "/"
#endif


/// for rare cases of undefined length using this-sized strings
#define MAX_STRING 0x400
/// for snprintf(buf, MAX_NUMBER, "%.2f")
#define MAX_NUMBER 40

#undef uint
/// handy type
typedef unsigned int uint;

#ifndef NO_STRING_ORIGIN
/// all String pieces hold information of where they come from
struct Origin {
	const char *file;  ///< macros file name | load file name | sql query text
	uint line; ///< file line no | record no
};
#endif

#endif
