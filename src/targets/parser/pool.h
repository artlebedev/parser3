/*
  $Id: pool.h,v 1.3 2001/01/30 13:07:31 paf Exp $
*/

#ifndef POOL_H
#define POOL_H

#include "pa_pool.h"

class parser_Pool : public Pool {
public:
	parser_Pool(Exception& aexception) : Pool(aexception) {}

	void *real_malloc(size_t size) {
		return ::malloc(size);
	}

	void *real_calloc(size_t size) {
		return ::calloc(size, 1);
	}
};

#endif
