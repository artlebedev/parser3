/**	@file
	Parser: @b math class.

	Copyright(c) 2001-2003 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

static const char * const IDENT_VMATH_C="$Date: 2003/12/11 09:25:50 $";

#include "pa_vmath.h"
#include "pa_vdouble.h"

// externs

extern Methoded* math_base_class;

// methods

VMath::VMath(): VStateless_class(0, math_base_class) {
	fconsts.put("PI", new VDouble(PI));
}
