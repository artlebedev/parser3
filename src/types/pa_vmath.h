/** @file
	Parser: @b math class decls.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMATH_H
#define PA_VMATH_H

static const char * const IDENT_VMATH_H="$Date: 2004/02/11 15:33:18 $";

// includes

#include "classes.h"
#include "pa_common.h"

// defines

#define PI 3.1415926535

/**
*/
class VMath: public VStateless_class {
public: // Value
	
	const char* type() const { return "math"; }

	// math: CLASS,method,field
	Value* get_element(const String& aname, Value& aself, bool looking_up) {
		// $CLASS,$method
		if(Value* result=VStateless_class::get_element(aname, aself, looking_up))
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
