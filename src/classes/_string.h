/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _string.h,v 1.4 2001/03/11 08:16:31 paf Exp $
*/

#ifndef _STRING_H
#define _STRING_H

#include "pa_vclass.h"

extern VClass *string_class; // global string class [^length[] & co]
void initialize_string_class(Pool& pool, VClass& vclass);

#endif
