/** @file
	Parser: @b double parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _double.h,v 1.7 2001/04/17 19:00:25 paf Exp $
*/

#ifndef CLASSES_DOUBLE_H
#define CLASSES_DOUBLE_H

#include "pa_vclass.h"

extern VStateless_class *double_class; // global double class [^length[] & co]
void initialize_double_class(Pool& pool, VStateless_class& vclass);

#endif
