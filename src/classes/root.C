/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: root.C,v 1.11 2001/03/10 16:58:56 paf Exp $
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
	String& IF_NAME=*new(pool) String(pool);
	IF_NAME.APPEND_CONST("if");

	Method& IF_METHOD=*new(pool) Method(pool,
		IF_NAME,
		2, 3, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _if
	);

	vclass.add_method(IF_NAME, IF_METHOD);
}
