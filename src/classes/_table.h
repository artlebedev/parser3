/** @file
	Parser: table parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _table.h,v 1.4 2001/03/19 22:11:07 paf Exp $
*/

#ifndef _TABLE_H
#define _TABLE_H

#include "pa_vclass.h"

extern VStateless_class *table_class; // global table class [^length[] & co]
void initialize_table_class(Pool& pool, VStateless_class& vclass);

#endif
