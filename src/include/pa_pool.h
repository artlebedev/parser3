/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.h,v 1.33 2001/03/14 08:50:01 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

class Exception;
class Temp_exception;

class Pool {
	friend Temp_exception;
public:

	Pool() : fexception(0) {}
	~Pool() {}

	Exception& exception() const { return *fexception; }

	void *malloc(size_t size) {
		return check(real_malloc(size), size);
	}
	void *calloc(size_t size) {
		return check(real_calloc(size), size);
	}

private: // implementation defined

    void *real_malloc(size_t size);
    void *real_calloc(size_t size);

private: 

	// checks whether mem allocated OK. throws exception otherwise
	void *check(void *ptr, size_t size) {
		if(ptr)
			return ptr;

		fail(size);

		// never reached
		return 0;
	}
	// throws proper exception
	void fail(size_t size) const;

protected: // exception handling

	// exception replacement mechanism is 'protected' from direct usage
	// Temp_exception_change object enforces paired set/restore
	Exception *set_exception(Exception *e){
		Exception *r=fexception;
		fexception=e;
		return r;
	}
	void restore_exception(Exception *e) {
		fexception=e;
	}

private:

	// current request's exception object
	Exception *fexception;

private: //disabled

	// Pool(const Pool&) {}
	Pool& operator = (const Pool&) { return *this; }
};

class Pooled {
	// the pool i'm allocated on
	Pool& fpool;
public:
	
	static void *operator new(size_t size, Pool& apool) { 
		return apool.malloc(size);
	}

	Pooled(Pool& apool) : fpool(apool) {
	}

	Pool& pool() const { return fpool; }

	void *malloc(size_t size) const { return fpool.malloc(size); }
	void *calloc(size_t size) const { return fpool.calloc(size); }
	Exception& exception() const { return fpool.exception(); }
};
#define NEW new(pool())

class Temp_exception {
	Pool& fpool;
	Exception *saved_exception;
public:
	Temp_exception(Pool& apool, Exception& exception) : 
		fpool(apool),
		saved_exception(apool.set_exception(&exception)) {
	}
	~Temp_exception() { 
		fpool.restore_exception(saved_exception); 
	}
};

#define XTRY(pool) \
	{ \
		Exception temp_exception; \
		Temp_exception le(pool, temp_exception); \
		if(setjmp(temp_exception.mark)==0)

#define XTHROW(exception) exception._throw
#define XCATCH(e) \
		else{ \
			Exception& e=temp_exception;

#define XEND_CATCH \
		} \
	}
// usage:
//   TRY { ...; if(?) RAISE(?); ...; } CATCH(e) { catch-code e.comment() } END_CATCH

#define TRY XTRY(pool())
#define THROW XTHROW(exception())
#define CATCH(e) XCATCH(e)
#define END_CATCH XEND_CATCH 

#define PTRY XTRY(pool)
#define PTHROW XTHROW(pool.exception())
#define PCATCH(e) XCATCH(e)
#define PEND_CATCH XEND_CATCH 

#define RTHROW XTHROW(r.pool().exception())

#endif
