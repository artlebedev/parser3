/** @file
	Parser: generally used types & constants decls.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

static const char* IDENT_TYPES_H="$Date: 2003/07/24 11:31:22 $";

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

//{@ handy types
#undef uchar
typedef unsigned char uchar;

#undef ushort
typedef unsigned short ushort;

#undef uint
typedef unsigned int uint;

#undef ulong
typedef unsigned long ulong;
//}@

/// max value of integral type
#define max_integral(type) ((1<<sizeof(type)*8)-1)

#endif
