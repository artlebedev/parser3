/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vint.h,v 1.7 2001/03/12 21:54:21 paf Exp $
*/

#ifndef PA_VINT_H
#define PA_VINT_H

#include "pa_vstateless_object.h"
#include "pa_common.h"
#include "_int.h"

class VInt : public VStateless_object {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "int"; }
	// int: this
	Value *get_expr_result() { return this; }

	// integer: finteger
	const String *get_string() {
		char *buf=(char *)pool().malloc(MAX_NUMBER);
		snprintf(buf, MAX_NUMBER, "%d", finteger);
		String *result=NEW String(pool());
		result->APPEND_CONST(buf);
		return result;
	}
	// integer: finteger
	double get_double() { return finteger; }
	// integer: 0 or !0
	bool get_bool() { return finteger!=0; }

public: // usage

	VInt(Pool& apool, int ainteger) : VStateless_object(apool, *int_class), 
		finteger(ainteger) {
	}

	int get_int() { return finteger; }

	void inc(int increment=1) { finteger+=increment; }

private:

	int finteger;

};

#endif
