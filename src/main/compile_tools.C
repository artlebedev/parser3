/** @file
	Parser: compiler support helper functions.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_COMPILE_TOOLS_C="$Date: 2002/08/01 11:41:17 $";

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_vstring.h"
#include "pa_vdouble.h"

void PV(Array/*<Operation>*/ *result, Value *value) {
	// append OP_VALUE
	Operation op; op.code=OP_VALUE;
	*result+=op.cast;

	// append 'value'
	*result+=value;
}

Value *LA2V(Array *literal_string_array, int offset) {
	Operation op;
	op.cast=literal_string_array->get(offset+0);
	return op.code==OP_VALUE?
		static_cast<Value *>(literal_string_array->get(offset+1))
		:0;
}

void change_string_literal_to_double_literal(Array *literal_string_array) {
	Operation op;
	op.cast=literal_string_array->get(0);
	if(op.code==OP_VALUE) {
		VString *vstring=static_cast<VString *>(literal_string_array->get(1));
		Value *value=vstring->as_expr_result();
		literal_string_array->put(1, value);
	}
}
void changetail_or_append(Array *opcodes, 
						  OPCODE find, bool with_argument, OPCODE replace, OPCODE notfound) {
	int tail=opcodes->size()-(with_argument?2:1);
	if(tail>=0) {
		Operation op;
		op.cast=opcodes->get(tail);
		if(op.code==find) {
			op.code=replace;
			opcodes->put(tail, op.cast);
			return;
		}
	}

	Operation op; op.code=notfound;
	*opcodes+=op.cast;
}


void push_LS(parse_control& pc, lexical_state new_state) { 
	if(pc.ls_sp<MAX_LEXICAL_STATES) {
		pc.ls_stack[pc.ls_sp++]=pc.ls;  
		pc.ls=new_state;
	} else
		throw Exception(0, 0, "push_LS: ls_stack overflow");
}
void pop_LS(parse_control& pc) {
	if(--pc.ls_sp>=0)
		pc.ls=pc.ls_stack[pc.ls_sp];
	else
		throw Exception(0, 0, "pop_LS: ls_stack underflow");
}
