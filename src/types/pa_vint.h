/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vint.h,v 1.3 2001/03/11 12:22:00 paf Exp $
*/

#ifndef PA_VINT_H
#define PA_VINT_H

#include "pa_value.h"
#include "pa_common.h"
#include "_int.h"

#define MAX_INT_AS_STRING 20

class VInt : public VObject_base {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "int"; }
	// int: this
	Value *get_expr_result() { return this; }

	// integer: finteger
	const String *get_string() {
		char *buf=static_cast<char *>(pool().malloc(MAX_INT_AS_STRING));
		snprintf(buf, MAX_INT_AS_STRING, "%d", finteger);
		String *result=NEW String(pool());
		result->APPEND_CONST(buf);
		return result;
	}
	// integer: finteger
	double get_double() { return finteger; }
	// integer: 0 or !0
	bool get_bool() { return finteger!=0; }

public: // usage

	VInt(Pool& apool, int ainteger) : VObject_base(apool, *int_class), 
		finteger(ainteger) {
	}

	int get_int() { return finteger; }

	void inc(int increment) { finteger+=increment; }

private:

	int finteger;

};

#endif
