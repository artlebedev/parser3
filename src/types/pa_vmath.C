/**	@file
	Parser: @b math class.

	Copyright(c) 2001-2005 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

static const char * const IDENT_VMATH_C="$Date: 2005/08/05 13:03:05 $";

#include "pa_vmath.h"
#include "pa_vdouble.h"

// externs

extern Methoded* math_base_class;

// methods

VMath::VMath(): VStateless_class(0, math_base_class) {
	fconsts.put(String::Body("PI"), new VDouble(PI));
}
