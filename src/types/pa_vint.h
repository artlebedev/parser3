/** @file
	Parser: @b int parser class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VINT_H
#define PA_VINT_H

static const char * const IDENT_VINT_H="$Date: 2005/08/05 13:03:04 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

extern Methoded* int_class;

/// value of type 'int'. implemented with @c int
class VInt: public VStateless_object {
public: // Value

	override const char* type() const { return "int"; }
	override VStateless_class *get_class() { return int_class; }

	/// VInt: clone
	override Value& as_expr_result(bool) { return *new VInt(finteger); }

	/// VInt: finteger
	override const String* get_string() {
		char local_buf[MAX_NUMBER];
		size_t length=snprintf(local_buf, MAX_NUMBER, "%d", finteger);
		return new String(strdup(local_buf, length), length);
	}
	/// VInt: finteger
	override double as_double() const { return as_int(); }
	/// VInt: finteger
	override int as_int() const { return finteger; }
	/// VInt: 0 or !0
	override bool as_bool() const { return finteger!=0; }

public: // usage

	VInt(int ainteger): finteger(ainteger) {}

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
