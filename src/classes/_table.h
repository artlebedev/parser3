/** @file
	Parser: @b table parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _table.h,v 1.5 2001/04/10 10:32:04 paf Exp $
*/

#ifndef _TABLE_H
#define _TABLE_H

#include "pa_vclass.h"

extern VStateless_class *table_class; // global table class [^length[] & co]
void initialize_table_class(Pool& pool, VStateless_class& vclass);

#endif
