/** @file
	Parser: compiler support helper functions.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile_tools.C,v 1.32 2001/03/27 16:35:54 paf Exp $
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

Array *VL(Value *value) {
	// empty ops array
	Array *result=N(value->pool());

	// append 'value' to 'result'
	PV(result, value);

	return result;
}

const String *SLA2S(Array *literal_string_array, int offset) {
	Operation op;
	op.cast=literal_string_array->get(offset+0);
	if(op.code!=OP_VALUE)
		return 0;

	return static_cast<VString *>(literal_string_array->get(offset+1))->get_string();
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
	if(pc.sp<MAX_LEXICAL_STATES) {
		pc.stack[pc.sp++]=pc.ls;  pc.ls=new_state;
	} else
		pc.pool->THROW(0, 0, 0, "push_LS: stack overflow");
}
void pop_LS(parse_control& pc) {
	if(--pc.sp>=0)
		pc.ls=pc.stack[pc.sp];
	else
		pc.pool->THROW(0, 0, 0, "push_LS: stack underflow");
}
