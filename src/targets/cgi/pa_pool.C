/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.3 2001/03/23 19:25:17 paf Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"

void *Pool::real_malloc(size_t size) {
	return ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
	return ::calloc(size, 1);
}
