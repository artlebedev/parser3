/** @file
	Parser: @b random parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _random.h,v 1.3 2001/04/17 19:00:26 paf Exp $
*/

#ifndef CLASSES_RANDOM_H
#define CLASSES_RANDOM_H

#include "pa_vclass.h"

extern VStateless_class *random_class; // global random class [^length[] & co]
void initialize_random_class(Pool& pool, VStateless_class& vclass);

#endif
