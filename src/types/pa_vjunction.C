/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VJUNCTION_C="$Date: 2009/04/16 01:08:27 $";

// include

#include "pa_vjunction.h"
#include "pa_vbool.h"

override Value& VJunction::as_expr_result(bool) {
	return VBool::get(as_bool());
}
