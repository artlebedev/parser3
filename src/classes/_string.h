/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: _string.h,v 1.3 2001/03/10 16:34:33 paf Exp $
*/

#ifndef _STRING_H
#define _STRING_H

#include "pa_vclass.h"

extern VClass *string_class; // global string class [^length[] & co]
void initialize_string_class(Pool& pool, VClass& vclass);

#endif
