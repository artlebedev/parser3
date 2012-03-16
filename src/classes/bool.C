/** @file
	Parser: @b int parser class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "pa_vbool.h"

volatile const char * IDENT_BOOL_C="$Id: bool.C,v 1.4 2012/03/16 09:24:06 moko Exp $" IDENT_PA_VBOOL_H;

// externs

void _string_format(Request& r, MethodParams&);

// class

class MBool: public Methoded {
public:
	MBool();
};

// global variable

DECLARE_CLASS_VAR(bool, new MBool, 0);

// methods

static void _int(Request& r, MethodParams& params) {
	// just checking (default) syntax validity, never really using it here, just for string.int compatibility
	if(params.count()>0)
		params.as_int(0, "default must be int", r);

	VBool& vbool=GET_SELF(r, VBool);
	r.write_no_lang(*new VInt(vbool.as_bool()));
}

static void _double(Request& r, MethodParams& params) {
	// just checking (default) syntax validity, never really using it here, just for string.double compatibility
	if(params.count()>0)
		params.as_double(0, "default must be double", r);

	VBool& vbool=GET_SELF(r, VBool);
	r.write_no_lang(*new VDouble(vbool.as_bool()));
}

static void _bool(Request& r, MethodParams& params) {
	// just checking (default) syntax validity, never really using it here, just for string.bool compatibility
	if(params.count()>0)
		params.as_bool(0, "default must be bool", r);

	r.write_no_lang(GET_SELF(r, VBool));
}

//typedef void (*vbool_op_func_ptr)(VBool& vbool, double param);

//static void vbool_op(Request& r, MethodParams& params, 
//					 vbool_op_func_ptr func) {
//	VBool& vbool=GET_SELF(r, VBool);
//	double param=params.count()?params.as_bool(0, "param must be bool", r):1;
//	(*func)(vbool, param);
//}

// constructor

MBool::MBool(): Methoded("bool") {
	// ^bool.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);
	// ^bool.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);
	// ^bool.bool[]
	add_native_method("bool", Method::CT_DYNAMIC, _bool, 0, 1);
}
