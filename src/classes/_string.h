/** @file
	Parser: @b string parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _string.h,v 1.7 2001/04/10 10:32:03 paf Exp $
*/

#ifndef _STRING_H
#define _STRING_H

#include "pa_vclass.h"

extern VStateless_class *string_class; // global string class [^length[] & co]
void initialize_string_class(Pool& pool, VStateless_class& vclass);

void _string_format(Request& r, const String& method_name, Array *params);

#endif
