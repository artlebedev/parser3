/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _response.h,v 1.1 2001/03/18 11:37:46 paf Exp $
*/

#ifndef _response_H
#define _response_H

#include "pa_vstateless_class.h"

extern VStateless_class *response_base_class;
void initialize_response_base_class(Pool& pool, VStateless_class& vclass);

#endif
