/*
  $Id: pa_pool.h,v 1.23 2001/02/22 10:43:42 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

//class String;
class Exception;
class Local_exception;

class Pool {
	friend Local_exception;
public:

	// Exception to report pool errors 
	Pool(Exception& aexception) : fexception(&aexception) {}
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
	// raises proper exception
	void fail(size_t size) const;

protected: // exception handling

	// exception replacement mechanism is 'protected' from direct usage
	// Local_exception object enforces paired set/restore
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

class Local_exception {
	Pool pool;
	Exception *saved_exception;
public:
	Local_exception(Pool& apool, Exception& exception) : 
		pool(apool),
		saved_exception(apool.set_exception(&exception)) {
	}
	~Local_exception() { 
		pool.restore_exception(saved_exception); 
	}
};

#define TRY if(setjmp((exception()).mark)==0){
#define THROW exception()._throw
#define CATCH(e) }else{ Exception& e=exception();
#define END_CATCH }
// usage:
//   TRY { ...; if(?) RAISE(?); ...; } CATCH { catch-code EXCEPTION.comment() }

#endif
