/** @file
	Parser: @b double parser class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_DOUBLE_C="$Date: 2004/02/11 15:33:12 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// externs

void _string_format(Request& r, MethodParams&);

// class

class MDouble: public Methoded {
public:
	MDouble();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(double, new MDouble, 0);

// methods

static void _int(Request& r, MethodParams& params) {
	// just checking (default) syntax validity, never really using it  here, just for string.int compatibility
	if(params.count()>0)
		params.as_junction(0, "default must be int");

	VDouble& vdouble=GET_SELF(r, VDouble);
	r.write_no_lang(*new VInt(vdouble.as_int()));
}

static void _double(Request& r, MethodParams& params) {
	// just checking (default) syntax validity, never really using it  here, just for string.doube compatibility
	if(params.count()>0)
		params.as_junction(0, "default must be double");

	VDouble& vdouble=GET_SELF(r, VDouble);
	r.write_no_lang(*new VDouble(vdouble.as_double()));
}

typedef void (*vdouble_op_func_ptr)(VDouble& vdouble, double param);

static void __inc(VDouble& vdouble, double param) { vdouble.inc(param); }
static void __dec(VDouble& vdouble, double param) { vdouble.inc(-param); }
static void __mul(VDouble& vdouble, double param) { vdouble.mul(param); }
static void __div(VDouble& vdouble, double param) { vdouble.div(param); }
static void __mod(VDouble& vdouble, double param) { vdouble.mod((int)param); }

static void vdouble_op(Request& r, MethodParams& params, 
					   vdouble_op_func_ptr func) {
	VDouble& vdouble=GET_SELF(r, VDouble);
	double param=params.count()?
			params.as_double(0, "param must be double", r):1/*used in inc/dec*/;
	(*func)(vdouble, param);
}

static void _inc(Request& r, MethodParams& params) { vdouble_op(r, params, &__inc); }
static void _dec(Request& r, MethodParams& params) { vdouble_op(r, params, &__dec); }
static void _mul(Request& r, MethodParams& params) { vdouble_op(r, params, &__mul); }
static void _div(Request& r, MethodParams& params) { vdouble_op(r, params, &__div); }
static void _mod(Request& r, MethodParams& params) { vdouble_op(r, params, &__mod); }

// from string.C
extern 
const String* sql_result_string(Request& r, MethodParams& params,
				HashStringValue*& options, Value*& default_code);

static void _sql(Request& r, MethodParams& params) {
	double val;
	HashStringValue* options;
	Value* default_code;
	if(const String* string=sql_result_string(r, params, options, default_code))
		val=string->as_double();
	else
		if(default_code)
			val=r.process_to_value(*default_code).as_double();
		else {
			throw Exception("parser.runtime",
				0,
				"produced no result, but no default option specified");
		}
	r.write_no_lang(*new VDouble(val));
}

// constructor

MDouble::MDouble(): Methoded("double") {
	// ^double.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);

	// ^double.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);
	
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
