/*
  $Id: pa_pool.C,v 1.1 2001/01/30 14:57:21 paf Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"

void *Pool::real_malloc(size_t size) {
	return ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
	return ::calloc(size, 1);
}
