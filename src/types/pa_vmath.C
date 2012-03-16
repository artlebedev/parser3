/**	@file
	Parser: @b math class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

#include "pa_vmath.h"
#include "pa_vdouble.h"

volatile const char * IDENT_PA_VMATH_C="$Id: pa_vmath.C,v 1.10 2012/03/16 09:24:18 moko Exp $" IDENT_PA_VMATH_H;

// externs

extern Methoded* math_base_class;

// methods

VMath::VMath(): VStateless_class(0, math_base_class) {
	fconsts.put(String::Body("PI"), new VDouble(PI));
	fconsts.put(String::Body("E"), new VDouble(MathE));
}
