/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: form.C,v 1.2 2001/03/13 20:02:08 paf Exp $
*/

#include "pa_request.h"
#include "_form.h"
#include "pa_vform.h"

// global var

VStateless_class *form_base_class;

// methods

// initialize

void initialize_form_base_class(Pool& pool, VStateless_class& vclass) {
}
