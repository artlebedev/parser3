/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _cookie.h,v 1.1 2001/03/18 20:31:24 paf Exp $
*/

#ifndef _cookie_H
#define _cookie_H

#include "pa_vstateless_class.h"

extern VStateless_class *cookie_base_class;
void initialize_cookie_base_class(Pool& pool, VStateless_class& vclass);

#endif
