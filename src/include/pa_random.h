/** @file
	Parser: random related functions.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_RANDOM_H
#define PA_RANDOM_H

#define IDENT_PA_RANDOM_H "$Id: pa_random.h,v 1.13 2024/11/04 03:53:25 moko Exp $"

#include "pa_types.h"

void random(void *buffer, size_t size);

static inline int _random(int top) {
	uint32_t raw;
	random(&raw, sizeof(raw));
	return int( double(raw) / ((double)UINT_MAX+1) * top );
}

char *get_uuid_cstr(bool lower, bool solid);
char *get_uuid7_cstr(bool lower, bool solid);
char *get_uuid_boundary();

#endif
