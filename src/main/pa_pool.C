/** @file
	Parser: pool class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.18 2001/05/17 19:33:33 parser Exp $
*/

#include "pa_pool.h"
#include "pa_exception.h"

void Pool::fail(size_t size) const {
	fexception->_throw(0, 0,
		0,
		"failed to allocate %u bytes", size);
}
