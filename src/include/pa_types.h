/** @file
	Parser: generally used types & constants decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_types.h,v 1.39 2002/02/21 14:36:54 paf Exp $
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

#ifdef _DEBUG
#	define _CRTDBG_MAP_ALLOC
// When the _CRTDBG_MAP_ALLOC flag is defined in the debug version of an application, 
// the base version of the heap functions are directly mapped to their debug versions
#endif

#endif

/// for rare cases of undefined length using this-sized strings
#define MAX_STRING 0x400
/// for snprintf(buf, MAX_NUMBER, "%.2f")
#define MAX_NUMBER 40

///{@ handy types
#undef uchar
typedef unsigned char uchar;

#undef ushort
typedef unsigned short ushort;

#undef uint
typedef unsigned int uint;

#undef ulong
typedef unsigned long ulong;
///}@

/// max value of integral type
#define max_integral(type) ((1<<sizeof(type)*8)-1)


#ifndef NO_STRING_ORIGIN
/// all String pieces hold information of where they come from

#include "pa_pragma_pack_begin.h"
struct Origin {
	const char *file;  ///< macros file name | load file name | sql query text
	unsigned short line; ///< file line no | record no
};
#include "pa_pragma_pack_end.h"
#define ORIGIN_FILE_LINE_FORMAT "%.300s(%d)"

/** helper used from body from STRING_FOREACH_ROW 
	to make preprocessor do if before macro expansion, 
	not after[which is impossible and caused errors]
*/
#define IFNDEF_NO_STRING_ORIGIN(body) body

#else
#define IFNDEF_NO_STRING_ORIGIN(body)
#endif

#endif
