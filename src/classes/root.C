/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: root.C,v 1.13 2001/03/11 08:44:39 paf Exp $
*/

#include "pa_request.h"
#include "_root.h"

static void _if(Request& r, Array *params) {
	bool condition=
		r.autocalc(
			*static_cast<Value *>(params->get(0)), 
			0/*no name*/,
			false/*don't make it string*/).get_bool();
	if(condition) {
		Value& value=r.autocalc(*static_cast<Value *>(params->get(1)));
		r.wcontext->write(value, String::Untaint_lang::PASS_APPENDED);
	} else if(params->size()==3) {
		Value& value=r.autocalc(*static_cast<Value *>(params->get(2)));
		r.wcontext->write(value, String::Untaint_lang::PASS_APPENDED);
	}
}

void initialize_root_class(Pool& pool, VClass& vclass) {
	vclass.add_native_method("if", _if, 2, 3); // ^if(c){t} ^if(c){t}{f}
}
