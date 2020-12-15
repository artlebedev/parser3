/** @file
	Parser: @b double parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "pa_vbool.h"

volatile const char * IDENT_DOUBLE_C="$Id: double.C,v 1.74 2020/12/15 17:10:27 moko Exp $" IDENT_PA_VDOUBLE_H;

// externs

void _string_format(Request& r, MethodParams&);

// class

class MDouble: public Methoded {
public:
	MDouble();
};

// global variable

DECLARE_CLASS_VAR(double, new MDouble);

// methods

static void _int(Request& r, MethodParams&) {
	VDouble& vdouble=GET_SELF(r, VDouble);
	r.write(*new VInt(vdouble.as_int()));
}

static void _double(Request& r, MethodParams&) {
	VDouble& vdouble=GET_SELF(r, VDouble);
	r.write(*new VDouble(vdouble.as_double()));
}

static void _bool(Request& r, MethodParams&) {
	VDouble& vdouble=GET_SELF(r, VDouble);
	r.write(VBool::get(vdouble.as_bool()));
}

typedef void (*vdouble_op_func_ptr)(VDouble& vdouble, double param);

static void __inc(VDouble& vdouble, double param) { vdouble.inc(param); }
static void __dec(VDouble& vdouble, double param) { vdouble.inc(-param); }
static void __mul(VDouble& vdouble, double param) { vdouble.mul(param); }
static void __div(VDouble& vdouble, double param) { vdouble.div(param); }
static void __mod(VDouble& vdouble, double param) { vdouble.mod((int)param); }

static void vdouble_op(Request& r, MethodParams& params, vdouble_op_func_ptr func) {
	VDouble& vdouble=GET_SELF(r, VDouble);
	double param=params.count() ? params.as_double(0, "param must be double", r) : 1/*used in inc/dec*/;
	(*func)(vdouble, param);
}

static void _inc(Request& r, MethodParams& params) { vdouble_op(r, params, &__inc); }
static void _dec(Request& r, MethodParams& params) { vdouble_op(r, params, &__dec); }
static void _mul(Request& r, MethodParams& params) { vdouble_op(r, params, &__mul); }
static void _div(Request& r, MethodParams& params) { vdouble_op(r, params, &__div); }
static void _mod(Request& r, MethodParams& params) { vdouble_op(r, params, &__mod); }

// from string.C
extern const String* sql_result_string(Request& r, MethodParams& params, Value*& default_code);

static void _sql(Request& r, MethodParams& params) {
	double val;
	Value* default_code;
	if(const String* string=sql_result_string(r, params, default_code))
		val=string->as_double();
	else
		if(default_code)
			val=r.process(*default_code).as_double();
		else {
			throw Exception(PARSER_RUNTIME, 0, "produced no result, but no default option specified");
		}
	r.write(*new VDouble(val));
}

// constructor

MDouble::MDouble(): Methoded("double") {
	// ^double.int[]
	// ^double.int[default for ^string.int compatibility]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);
	// ^double.double[]
	// ^double.double[default for ^string.double compatibility]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);
	// ^double.bool[]
	// ^double.bool[default for ^string.bool compatibility]
	add_native_method("bool", Method::CT_DYNAMIC, _bool, 0, 1);
	
	// ^double.inc[] 
	// ^double.inc[offset]
	add_native_method("inc", Method::CT_DYNAMIC, _inc, 0, 1);
	// ^double.dec[] 
	// ^double.dec[offset]
	add_native_method("dec", Method::CT_DYNAMIC, _dec, 0, 1);
	// ^double.mul[k] 
	add_native_method("mul", Method::CT_DYNAMIC, _mul, 1, 1);
	// ^double.div[d]
	add_native_method("div", Method::CT_DYNAMIC, _div, 1, 1);
	// ^double.mod[offset]
	add_native_method("mod", Method::CT_DYNAMIC, _mod, 1, 1);

	// ^double.format{format}
	add_native_method("format", Method::CT_DYNAMIC, _string_format, 1, 1);
	
	// ^sql[query]
	// ^sql[query][$.limit(1) $.offset(2) $.default(0.0)]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);
}
