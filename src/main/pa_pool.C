/** @file
	Parser: pool class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_pool.C,v 1.21 2001/09/15 13:20:22 parser Exp $"; 

#include "pa_pool.h"
#include "pa_exception.h"

void Pool::fail_alloc(size_t size) const {
	fexception->_throw(0, 0,
		0,
		"failed to allocate %u bytes", size);
}

void Pool::fail_register_cleanup() const {
	fexception->_throw(0, 0,
		0,
		"failed to register cleanup");
}