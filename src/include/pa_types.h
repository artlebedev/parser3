/** @file
	Parser: generally used types & constants decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_types.h,v 1.31 2001/11/05 11:46:26 paf Exp $
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#include "pa_config_includes.h"

#if _MSC_VER
// using this in calls to parent constructors
#pragma warning(disable:4355)   

//#	pragma warning(disable:4065)   // switch statement contains 'default' 
								   // but no 'case' labels
//#	pragma warning(disable:4800)   // (bool)(1&2)

// assignment operator could not be generated
//#pragma warning(disable:4512) 

// identifier was truncated to '255' characters in the debug information
#pragma warning(disable:4786)

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

#include "pa_pragma_pack_begin.h"
struct Origin {
	const char *file;  ///< macros file name | load file name | sql query text
	unsigned short line; ///< file line no | record no
};
#include "pa_pragma_pack_end.h"

#endif

#endif
