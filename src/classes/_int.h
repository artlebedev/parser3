/** @file
	Parser: @b int parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _int.h,v 1.7 2001/04/17 19:00:26 paf Exp $
*/

#ifndef CLASSES_INT_H
#define CLASSES_INT_H

#include "pa_vclass.h"

extern VStateless_class *int_class; // global int class [^length[] & co]
void initialize_int_class(Pool& pool, VStateless_class& vclass);

#endif
