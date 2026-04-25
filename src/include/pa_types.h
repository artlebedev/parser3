/** @file
	Parser: generally used types & constants decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#define IDENT_PA_TYPES_H "$Id: pa_types.h,v 1.61 2026/04/25 13:38:46 moko Exp $"

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

#ifndef SSIZE_MAX
#define SSIZE_MAX (sizeof(size_t)==4 ? INT_MAX : LLONG_MAX)
#endif

#endif
