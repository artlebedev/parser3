/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_pool.C,v 1.10 2001/03/10 16:34:39 paf Exp $
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
