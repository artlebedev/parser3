/*
  $Id: pa_pool.h,v 1.10 2001/01/29 12:13:14 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"

class Pool {
public:
	Pool();
	~Pool();
    void *malloc(size_t size);
    void *calloc(size_t size);

	String& make_string() { 
		return *new(this) String(this);
	}
	String& make_string(char *src) {
		return *new(this) String(this, src);
	}
	Hash& make_hash() {
		return *new(this) Hash(this, false);
	}
	Hash& make_thread_safe_hash() {
		return *new(this) Hash(this, true);
	}
	Array& make_array() {
		return *new(this) Array(this);
	}
	Array& make_array(int initial_rows) {
		return *new(this) Array(this, initial_rows);
	}

private: //disabled

	Pool& operator = (Pool& src) { return *this; }
	Pool(Pool& src) {}
};

#endif
