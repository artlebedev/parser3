/** @file
	Parser: @b unknown parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _unknown.h,v 1.1 2001/04/12 15:02:37 paf Exp $
*/

#ifndef _UNKNOWN_H
#define _UNKNOWN_H

#include "pa_vclass.h"

extern VStateless_class *unknown_class; // global unknown class [^length[] & co]
void initialize_unknown_class(Pool& pool, VStateless_class& vclass);

#endif
