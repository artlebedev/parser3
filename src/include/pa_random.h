/** @file
	Parser: random related functions.

	Copyright (c) 2001-2008 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_RANDOM_H
#define PA_RANDOM_H

static const char * const IDENT_RANDOM_H="$Date: 2009/01/25 01:57:09 $";

#include "pa_types.h"

#define MAX_UINT 0xFFFFFFFFu

void random(void *buffer, size_t size);

static inline int _random(uint top) {
	uint raw;
	random(&raw, sizeof(raw));
	return int(double(raw) / MAX_UINT * top );
}

/// to hell with extra bytes on 64bit platforms
struct uuid {
	unsigned int	time_low;
	unsigned short	time_mid;
	unsigned short	time_hi_and_version;
	unsigned short	clock_seq;
	unsigned char	node[6];
};

uuid get_uuid();

#endif
