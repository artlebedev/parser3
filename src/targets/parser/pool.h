/*
  $Id: pool.h,v 1.4 2001/01/30 14:51:56 paf Exp $
*/

#ifndef POOL_H
#define POOL_H

#include "pa_pool.h"

class parser_Pool : public Pool {
public:
	parser_Pool(Exception& aexception) : Pool(aexception) {}

protected:

	void *real_malloc(size_t size) {
		return ::malloc(size);
	}

	void *real_calloc(size_t size) {
		return ::calloc(size, 1);
	}
};

#endif
