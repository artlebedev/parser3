/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: _int.h,v 1.3 2001/03/10 16:34:33 paf Exp $
*/

#ifndef _INT_H
#define _INT_H

#include "pa_vclass.h"

extern VClass *int_class; // global int class [^length[] & co]
void initialize_int_class(Pool& pool, VClass& vclass);

#endif
