/** @file
	Parser: @b math class decls.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMATH_H
#define PA_VMATH_H

static const char * const IDENT_VMATH_H="$Date: 2009/08/08 13:30:21 $";

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

	// math: CLASS,method,field
	Value* get_element(const String& aname) {
		// $CLASS,$method
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
