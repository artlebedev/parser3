/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _request.h,v 1.3 2001/03/19 22:56:42 paf Exp $
*/

#ifndef _request_H
#define _request_H

#include "pa_vstateless_class.h"

extern VStateless_class *request_class;

void initialize_request_class(Pool& pool, VStateless_class& vclass);

#endif
