/** @file
	Parser: generally used types & constants decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_types.h,v 1.24 2001/04/05 11:01:55 paf Exp $
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#include "pa_config_includes.h"

#if _MSC_VER
#	pragma warning(disable:4355)   // disable C4355 warning about using this
	                               // in calls to parent constructors
#	pragma warning(disable:4065)   // switch statement contains 'default' 
								   // but no 'case' labels
#endif

/// for rare cases of undefined length using this-sized strings
#define MAX_STRING 0x400
/// for snprintf(buf, MAX_NUMBER, "%.2f")
#define MAX_NUMBER 40

#undef uint
/// handy type
typedef unsigned int uint;
#undef ulong
/// handy type
typedef unsigned long ulong;

#ifndef NO_STRING_ORIGIN
/// all String pieces hold information of where they come from
struct Origin {
	const char *file;  ///< macros file name | load file name | sql query text
	uint line; ///< file line no | record no
};
#endif

#endif
