/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: request.C,v 1.2 2001/03/18 14:45:25 paf Exp $
*/

#include "pa_request.h"
#include "_request.h"

// global var

VStateless_class *request_base_class;

// methods

// initialize

void initialize_request_base_class(Pool& pool, VStateless_class& vclass) {
	// TODO: ^browser
}
