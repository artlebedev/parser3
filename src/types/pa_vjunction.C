/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

// include

#include "pa_vjunction.h"
#include "pa_vbool.h"
#include "pa_wcontext.h"

volatile const char * IDENT_PA_VJUNCTION_C="$Id: pa_vjunction.C,v 1.16 2016/04/06 22:45:33 moko Exp $" IDENT_PA_VJUNCTION_H IDENT_PA_JUNCTION_H;

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


#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
Value* VJunction::get_element(const String& aname) {
	// .CLASS
	if(aname==class_element_name)
		return this;
	// .CLASS_NAME
	if(aname==class_name_element_name)
		return new VString(junction_class_name);
	return Value::get_element(aname);
}
#endif
