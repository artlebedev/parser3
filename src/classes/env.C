/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: env.C,v 1.7 2001/03/13 17:17:25 paf Exp $
*/

#include "pa_request.h"
#include "_env.h"

// global var

VStateless_class *env_base_class;

// methods

// initialize

void initialize_env_base_class(Pool& pool, VStateless_class& vclass) {
	// TODO: подумать, нужен ли метод "в hash"?
}
