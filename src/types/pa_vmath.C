/**	@file
	Parser: @b math class.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_vmath.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

volatile const char * IDENT_PA_VMATH_C="$Id: pa_vmath.C,v 1.19 2026/01/07 01:10:05 moko Exp $" IDENT_PA_VMATH_H;

// externs

extern Methoded* math_class;

// methods

VMath::VMath(): VStateless_class(math_class) {
	fconsts.put("PI", new VDouble(PI));
	fconsts.put("E", new VDouble(MathE));
	fconsts.put("int_max", new VInt(PA_WINT_MAX));
	fconsts.put("int_min", new VInt(PA_WINT_MIN));
}
