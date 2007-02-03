/**	@file
	Parser: @b property class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VPROPERTY_C="$Date: 2007/02/03 18:08:38 $";

// include

#include "pa_vproperty.h"
#include "pa_vbool.h"

override Value& VProperty::as_expr_result(bool) {
	return *new VBool(as_bool());
}
