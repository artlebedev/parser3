/*
  $Id: pa_pool.h,v 1.17 2001/01/30 11:51:07 paf Exp $
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
	Pool();
	~Pool();
    virtual void *malloc(size_t size)=0;
    virtual void *calloc(size_t size)=0;

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

	Exception *global_exception() { return fglobal_exception; }
	Exception *set_global_exception(Exception *e);
	void restore_global_exception(Exception *e);

	Exception *local_exception() { return flocal_exception; }
	Exception *set_local_exception(Exception *e);
	void restore_local_exception(Exception *e);

protected:

	Exception *fglobal_exception;
	Exception *flocal_exception;

	// checks whether mem allocated OK. throws exceptions
	void *check(void *ptr);

private: //disabled

	Pool(const Pool&) {}
	Pool& operator = (const Pool&) { return *this; }
};

#endif
