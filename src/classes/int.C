/*
$Id: int.C,v 1.1 2001/03/09 08:19:46 paf Exp $
*/

#include "pa_request.h"
#include "_int.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// global var

VClass *int_class;

// methods

static void _int(Request& r, Array *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VInt(pool, vint->get_int());
	r.wcontext->write(value);
}

static void _double(Request& r, Array *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VDouble(pool, vint->get_double());
	r.wcontext->write(value);
}

static void _inc(Request& r, Array *params) {
	VInt *vint=static_cast<VInt *>(r.self);
	int increment=params->size()?
		static_cast<int>(static_cast<Value *>(params->get(0))->get_double()):1;
	vint->inc(increment);
}

void initialize_int_class(Pool& pool) {
	int_class=new(pool) VClass(pool);

	// ^int.int[]
	String& INT_NAME=*new(pool) String(pool);
	INT_NAME.APPEND_CONST("int");

	Method& INT_METHOD=*new(pool) Method(pool,
		INT_NAME,
		0, 0, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _int
	);
	int_class->add_method(INT_NAME, INT_METHOD);

	// ^int.int[]
	String& DOUBLE_NAME=*new(pool) String(pool);
	DOUBLE_NAME.APPEND_CONST("int");

	Method& DOUBLE_METHOD=*new(pool) Method(pool,
		DOUBLE_NAME,
		0, 0, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _int
	);
	int_class->add_method(DOUBLE_NAME, DOUBLE_METHOD);

	// ^int.inc[] ^int.inc[offset]
	String& INC_NAME=*new(pool) String(pool);
	INC_NAME.APPEND_CONST("inc");

	Method& INC_METHOD=*new(pool) Method(pool,
		INC_NAME,
		0, 1, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _inc
	);
	int_class->add_method(INC_NAME, INC_METHOD);
}

