/** @file
	Parser: @b double parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

static const char* IDENT_VDOUBLE_H="$Date: 2002/09/23 14:09:25 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

extern Methoded *double_class;

/// value of type 'double'. implemented with @c double
class VDouble : public VStateless_object {
public: // Value

	const char *type() const { return "double"; }
	VStateless_class *get_class() { return double_class; }

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
			pool_buf, size/*, 
			name().origin().file, name().origin().line*/, 0, 0);
		return result;
	}
	/// VDouble: fdouble
	double as_double() const { return fdouble; }
	/// VDouble: fdouble
	int as_int() const { return (int)round(fdouble); }
	/// VDouble: 0 or !0
	bool as_bool() const { return fdouble!=0; }

public: // usage

	VDouble(Pool& apool, double adouble) : VStateless_object(apool), 
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
