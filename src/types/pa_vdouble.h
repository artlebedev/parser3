/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdouble.h,v 1.10 2001/03/26 08:27:28 paf Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "_double.h"

class VDouble : public VStateless_object {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "double"; }
	// double: this
	Value *get_expr_result() { return this; }

	// double: fdouble
	const String *get_string() {
		char *buf=(char *)pool().malloc(MAX_NUMBER);
		snprintf(buf, MAX_NUMBER, "%g", fdouble);
		return NEW String(pool(), buf);
	}
	// double: fdouble
	double get_double() { return fdouble; }
	// double: 0 or !0
	bool get_bool() { return fdouble!=0; }

protected: // VAliased

	VStateless_class *get_class_alias() { return 0; }

public: // usage

	VDouble(Pool& apool, double adouble) : VStateless_object(apool, *double_class), 
		fdouble(adouble) {
	}

	void inc(double increment) { fdouble+=increment; }
	void mul(double k) { fdouble*=k; }
	void div(double d) { fdouble/=d; }
	void mod(int d) { fdouble=((int)fdouble)%d; }

private:

	double fdouble;

};

#endif
