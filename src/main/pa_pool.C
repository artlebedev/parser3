/*
  $Id: pa_pool.C,v 1.7 2001/02/11 11:27:25 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_exception.h"

void *Pool::check(void *ptr, size_t size) {
	if(!ptr)
		fexception.raise(0, 0,
		0,
		"Pool::_alloc(%u) returned NULL", size);
	
	return ptr;
}
