/*
  $Id: pa_pool.h,v 1.18 2001/01/30 13:07:31 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"
//#include "pa_table.h"
#include "pa_exception.h"

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

	String& make_string() {
		return *new(*this) String(*this);
	}
	Hash& make_hash() {
		return *new(*this) Hash(*this, false);
	}
	Hash& make_thread_safe_hash() {
		return *new(*this) Hash(*this, true);
	}
	Array& make_array() {
		return *new(*this) Array(*this);
	}
	Array& make_array(int initial_rows) {
		return *new(*this) Array(*this, initial_rows);
	}
	/*Table& make_table(char *afile, uint aline, Array *acolumns, int initial_rows) {
		return *new(this) Table(this, afile, aline, acolumns, initial_rows);
	}*/

protected: // pure virtuals

    virtual void *real_malloc(size_t size)=0;
    virtual void *real_calloc(size_t size)=0;

protected:

	Exception& fexception;

	// checks whether mem allocated OK. throws exception otherwise
	void *check(void *ptr, size_t size) {
		if(!ptr)
			fexception.raise(0, 0,
				0,
				"allocating %ud bytes", size);
		
		return ptr;
	}

private: //disabled

	// Pool(const Pool&) {}
	Pool& operator = (const Pool&) { return *this; }
};

#endif
