/**	@file
	Parser: @b math class.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

#include "pa_vmath.h"
#include "pa_vdouble.h"

volatile const char * IDENT_PA_VMATH_C="$Id: pa_vmath.C,v 1.14 2016/04/06 16:08:20 moko Exp $" IDENT_PA_VMATH_H;

// externs

extern Methoded* math_class;

// methods

VMath::VMath(): VStateless_class(math_class) {
	fconsts.put("PI", new VDouble(PI));
	fconsts.put("E", new VDouble(MathE));
}
