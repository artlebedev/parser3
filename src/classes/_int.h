/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _int.h,v 1.5 2001/03/13 13:43:27 paf Exp $
*/

#ifndef _INT_H
#define _INT_H

#include "pa_vclass.h"

extern VStateless_class *int_class; // global int class [^length[] & co]
void initialize_int_class(Pool& pool, VStateless_class& vclass);

#endif
