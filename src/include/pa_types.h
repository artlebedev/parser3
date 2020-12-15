/** @file
	Parser: generally used types & constants decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#define IDENT_PA_TYPES_H "$Id: pa_types.h,v 1.55 2020/12/15 17:10:32 moko Exp $"

#include "pa_config_includes.h"

/// for rare cases of undefined length using this-sized strings
#define MAX_STRING 0x400
/// buffer size for parser.log
#define MAX_LOG_STRING 0x400*4
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

#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif

#ifndef INT32_MAX
#define INT32_MAX 2147483647L
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 4294967295UL
#endif

#ifndef SSIZE_MAX
#define SSIZE_MAX (sizeof(size_t)==4 ? INT32_MAX : LLONG_MAX)
#endif

#endif
