/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.h,v 1.30 2001/03/13 11:15:03 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

class Exception;
class Temp_exception;

class Pool {
	friend Temp_exception;
public:

	Pool() : 
		fexception_to_trhow(0),
		fpending_exception(0) {
	}
	~Pool() {}

	Exception& exception_to_throw() const { return *fexception_to_throw; }
	//Exception *pending_exception() const { return fpending_exception; }

	void *malloc(size_t size) {
		return check(real_malloc(size), size);
	}
	void *calloc(size_t size) {
		return check(real_calloc(size), size);
	}

	void maybe_rethrow_pending_exception() {
		if(fpending_exception)
			fexception_to_throw._throw(
				fpending_exception.type(),
				fpending_exception.code(),
				fpending_exception.problem_source(),
				fpending_exception.comment()
			);
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
		Exception *r=fexception_to_throw;
		fexception_to_throw=e;
		return r;
	}
	void restore_exception(Exception *e) {
		fpending_exception=fexception_to_throw.is_handled()?0:fexception_to_throw;
		fexception_to_throw=e;
	}

private:

	// current request's exception object
	Exception *fexception_to_throw;

	// just catched request's exception object
	Exception *fpending_exception;

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
public:
	bool triggered;
};

#define XTRY(p) \
	{ \
		Exception temp_exception; \
		Temp_exception le(p, temp_exception); \
		if(setjmp(temp_exception.mark)==0)
#define XTHROW(p) p.exception_to_throw()._throw
#define XCATCH(e) \
		else{ \
			Exception& e=temp_exception;
#define XFINALLY(p) \
		} \
		p.maybe_rethrow_pending_exception();
#define XEND_TRY \
	}

// usage:
//   TRY { ...; if(?) RAISE(?); ...; } CATCH(e) { catch-code e.comment() } END_TRY

#define TRY XTRY(pool())
#define THROW XTHROW(pool())
#define CATCH(e) XCATCH(e)
#define FINALLY(p) XFINALLY(pool())
#define END_TRY XEND_TRY

#define PTRY XTRY(pool)
#define PTHROW XTHROW(pool)
#define PCATCH(e) XCATCH(e)
#define PINALLY(p) XFINALLY(pool)
#define PEND_TRY XEND_TRY

#define RTRY XTRY(r.pool())
#define RTHROW XTHROW(r.pool())
#define RCATCH(e) XCATCH(e)
#define RFINALLY(p) XFINALLY(r.pool())
#define REND_TRY XEND_TRY

#endif
