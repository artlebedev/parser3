/** @file
	Parser: pool class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.h,v 1.36 2001/03/19 17:42:12 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

class Exception;
class Temp_exception;

/** 
	Pool mechanizm allows users not to free up allocated memory,
	leaving that problem to 'pools'.

	Also holds Exception object, which can be temporary set using 
	Temp_exception auto-object.
*/

class Pool {
	friend Temp_exception;
public:

	Pool() : fexception(0) {}
	~Pool() {}

	/// current exception object of the pool
	Exception& exception() const { return *fexception; }

	/// allocates some bytes on pool
	void *malloc(size_t size) {
		return check(real_malloc(size), size);
	}
	/// allocates some bytes clearing them with zeros
	void *calloc(size_t size) {
		return check(real_calloc(size), size);
	}

private: // implementation defined

    void *real_malloc(size_t size);
    void *real_calloc(size_t size);

private: 

	/// checks whether mem allocated OK. throws exception otherwise
	void *check(void *ptr, size_t size) {
		if(ptr)
			return ptr;

		fail(size);

		// never reached
		return 0;
	}
	/// throws proper exception
	void fail(size_t size) const;

private: // exception handling

	// exception replacement mechanism is 'protected' from direct usage
	// Temp_exception object enforces paired set/restore
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

/** 
	Base for all classes that are allocated in 'pools'.

	Holds Pool object. Contains useful wrappers to it's methods.
*/
class Pooled {
	// the pool i'm allocated on
	Pool& fpool;
public:
	
	static void *operator new(size_t size, Pool& apool) { 
		return apool.malloc(size);
	}

	Pooled(Pool& apool) : fpool(apool) {
	}

	/// my pool
	Pool& pool() const { return fpool; }

	/// useful wrapper around pool
	void *malloc(size_t size) const { return fpool.malloc(size); }
	/// useful wrapper around pool
	void *calloc(size_t size) const { return fpool.calloc(size); }
	/// useful wrapper around pool
	Exception& exception() const { return fpool.exception(); }
};
/// useful macro for creating objects on current Pooled object Pooled::pool()
#define NEW new(pool())

/** 
	Auto-object used for temporary changing Pool's exception().

	Use by with these macros:
	\code
		TRY { 
			... 
			if(?) 
				THROW(?); 
			...;
		} CATCH(e) { 
			code, using e fields
			e.comment() 
		}
		END_CATCH
	\endcode
*/
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

//@{
/// @see Temp_exception 
#define TRY XTRY(pool())
#define THROW XTHROW(exception())
#define CATCH(e) XCATCH(e)
#define END_CATCH XEND_CATCH 

#define PTRY XTRY(pool)
#define PTHROW XTHROW(pool.exception())
#define PCATCH(e) XCATCH(e)
#define PEND_CATCH XEND_CATCH 

#define RTHROW XTHROW(r.pool().exception())
//@}

#endif
