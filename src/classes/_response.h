/** @file
	Parser: @b response parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _response.h,v 1.3 2001/04/10 10:32:03 paf Exp $
*/

#ifndef _response_H
#define _response_H

#include "pa_vstateless_class.h"

extern VStateless_class *response_class;
void initialize_response_class(Pool& pool, VStateless_class& vclass);

#endif
