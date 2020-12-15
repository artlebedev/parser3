/** @file
	Parser: @b math class decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMATH_H
#define PA_VMATH_H

#define IDENT_PA_VMATH_H "$Id: pa_vmath.h,v 1.25 2020/12/15 17:10:41 moko Exp $"

// includes

#include "classes.h"
#include "pa_common.h"

// defines

#define PI		3.14159265358979
#define MathE	2.718281828459

/**
*/
class VMath: public VStateless_class {
public: // Value
	
	const char* type() const { return "math"; }

	// math: method,field
	Value* get_element(const String& aname) {
		// $method
		if(Value* result=VStateless_class::get_element(aname))
			return result;

		// $const
		return fconsts.get(aname);
	}

public: // usage

	VMath();

private:

	HashStringValue fconsts;

};

#endif
