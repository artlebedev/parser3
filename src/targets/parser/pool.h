#ifndef POOL_H
#define POOL_H

#include "pa_pool.h"

class parser_Pool : public Pool {
public:
	parser_Pool() : Pool() {}

	void *malloc(size_t size) {
		return check(::malloc(size));
	}

	void *calloc(size_t size) {
		return check(::calloc(size, 1));
	}
};

#endif
