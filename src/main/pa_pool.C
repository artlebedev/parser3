/*
  $Id: pa_pool.C,v 1.8 2001/02/22 10:43:46 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_exception.h"

void Pool::fail(size_t size) const {
	fexception->_throw(0, 0,
		0,
		"Pool::_alloc(%u) returned NULL", size);
}
