/** @file
	Parser: pool class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_pool.C,v 1.20 2001/06/28 07:44:17 parser Exp $"; 

#include "pa_pool.h"
#include "pa_exception.h"

void Pool::fail(size_t size) const {
	fexception->_throw(0, 0,
		0,
		"failed to allocate %u bytes", size);
}
