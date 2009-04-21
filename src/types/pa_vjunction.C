/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VJUNCTION_C="$Date: 2009/04/21 11:44:04 $";

// include

#include "pa_vjunction.h"
#include "pa_vbool.h"
#include "pa_wcontext.h"

void VJunction::reattach(WContext *new_wcontext){

#ifdef USE_DESTRUCTORS
	if(fjunction.wcontext)
#endif
	if(new_wcontext) {
		assert(fjunction.wcontext!=new_wcontext);
		fjunction.wcontext=new_wcontext;
		fjunction.wcontext->attach_junction(this);
		return;
	}

#ifdef USE_DESTRUCTORS
	if(fjunction.is_temporal){
		delete this;
		return;
	}
#endif
	fjunction.method_frame=0;
	fjunction.rcontext=0;
	fjunction.wcontext=0;
}


override Value& VJunction::as_expr_result(bool) {
	return VBool::get(as_bool());
}
