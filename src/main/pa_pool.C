/*
  $Id: pa_pool.C,v 1.9 2001/02/22 11:08:25 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_exception.h"

void Pool::fail(size_t size) const {
	// TODO: if(!fexception) die("Pool::fail with no exception set")
	fexception->_throw(0, 0,
		0,
		"failed to allocate %u bytes", size);
}
