/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: _double.h,v 1.3 2001/03/10 16:34:33 paf Exp $
*/

#ifndef _DOUBLE_H
#define _DOUBLE_H

#include "pa_vclass.h"

extern VClass *double_class; // global double class [^length[] & co]
void initialize_double_class(Pool& pool, VClass& vclass);

#endif
