/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _table.h,v 1.1 2001/03/12 13:40:01 paf Exp $
*/

#ifndef _TABLE_H
#define _TABLE_H

#include "pa_vclass.h"

extern VClass *table_class; // global table class [^length[] & co]
void initialize_table_class(Pool& pool, VClass& vclass);

#endif
