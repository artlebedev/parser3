/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _double.h,v 1.5 2001/03/13 13:43:27 paf Exp $
*/

#ifndef _DOUBLE_H
#define _DOUBLE_H

#include "pa_vclass.h"

extern VStateless_class *double_class; // global double class [^length[] & co]
void initialize_double_class(Pool& pool, VStateless_class& vclass);

#endif
