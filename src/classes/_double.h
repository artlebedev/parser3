/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _double.h,v 1.4 2001/03/11 08:16:31 paf Exp $
*/

#ifndef _DOUBLE_H
#define _DOUBLE_H

#include "pa_vclass.h"

extern VClass *double_class; // global double class [^length[] & co]
void initialize_double_class(Pool& pool, VClass& vclass);

#endif
