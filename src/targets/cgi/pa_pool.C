/** @file
	Parser: CGI memory manager impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.6 2001/05/15 11:36:15 parser Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"

#include "pa_sapi.h"
#include "pa_common.h"
#define MALLOC_STAT_MAXSIZE (0x400*0x400)
#define MALLOC_STAT_PIECES MALLOC_STAT_MAXSIZE
///(10*1000*10*5)
int malloc_size[MALLOC_STAT_PIECES];
int malloc_times[MALLOC_STAT_PIECES];

void log_pool_stats(Pool& pool) {
	for(int i=0; i<MALLOC_STAT_PIECES; i++)
		if(malloc_times[i]) 
			SAPI::log(pool, 
				"%10d..%10d: %10d/%10d=%10d", 
				MALLOC_STAT_MAXSIZE/MALLOC_STAT_PIECES*(i), 
				MALLOC_STAT_MAXSIZE/MALLOC_STAT_PIECES*(1+i),
				malloc_size[i], malloc_times[i], malloc_size[i]/malloc_times[i]);
}

void *Pool::real_malloc(size_t size) {
	int index=min(MALLOC_STAT_PIECES-1, 
		((double)size)/MALLOC_STAT_MAXSIZE*MALLOC_STAT_PIECES);
	malloc_size[index]+=size;
	malloc_times[index]++;
	return ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
	return ::calloc(size, 1);
}
