/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_pool.C,v 1.2 2001/03/10 16:34:41 paf Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"

void *Pool::real_malloc(size_t size) {
	return ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
	return ::calloc(size, 1);
}
