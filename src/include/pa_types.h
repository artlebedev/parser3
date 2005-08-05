/** @file
	Parser: generally used types & constants decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

static const char * const IDENT_TYPES_H="$Date: 2005/08/05 13:03:00 $";

#include "pa_config_includes.h"

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
