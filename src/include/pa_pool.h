/*
  $Id: pa_pool.h,v 1.16 2001/01/29 20:46:22 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"
//#include "pa_table.h"

class Pool {
public:
	Pool();
	~Pool();
    void *malloc(size_t size);
    void *calloc(size_t size);

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

private: //disabled

	Pool(Pool&) {}
	Pool& operator = (const Pool&) { return *this; }
};

#endif
