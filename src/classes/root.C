/*
$Id: root.C,v 1.6 2001/03/09 08:19:46 paf Exp $
*/

#include "pa_request.h"

static void _if(Request& r, Array *params) {
	bool condition=
		r.autocalc(
			*static_cast<Value *>(params->get(0)), 
			false/*don't make it string*/).get_bool();
	if(condition) {
		Value& value=r.autocalc(*static_cast<Value *>(params->get(1)));
		r.wcontext->write(value);
	} else if(params->size()==3) {
		Value& value=r.autocalc(*static_cast<Value *>(params->get(2)));
		r.wcontext->write(value);
	}
}

void construct_root_class(Request& request) {
	Pool& pool=request.pool();
	String& IF_NAME=*new(pool) String(pool);
	IF_NAME.APPEND_CONST("if");

	Method& IF_METHOD=*new(pool) Method(pool,
		IF_NAME,
		2, 3, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _if
	);

	request.root_class.add_method(IF_NAME, IF_METHOD);
}
