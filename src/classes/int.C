/** @file
	Parser: @b int parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "pa_vbool.h"

volatile const char * IDENT_INT_C="$Id: int.C,v 1.70 2020/12/15 17:10:28 moko Exp $" IDENT_PA_VINT_H;

// externs

void _string_format(Request& r, MethodParams&);

// class

class MInt: public Methoded {
public:
	MInt();
};

// global variable

DECLARE_CLASS_VAR(int, new MInt);

// methods

static void _int(Request& r, MethodParams&) {
	VInt& vint=GET_SELF(r, VInt);
	r.write(*new VInt(vint.get_int()));
}

static void _double(Request& r, MethodParams&) {
	VInt& vint=GET_SELF(r, VInt);
	r.write(*new VDouble(vint.as_double()));
}

static void _bool(Request& r, MethodParams&) {
	VInt& vint=GET_SELF(r, VInt);
	r.write(VBool::get(vint.as_bool()));
}

typedef void (*vint_op_func_ptr)(VInt& vint, double param);

static void __inc(VInt& vint, double param) { vint.inc((int)param); }
static void __dec(VInt& vint, double param) { vint.inc((int)-param); }
static void __mul(VInt& vint, double param) { vint.mul(param); }
static void __div(VInt& vint, double param) { vint.div(param); }
static void __mod(VInt& vint, double param) { vint.mod((int)param); }

static void vint_op(Request& r, MethodParams& params, vint_op_func_ptr func) {
	VInt& vint=GET_SELF(r, VInt);
	double param=params.count()?params.as_double(0, "param must be numerical", r):1;
	(*func)(vint, param);
}

static void _inc(Request& r, MethodParams& params) { vint_op(r, params, &__inc); }
static void _dec(Request& r, MethodParams& params) { vint_op(r, params, &__dec); }
static void _mul(Request& r, MethodParams& params) { vint_op(r, params, &__mul); }
static void _div(Request& r, MethodParams& params) { vint_op(r, params, &__div); }
static void _mod(Request& r, MethodParams& params) { vint_op(r, params, &__mod); }

// from string.C
extern const String* sql_result_string(Request& r, MethodParams& params, Value*& default_code);

static void _sql(Request& r, MethodParams& params) {
	int val;
	Value* default_code=0;
	if(const String* string=sql_result_string(r, params, default_code))
		val=string->as_int();
	else
		if(default_code)
			val=r.process(*default_code).as_int();
		else {
			throw Exception(PARSER_RUNTIME, 0, "produced no result, but no default option specified");
		}
	r.write(*new VInt(val));
}

// constructor

MInt::MInt(): Methoded("int") {
	// ^int.int[]
	// ^int.int[default for ^string.int compatibility]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);
	// ^int.double[]
	// ^int.double[default for ^string.double compatibility]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);
	// ^int.bool[]
	// ^int.bool[default for ^string.bool compatibility]
	add_native_method("bool", Method::CT_DYNAMIC, _bool, 0, 1);

	// ^int.inc[] 
	// ^int.inc[offset]
	add_native_method("inc", Method::CT_DYNAMIC, _inc, 0, 1);
	// ^int.dec[] 
	// ^int.dec[offset]
	add_native_method("dec", Method::CT_DYNAMIC, _dec, 0, 1);
	// ^int.mul[k] 
	add_native_method("mul", Method::CT_DYNAMIC, _mul, 1, 1);
	// ^int.div[d]
	add_native_method("div", Method::CT_DYNAMIC, _div, 1, 1);
	// ^int.mod[offset]
	add_native_method("mod", Method::CT_DYNAMIC, _mod, 1, 1);


	// ^int.format{format}
	add_native_method("format", Method::CT_DYNAMIC, _string_format, 1, 1);

	// ^sql[query]
	// ^sql[query][$.limit(1) $.offset(2) $.default(0)]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);
}
