/*
$Id: root.C,v 1.1 2001/03/08 13:19:09 paf Exp $
*/

#include "pa_request.h"

static void _if(Request& r, Array& params) {
	bool condition=r.autocalc(*static_cast<Value *>(params.get(0))).get_bool();
	Value& value=r.autocalc(*static_cast<Value *>(params.get(condition?1:2)));
	r.write(value);
}

void construct_root_class(Request& request) {
	Pool& pool=request.pool();
	String& IF_NAME=*new(pool) String(pool);
	IF_NAME.APPEND_CONST("if");

	Method& IF_METHOD=*new(pool) Method(pool,
		IF_NAME,
		3/*numbered_params_count*/,
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _if
	);

	request.root_class.add_method(IF_NAME, IF_METHOD);
}
