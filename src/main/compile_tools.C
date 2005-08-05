/** @file
	Parser: compiler support helper functions.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_COMPILE_TOOLS_C="$Date: 2005/08/05 13:03:00 $";

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_vstring.h"
#include "pa_vdouble.h"

Value* LA2V(ArrayOperation& literal_string_array, int offset) {
	return literal_string_array[offset+0].code==OP_VALUE?literal_string_array[offset+2/*skip opcode&origin*/].value
		:0;
}

void maybe_change_string_literal_to_double_literal(ArrayOperation& literal_array) {
	assert(literal_array[0].code==OP_VALUE);
	VString& vstring=*static_cast<VString*>(literal_array[2/*opcode+origin*/].value);
	if(isdigit(vstring.string().first_char()))
		literal_array.put(2/*opcode+origin*/, &vstring.as_expr_result());
}

void change_string_literal_value(ArrayOperation& literal_string_array, const String& new_value) {
	assert(literal_string_array[0].code==OP_VALUE);
	static_cast<VString*>(literal_string_array[2/*opcode+origin*/].value)->set_string(new_value);
}

void changetail_or_append(ArrayOperation& opcodes, 
						  OPCODE find, bool with_argument, OPCODE replace, OPCODE notfound) {
	int tail=opcodes.count()-(with_argument?2:1);
	if(tail>=0) {
		Operation& op=opcodes.get_ref(tail);
		if(op.code==find) {
			op.code=replace;
			return;
		}
	}

	opcodes+=Operation(notfound);
}


void push_LS(Parse_control& pc, lexical_state new_state) { 
	if(pc.ls_sp<MAX_LEXICAL_STATES) {
		pc.ls_stack[pc.ls_sp++]=pc.ls;  
		pc.ls=new_state;
	} else
		throw Exception(0, 0, 
			"push_LS: ls_stack overflow");
}
void pop_LS(Parse_control& pc) {
	if(--pc.ls_sp>=0)
		pc.ls=pc.ls_stack[pc.ls_sp];
	else
		throw Exception(0, 0, 
			"pop_LS: ls_stack underflow");
}

const String& Parse_control::alias_method(const String& name) {
	return (main_alias && name==main_method_name)?*main_alias:name;
}
