/** @file
	Parser: random related functions.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_RANDOM_H
#define PA_RANDOM_H

#define IDENT_PA_RANDOM_H "$Id: pa_random.h,v 1.5 2016/07/21 17:05:37 moko Exp $"

#include "pa_types.h"

#define MAX_UINT 0xFFFFFFFFu

void random(void *buffer, size_t size);

static inline int _random(uint top) {
	uint raw;
	random(&raw, sizeof(raw));
	return int(double(raw) / MAX_UINT * top );
}

char *get_uuid_cstr();
char *get_uuid_boundary();

#endif
