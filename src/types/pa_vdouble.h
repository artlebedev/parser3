/** @file
	Parser: @b double parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vdouble.h,v 1.35 2002/04/09 08:10:37 paf Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

extern Methoded *double_class;

/// value of type 'double'. implemented with @c double
class VDouble : public VStateless_object {
public: // Value

	const char *type() const { return "double"; }
	/// VDouble: clone
	Value *as_expr_result(bool ) { return NEW VDouble(pool(), fdouble); }

	/** VDouble: fdouble 
	*/
	const String *get_string() {
		char local_buf[MAX_NUMBER];
		size_t size=snprintf(local_buf, MAX_NUMBER, "%g", fdouble);
		
		char *pool_buf=(char *)pool().malloc(size);
		memcpy(pool_buf, local_buf, size);

		String *result=NEW String(pool());
		result->APPEND_CLEAN(
			pool_buf, size, 
			name().origin().file, name().origin().line);
		return result;
	}
	/// VDouble: fdouble
	double as_double() const { return fdouble; }
	/// VDouble: fdouble
	int as_int() const { return (int)fdouble; }
	/// VDouble: 0 or !0
	bool as_bool() const { return fdouble!=0; }

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

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
