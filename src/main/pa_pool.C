/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.12 2001/03/19 15:29:39 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_exception.h"

/// \todo if(!fexception) die("Pool::fail with no exception set")
void Pool::fail(size_t size) const {
	fexception->_throw(0, 0,
		0,
		"failed to allocate %u bytes", size);
}
