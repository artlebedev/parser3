/** @file
	Parser: compiler support helper functions.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_COMPILE_TOOLS_C="$Date: 2009/05/20 13:22:59 $";

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_vstring.h"
#include "pa_vdouble.h"

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

void changetail_or_append(ArrayOperation& opcodes, 
						  OP::OPCODE find, bool with_argument, OP::OPCODE replace, OP::OPCODE notfound) {
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


bool maybe_change_first_opcode(ArrayOperation& opcodes, OP::OPCODE find, OP::OPCODE replace, bool strict) {
	if(opcodes[0].code==find){
		opcodes.put(0, replace);
		return true;
	}
	if(strict)
		assert(opcodes[0].code==find);

	return false;
}


bool maybe_change_first_opcode(ArrayOperation& opcodes, OP::OPCODE find, OP::OPCODE last, OP::OPCODE replace) {
	if(opcodes[0].code==find && opcodes[opcodes.count()-1].code==last){
		opcodes.put(0, replace);
		return true;
	}
	return false;
}


// OP_VALUE+origin+value+OP_GET_ELEMENT+OP_VALUE+origin+value+OP_GET_ELEMENT => OP_GET_OBJECT_ELEMENT+origin+value+[OP_VALUE]+origin+value+OP_GET_ELEMENT
bool maybe_make_get_object_element(ArrayOperation& opcodes, ArrayOperation& diving_code, size_t divine_count){
	if(divine_count!=8)
		return false;

	assert(diving_code[0].code==OP::OP_VALUE);
	if(
		diving_code[4].code==OP::OP_VALUE
		&& diving_code[divine_count-1].code==OP::OP_GET_ELEMENT
	){
		O(opcodes, OP::OP_GET_OBJECT_ELEMENT);
		P(opcodes, diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
		P(opcodes, diving_code, 5, 3); // copy specified tail
		return true;
	}
	return false;
}

// OP_VALUE+origin+value+OP_GET_ELEMENT+OP_WITH_READ+OP_VALUE+origin+value+OP_GET_ELEMENT+OP_GET_ELEMENT => OP_GET_OBJECT_VAR_ELEMENT+origin+value+[OP_VALUE]+origin+value+OP_GET_ELEMENT
bool maybe_make_get_object_var_element(ArrayOperation& opcodes, ArrayOperation& diving_code, size_t divine_count){
	if(divine_count!=10)
		return false;

	assert(diving_code[0].code==OP::OP_VALUE);
	if(
		diving_code[4].code==OP::OP_WITH_READ
		&& diving_code[5].code==OP::OP_VALUE
		&& diving_code[divine_count-1].code==OP::OP_GET_ELEMENT
	){
		O(opcodes, OP::OP_GET_OBJECT_VAR_ELEMENT);
		P(opcodes, diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
		P(opcodes, diving_code, 6, 3); // copy specified tail
		return true;
	}
	return false;
}

// $a(1) or $.b(2)
// (OP_WITH_ROOT|OP_WITH_WRITE)+OP_VALUE+origin+value
// OP_PREPARE_TO_EXPRESSION+OP_VALUE+origin+value+CONSTRUCT_EXPR

// $c[d] or $.e[f]
// OP_WITH_ROOT+OP_VALUE+origin+value
// OP_VALUE+origin+value+OP_CONSTRUCT_VALUE
bool maybe_make_root_or_write_construct(ArrayOperation& opcodes, ArrayOperation& var_ops, ArrayOperation& expr_ops){
	if(var_ops.count()==4 && (var_ops[0].code==OP::OP_WITH_ROOT || var_ops[0].code==OP::OP_WITH_WRITE)){
		if(
			expr_ops.count()==5
			&& expr_ops[0].code==OP::OP_PREPARE_TO_EXPRESSION
			&& expr_ops[1].code==OP::OP_VALUE
			&& expr_ops[4].code==OP::OP_CONSTRUCT_EXPR
		){
			// $a(1) or $.b(2)
			O(opcodes, (var_ops[0].code==OP::OP_WITH_ROOT) ? OP::OP_ROOT_CONSTRUCT_EXPR : OP::OP_WRITE_CONSTRUCT_EXPR);
			P(opcodes, var_ops, 2/*offset*/); // skip OP_WITH+OP_VALUE and copy 1st origin+value
			P(opcodes, expr_ops, 2/*offset*/, 2/*limit*/); // copy 2nd origin+value
			return true;
		} else if(
			 expr_ops.count()==4
			 && expr_ops[0].code==OP::OP_VALUE
			 && expr_ops[3].code==OP::OP_CONSTRUCT_VALUE
		){
			// $c[d] or $.e[f]
			O(opcodes, (var_ops[0].code==OP::OP_WITH_ROOT) ? OP::OP_ROOT_CONSTRUCT_VALUE : OP::OP_WRITE_CONSTRUCT_VALUE);
			P(opcodes, var_ops, 2/*offset*/); // skip OP_WITH+OP_VALUE and copy 1st origin+value
			P(opcodes, expr_ops, 1/*offset*/, 2/*limit*/); // copy 2nd origin+value
			return true;
		}
	}
	return false;
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
