#include <stdlib.h>

#include "pa_pool.h"

Pool::Pool() {
}

Pool::~Pool() {
}

void *Pool::alloc(size_t size) {
	return ::malloc(size);
}

void *Pool::calloc(size_t size) {
	return ::calloc(size, 1);
}
