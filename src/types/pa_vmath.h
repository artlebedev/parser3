/** @file
	Parser: @b math class decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMATH_H
#define PA_VMATH_H

static const char* IDENT_VMATH_H="$Date: 2002/08/13 13:02:42 $";

#include "classes.h"
#include "pa_common.h"

extern Methoded *math_base_class;
extern Hash *math_consts;

class Request;

/**
*/
class VMath : public VStateless_class {
public: // Value
	
	const char *type() const { return "math"; }

	// math: CLASS,method,field
	Value *get_element(const String& aname, Value *aself) {
		// $CLASS,$method
		if(Value *result=VStateless_class::get_element(aname, aself))
			return result;

		// $const
		return static_cast<Value *>(fconsts.get(aname));
	}

public: // usage

	VMath(Pool& apool) : VStateless_class(apool, 0, math_base_class),
		fconsts(*math_consts) {
	}

	Hash& consts() { return fconsts; }

private:

	Hash& fconsts;

};

#endif
