/** @file
	Parser: @b int parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_INT_C="$Id: int.C,v 1.45 2002/08/01 11:26:43 paf Exp $";

#include "classes.h"
#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// externs

void _string_format(Request& r, const String& method_name, MethodParams *);

// class

class MInt : public Methoded {
public:
	MInt(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _int(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	 // just checking (default) syntax validity, never really using it  here, just for string.int compatibility
	if(params->size()>0)
		params->as_junction(0, "default must be int");

	VInt *vint=static_cast<VInt *>(r.self);
	r.write_no_lang(*new(pool) VInt(pool, vint->get_int()));
}

static void _double(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	 // just checking (default) syntax validity, never really using it  here, just for string.doube compatibility
	if(params->size()>0)
		params->as_junction(0, "default must be double");

	VInt *vint=static_cast<VInt *>(r.self);
	r.write_no_lang(*new(pool) VDouble(pool, vint->as_double()));
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
	double param=params->size()?params->as_double(0, "param must be numerical", r):1;
	(*func)(*vint, param);
}

static void _inc(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__inc); }
static void _dec(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__dec); }
static void _mul(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__mul); }
static void _div(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__div); }
static void _mod(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__mod); }

// from string.C
extern 
const String* sql_result_string(Request& r, const String& method_name, MethodParams *params,
								Hash *& options, Value *& default_code);

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	int val;
	Hash *options;
	Value *default_code;
	if(const String *string=sql_result_string(r, method_name, params, options, default_code))
		val=string->as_int();
	else
		if(default_code)
			val=r.process_to_value(*default_code).as_int();
		else {
			throw Exception("parser.runtime",
				&method_name,
				"produced no result, but no default option specified");
			val=0; //calm, compiler
		}
	r.write_no_lang(*new(pool) VInt(pool, val));
}

// constructor

MInt::MInt(Pool& apool) : Methoded(apool, "int") {
	// ^int.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);

	// ^int.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);

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
// global variable

Methoded *int_class;

// creator

Methoded *MInt_create(Pool& pool) {
	return int_class=new(pool) MInt(pool);
}
