/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

// include

#include "pa_vjunction.h"
#include "pa_vbool.h"
#include "pa_wcontext.h"

volatile const char * IDENT_PA_VJUNCTION_C="$Id: pa_vjunction.C,v 1.12 2012/05/23 16:26:41 moko Exp $" IDENT_PA_VJUNCTION_H IDENT_PA_JUNCTION_H;

void VJunction::reattach(WContext *new_wcontext){
	if(new_wcontext) {
		assert(fjunction.wcontext!=new_wcontext);
		fjunction.wcontext=new_wcontext;
		fjunction.wcontext->attach_junction(this);
	} else {
		fjunction.method_frame=0;
		fjunction.rcontext=0;
		fjunction.wcontext=0;
	}
}


override Value& VJunction::as_expr_result() {
	return VBool::get(false);
}


Value* VJunction::get_element(const String& aname) {
	// .CLASS
	if(aname==CLASS_NAME)
		return this;
	// .CLASS_NAME
	if(aname==CLASS_NAMETEXT)
		return new VString(junction_class_name);
	return Value::get_element(aname);
}
