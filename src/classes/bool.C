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

volatile const char * IDENT_BOOL_C="$Id: bool.C,v 1.11 2020/12/15 17:10:27 moko Exp $" IDENT_PA_VBOOL_H;

// externs

void _string_format(Request& r, MethodParams&);

// class

class MBool: public Methoded {
public:
	MBool();
};

// global variable

DECLARE_CLASS_VAR(bool, new MBool);

// methods

static void _int(Request& r, MethodParams&) {
	VBool& vbool=GET_SELF(r, VBool);
	r.write(*new VInt(vbool.as_bool()));
}

static void _double(Request& r, MethodParams&) {
	VBool& vbool=GET_SELF(r, VBool);
	r.write(*new VDouble(vbool.as_bool()));
}

static void _bool(Request& r, MethodParams&) {
	r.write(GET_SELF(r, VBool));
}

// constructor

MBool::MBool(): Methoded("bool") {
	// ^bool.int[]
	// ^bool.int[default for ^string.int compatibility]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);
	// ^bool.double[]
	// ^bool.double[default for ^string.double compatibility]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);
	// ^bool.bool[]
	// ^bool.bool[default for ^string.bool compatibility]
	add_native_method("bool", Method::CT_DYNAMIC, _bool, 0, 1);
}
