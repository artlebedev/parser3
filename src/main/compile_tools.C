/** @file
	Parser: compiler support helper functions.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_vstring.h"
#include "pa_vdouble.h"
#include "pa_vmethod_frame.h"

volatile const char * IDENT_COMPILE_TOOLS_C="$Id: compile_tools.C,v 1.79 2017/02/07 22:00:40 moko Exp $" IDENT_COMPILE_TOOLS_H;

Value* LA2V(ArrayOperation& literal_string_array, int offset, OP::OPCODE code) {
	return literal_string_array[offset+0].code==code?literal_string_array[offset+2/*skip opcode&origin*/].value
		:0;
}

void maybe_change_string_literal_to_double_literal(ArrayOperation& literal_array) {
	assert(literal_array[0].code==OP::OP_VALUE);
	VString& vstring=*static_cast<VString*>(literal_array[2/*opcode+origin*/].value);
	if(isdigit(vstring.string().first_char()))
		literal_array.put(2/*opcode+origin*/, &vstring.as_expr_result());
}

void change_string_literal_value(ArrayOperation& literal_string_array, const String& new_value) {
	assert(literal_string_array[0].code==OP::OP_VALUE);
	static_cast<VString*>(literal_string_array[2/*opcode+origin*/].value)->set_string(new_value);
}

bool change_first(ArrayOperation& opcodes, OP::OPCODE find, OP::OPCODE replace) {
	Operation& op=opcodes.get_ref(0);
	if(op.code!=find)
		return false;

	op.code=replace;
	return true;
}


// OP_VALUE+origin+self+OP_GET_ELEMENT+OP_VALUE+origin+value+OP_GET_ELEMENT => OP_WITH_SELF__VALUE__GET_ELEMENT+origin+value
bool maybe_make_self(ArrayOperation& opcodes, ArrayOperation& diving_code, size_t divine_count){
	const String* first_name=LA2S(diving_code);

	if(first_name && SYMBOLS_EQ(*first_name,SELF_SYMBOL)){
#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
		if(
			divine_count>=8
			&& diving_code[3].code==OP::OP_GET_ELEMENT
			&& diving_code[4].code==OP::OP_VALUE
			&& diving_code[7].code==OP::OP_GET_ELEMENT
		){
			// optimization for $self.field and ^self.method
			O(opcodes, OP::OP_WITH_SELF__VALUE__GET_ELEMENT);
			P(opcodes, diving_code, 5/*offset*/, 2/*limit*/); // copy second origin+value. we know that the first one is "self"
			if(divine_count>8)
				P(opcodes, diving_code, 8/*offset*/); // copy tail
		} else
#endif
		{
			// self.xxx... => xxx...
			// OP_VALUE+origin+string+OP_GET_ELEMENT+... -> OP_WITH_SELF+...
			O(opcodes, OP::OP_WITH_SELF); /* stack: starting context */
			P(opcodes, diving_code, divine_count>=4?4/*OP::OP_VALUE+origin+string+OP::OP_GET_ELEMENTx*/:3/*OP::OP_+origin+string*/);
		}
		return true;
	}
	return false;
}

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
bool maybe_append_simple_diving_code(ArrayOperation& code, ArrayOperation& diving_code){
	if(
		diving_code.count()==3
		&& diving_code[0].code==OP::OP_STRING__WRITE
	){
		O(code, OP::OP_VALUE);
		P(code, diving_code, 1/*offset*/, 2/*limit*/); /*copy origin+value*/
		return true;
	} else {
		return false;
	}
}

bool is_special_element(ArrayOperation& opcodes){
	const String* name=LA2S(opcodes);
	return name && ( SYMBOLS_EQ(*name,CLASS_SYMBOL) || SYMBOLS_EQ(*name,CLASS_NAME_SYMBOL) );
}
#endif

Method::Call_type GetMethodCallType(Parse_control& pc, ArrayOperation& literal_array) {
	const String* full_name=LA2S(literal_array);
	int pos=full_name->pos(':');
	if(pos > 0) {
		const String call_type=full_name->mid(0, pos);
		if(call_type!=Symbols::STATIC_SYMBOL)
			throw Exception("parser.compile", &call_type, "incorrect method call type. the only valid call type method prefix is 'static'");
		const String *sole_name=&full_name->mid(pos+1, full_name->length());
		// replace full method name (static:method) by sole method name (method). it will be used later.
		change_string_literal_value(literal_array, *sole_name);
		return Method::CT_STATIC;
	}
	return pc.get_methods_call_type();
}

void push_LS(Parse_control& pc, lexical_state new_state) { 
	if(pc.ls_sp<MAX_LEXICAL_STATES) {
		pc.ls_stack[pc.ls_sp++]=pc.ls;  
		pc.ls=new_state;
	} else
		throw Exception(0, 0, "push_LS: ls_stack overflow");
}
void pop_LS(Parse_control& pc) {
	if(--pc.ls_sp>=0)
		pc.ls=pc.ls_stack[pc.ls_sp];
	else
		throw Exception(0, 0, "pop_LS: ls_stack underflow");
}

const String& Parse_control::alias_method(const String& name) {
	return (main_alias && name==main_method_name)?*main_alias:name;
}
