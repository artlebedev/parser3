/** @file
	Parser: ISAPI memory manager impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.2 2001/03/23 18:08:54 paf Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"

void *Pool::real_malloc(size_t size) {
	// todo: log
	return ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
	// todo: log
	return ::calloc(size, 1);
}

Pool::~Pool() {
	// todo: free 
}