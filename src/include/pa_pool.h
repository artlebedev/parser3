/*
  $Id: pa_pool.h,v 1.5 2001/01/26 18:34:02 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

#include "pa_string.h"
#include "pa_hash.h"

class Pool {
public:
	Pool();
	~Pool();
    void *malloc(size_t size);
    void *calloc(size_t size);

	String *make_string() { 
		return new(this) String(this);
	}
	String *make_string(char *src) {
		return new(this) String(this, src);
	}

	Hash<String, String> *make_hash() {
		return new(this) Hash<String, String>(this);
	}
};

#endif
