/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _request.h,v 1.1 2001/03/16 12:30:21 paf Exp $
*/

#ifndef _request_H
#define _request_H

#include "pa_vstateless_class.h"

extern VStateless_class *request_base_class;
void initialize_request_base_class(Pool& pool, VStateless_class& vclass);

#endif
