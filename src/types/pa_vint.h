/** @file
	Parser: @b int parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VINT_H
#define PA_VINT_H

static const char* IDENT_VINT_H="$Id: pa_vint.h,v 1.35 2002/08/01 11:26:55 paf Exp $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

extern Methoded *int_class;

/// value of type 'int'. implemented with @c int
class VInt : public VStateless_object {
public: // Value

	const char *type() const { return "int"; }
	VStateless_class *get_class() { return int_class; }

	/// VInt: clone
	Value *as_expr_result(bool ) { return NEW VInt(pool(), finteger); }

	/// VInt: finteger
	const String *get_string() {
		char local_buf[MAX_NUMBER];
		size_t size=snprintf(local_buf, MAX_NUMBER, "%d", finteger);

		char *pool_buf=(char *)pool().malloc(size);
		memcpy(pool_buf, local_buf, size);
		
		String *result=NEW String(pool());
		result->APPEND_CLEAN(
			pool_buf, size/*, 
			name().origin().file, name().origin().line*/,0,0);
		return result;
	}
	/// VInt: finteger
	double as_double() const { return as_int(); }
	/// VInt: finteger
	int as_int() const { return finteger; }
	/// VInt: 0 or !0
	bool as_bool() const { return finteger!=0; }

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VInt(Pool& apool, int ainteger) : VStateless_object(apool), 
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
