/*
  $Id: pa_pool.h,v 1.22 2001/02/20 18:45:51 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

class String;
class Exception;

class Pool {
public:

	// Exception to report pool errors 
	Pool(Exception& aexception) : fexception(aexception) {}
	~Pool() {}

	Exception& exception() { return fexception; }

	void *malloc(size_t size) {
		return check(real_malloc(size), size);
	}
	void *calloc(size_t size) {
		return check(real_calloc(size), size);
	}

protected: // implementation defined

    void *real_malloc(size_t size);
    void *real_calloc(size_t size);

protected:

	Exception& fexception;

	// checks whether mem allocated OK. throws exception otherwise
	void *check(void *ptr, size_t size);

private: //disabled

	// Pool(const Pool&) {}
	Pool& operator = (const Pool&) { return *this; }
};

class Pooled {
public:
	static void *operator new(size_t size, Pool& apool) { 
		return apool.malloc(size);
	}

	Pooled(Pool& apool) : fpool(apool) {}
	Pool& pool() const { return fpool; }

protected:
	// the pool I'm allocated on
	Pool& fpool;
};

#endif
