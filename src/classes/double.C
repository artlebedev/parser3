/** @file
	Parser: @b double parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: double.C,v 1.34 2001/07/26 12:25:37 parser Exp $"; 

#include "classes.h"
#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// externs

void _string_format(Request& r, const String& method_name, MethodParams *);

// defines

#define DOUBLE_CLASS_NAME "double"

// class

class MDouble : public Methoded {
public:
	MDouble(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _int(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	Value& result=*new(pool) VInt(pool, vdouble->as_int());
	result.set_name(method_name);
	r.write_no_lang(result);
}

static void _double(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	Value& result=*new(pool) VDouble(pool, vdouble->as_double());
	result.set_name(method_name);
	r.write_no_lang(result);
}

typedef void (*vdouble_op_func_ptr)(VDouble& vdouble, double param);

static void __inc(VDouble& vdouble, double param) { vdouble.inc(param); }
static void __dec(VDouble& vdouble, double param) { vdouble.inc(-param); }
static void __mul(VDouble& vdouble, double param) { vdouble.mul(param); }
static void __div(VDouble& vdouble, double param) { vdouble.div(param); }
static void __mod(VDouble& vdouble, double param) { vdouble.mod((int)param); }

static void vdouble_op(Request& r, MethodParams *params, 
					   vdouble_op_func_ptr func) {
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	double param=params->size()?
		r.process(
			params->get(0),
			0/*no name*/,
			false/*don't intercept string*/).as_double():1/*used in inc/dec*/;
	(*func)(*vdouble, param);
}

static void _inc(Request& r, const String&, MethodParams *params) { vdouble_op(r, params, &__inc); }
static void _dec(Request& r, const String&, MethodParams *params) { vdouble_op(r, params, &__dec); }
static void _mul(Request& r, const String&, MethodParams *params) { vdouble_op(r, params, &__mul); }
static void _div(Request& r, const String&, MethodParams *params) { vdouble_op(r, params, &__div); }
static void _mod(Request& r, const String&, MethodParams *params) { vdouble_op(r, params, &__mod); }

// from string.C
extern 
const String* sql_result_string(Request& r, const String& method_name, 
								MethodParams *params);

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	const String *string=sql_result_string(r, method_name, params);
	double val=string?string->as_double():params->as_double(1, r);
	VDouble& result=*new(pool) VDouble(pool, val);
	result.set_name(method_name);
	r.write_assign_lang(result);
}

// constructor

MDouble::MDouble(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DOUBLE_CLASS_NAME));


	// ^double.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);

	// ^double.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);
	
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
	
	// ^double:sql[query]
	// ^double:sql[query](default)
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);
}
// global variable

Methoded *double_class;

// creator

Methoded *MDouble_create(Pool& pool) {
	return double_class=new(pool) MDouble(pool);
}
