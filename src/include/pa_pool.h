/*
  $Id: pa_pool.h,v 1.4 2001/01/26 15:43:11 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

#include "pa_string.h"

class Pool {
public:
	Pool();
	~Pool();
    void *alloc(size_t size);
    void *calloc(size_t size);

	String *makeString() { 
		return new(this) String(this);
	}
	String *makeString(char *src) {
		return new(this) String(this, src);
	}
};

#endif
