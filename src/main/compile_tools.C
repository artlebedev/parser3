/** @file
	Parser: compiler support helper functions.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: compile_tools.C,v 1.42 2002/01/31 16:39:01 paf Exp $
*/

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

void PCA(Array/*<Operation>*/ *result, Array/*<Operation>*/ *code_array) {
	// append OP_CODE
	Operation op; op.code=OP_CURLY_CODE__STORE_PARAM;
	*result+=op.cast;

	// append 'vstring'
	*result+=code_array;
}

void PEA(Array/*<Operation>*/ *result, Array/*<Operation>*/ *code_array) {
	// append OP_CODE
	Operation op; op.code=OP_EXPR_CODE__STORE_PARAM;
	*result+=op.cast;

	// append 'vstring'
	*result+=code_array;
}

void CCA(Array/*<Operation>*/ *result, Array/*<Operation>*/ *code_array) {
	// append OP_CODE
	Operation op; op.code=OP_CURLY_CODE__CONSTRUCT;
	*result+=op.cast;

	// append 'vstring'
	*result+=code_array;
}

void PNC(Array/*<Operation>*/ *result, Array/*<Operation>*/ *code_array) {
	// append OP_CODE
	Operation op; op.code=OP_NESTED_CODE;
	*result+=op.cast;

	// append 'vstring'
	*result+=code_array;
}


Array *VL(Value *value) {
	// empty ops array
	Array *result=N(value->pool());

	// append 'value' to 'result'
	PV(result, value);

	return result;
}

Value *LA2V(Array *literal_string_array, int offset) {
	Operation op;
	op.cast=literal_string_array->get(offset+0);
	if(op.code!=OP_VALUE)
		return 0;

	return static_cast<Value *>(literal_string_array->get(offset+1));
}

void change_string_literal_to_double_literal(Array *literal_string_array) {
	VString *vstring=static_cast<VString *>(literal_string_array->get(1));
	Value *value=vstring->as_expr_result();
	literal_string_array->put(1, value);
}
void change_string_literal_to_write_string_literal(Array *literal_string_array) {
	Operation op; op.code=OP_STRING__WRITE;
	literal_string_array->put(0, op.cast);
}

void push_LS(parse_control& pc, lexical_state new_state) { 
	if(pc.ls_sp<MAX_LEXICAL_STATES) {
		pc.ls_stack[pc.ls_sp++]=pc.ls;  
		pc.ls=new_state;
	} else
		throw Exception(0, 0, 0, "push_LS: ls_stack overflow");
}
void pop_LS(parse_control& pc) {
	if(--pc.ls_sp>=0)
		pc.ls=pc.ls_stack[pc.ls_sp];
	else
		throw Exception(0, 0, 0, "pop_LS: ls_stack underflow");
}

void push_OCA(parse_control& pc, bool operator_call_allowed) { 
	if(pc.oca_sp<MAX_OPERATOR_STATES) {
		pc.oca_stack[pc.oca_sp++]=pc.operator_call_allowed; 
		pc.operator_call_allowed=operator_call_allowed;
	} else
		throw Exception(0, 0, 0, "push_OC: oca_stack overflow");
}
void pop_OCA(parse_control& pc) {
	if(--pc.oca_sp<0)
		throw Exception(0, 0, 0, "pop_OC: oca_stack underflow");
}
