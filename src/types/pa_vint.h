/** @file
	Parser: int parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vint.h,v 1.16 2001/04/02 16:00:17 paf Exp $
*/

#ifndef PA_VINT_H
#define PA_VINT_H

#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "_int.h"

/// value of type 'int'. implemented with @c int
class VInt : public VStateless_object {
public: // Value

	/// all: for error reporting after fail(), etc
	const char *type() const { return "int"; }
	/// VInt: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

	/// VInt: finteger
	const String *get_string() {
		char *buf=(char *)pool().malloc(MAX_NUMBER);
		snprintf(buf, MAX_NUMBER, "%d", finteger);
		return NEW String(pool(), buf);
	}
	/// VInt: finteger
	double as_double() { return finteger; }
	/// VInt: 0 or !0
	bool as_bool() { return finteger!=0; }

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VInt(Pool& apool, int ainteger) : VStateless_object(apool, *int_class), 
		finteger(ainteger) {
	}

	int get_int() { return finteger; }
	void set_int(int ainteger) { finteger=ainteger; }

	void inc(int increment) { finteger+=increment; }
	void mul(double k) { finteger=(int)(finteger*k); }
	void div(double d) { finteger=(int)(finteger/d); }
	void mod(int d) { finteger%=d; }

private:

	int finteger;

};

#endif
