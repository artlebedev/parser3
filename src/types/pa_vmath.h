/** @file
	Parser: @b math class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vmath.h,v 1.1 2001/07/06 11:13:35 parser Exp $
*/

#ifndef PA_VMATH_H
#define PA_VMATH_H

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

	// math: CLASS,BASE,method,field
	Value *get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_class::get_element(aname))
			return result;

		// $const
		return static_cast<Value *>(fconsts.get(aname));
	}

public: // usage

	VMath(Pool& apool) : VStateless_class(apool, math_base_class),
		fconsts(*math_consts) {
	}

	Hash& consts() { return fconsts; }

private:

	Hash& fconsts;

};

#endif
