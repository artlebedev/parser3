/*
  $Id: pa_pool.C,v 1.3 2001/01/26 18:34:02 paf Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"

Pool::Pool() {
}

Pool::~Pool() {
}

void *Pool::malloc(size_t size) {
	return ::malloc(size);
}

void *Pool::calloc(size_t size) {
	return ::calloc(size, 1);
}
