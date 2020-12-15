/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

// include

#include "pa_vjunction.h"
#include "pa_vbool.h"
#include "pa_wcontext.h"
#include "pa_symbols.h"

volatile const char * IDENT_PA_VJUNCTION_C="$Id: pa_vjunction.C,v 1.20 2020/12/15 17:10:41 moko Exp $" IDENT_PA_VJUNCTION_H IDENT_PA_JUNCTION_H;

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
#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	// $CLASS
	if(SYMBOLS_EQ(aname,CLASS_SYMBOL))
		return this;

	// $CLASS_NAME
	if(SYMBOLS_EQ(aname,CLASS_NAME_SYMBOL))
		return new VString(junction_class_name);
#endif

	// $name
	if(SYMBOLS_EQ(aname,NAME_SYMBOL))
		if(fjunction.method)
			return new VString(*fjunction.method->name);

	return Value::get_element(aname);
}
