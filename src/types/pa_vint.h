/** @file
	Parser: @b int parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vint.h,v 1.25 2001/08/20 13:22:05 parser Exp $
*/

#ifndef PA_VINT_H
#define PA_VINT_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

extern Methoded *int_class;

/// value of type 'int'. implemented with @c int
class VInt : public VStateless_object {
public: // Value

	const char *type() const { return "int"; }
	/// VInt: clone
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VInt(pool(), finteger); }

	/// VInt: finteger
	const String *get_string() {
		char *buf=(char *)pool().malloc(MAX_NUMBER);
		String *result=NEW String(pool());
		result->APPEND_CLEAN(
			buf, snprintf(buf, MAX_NUMBER, "%d", finteger), 
			name().origin().file, name().origin().line);
		return result;
	}
	/// VInt: finteger
	double as_double() { return as_int(); }
	/// VInt: finteger
	int as_int() { return finteger; }
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
