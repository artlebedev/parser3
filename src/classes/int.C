/** @file
	Parser: @b int parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: int.C,v 1.26 2001/05/21 16:55:52 parser Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// externs

void _string_format(Request& r, const String& method_name, MethodParams *);

// defines

#define INT_CLASS_NAME "int"

// class

class MInt : public Methoded {
public:
	MInt(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _int(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& result=*new(pool) VInt(pool, vint->get_int());
	result.set_name(method_name);
	r.write_no_lang(result);
}

static void _double(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& result=*new(pool) VDouble(pool, vint->as_double());
	result.set_name(method_name);
	r.write_no_lang(result);
}

typedef void (*vint_op_func_ptr)(VInt& vint, double param);

static void __inc(VInt& vint, double param) { vint.inc((int)param); }
static void __dec(VInt& vint, double param) { vint.inc((int)-param); }
static void __mul(VInt& vint, double param) { vint.mul(param); }
static void __div(VInt& vint, double param) { vint.div(param); }
static void __mod(VInt& vint, double param) { vint.mod((int)param); }

static void vint_op(Request& r, MethodParams *params, 
					 vint_op_func_ptr func) {
	VInt *vint=static_cast<VInt *>(r.self);
	double param=params->size()?
		r.process(
			params->get(0),
			0/*no name*/,
			false/*don't intercept string*/).as_double():1;
	(*func)(*vint, param);
}

static void _inc(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__inc); }
static void _dec(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__dec); }
static void _mul(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__mul); }
static void _div(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__div); }
static void _mod(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__mod); }

// from string.C
extern 
String& sql_result_string(Request& r, const String& method_name, MethodParams *params);

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	int val=sql_result_string(r, method_name, params).as_int();

	VInt& result=*new(pool) VInt(pool, val);
	result.set_name(method_name);
	r.write_assign_lang(result);
}

// constructor

MInt::MInt(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), INT_CLASS_NAME));


	// ^int.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);

	// ^int.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);

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

	// ^int:sql[query]
	// ^int:sql[query](offset)
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);
}
// global variable

Methoded *int_class;

// creator

Methoded *MInt_create(Pool& pool) {
	return int_class=new(pool) MInt(pool);
}
