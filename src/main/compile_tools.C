/*
  $Id: compile_tools.C,v 1.8 2001/02/22 13:52:26 paf Exp $
*/

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_value.h"

Array *L(Value *value) {
	// empty ops array
	Array *result=N(value->pool());

	// append OP_STRING
	Operation op; op.code=OP_VALUE;
	*result+=op.cast;

	// append 'value'
	*result+=value;

	return result;
}

String *LA2S(Array *literal_string_array, int offset) {
	return static_cast<Value *>(literal_string_array->get(offset+1))->get_string();
}


void push_LS(struct parse_control *pc, lexical_state new_state) {
	if(pc->sp<MAX_LEXICAL_STATES) {
		pc->stack[pc->sp++]=pc->ls;  pc->ls=new_state;
	} else
		pc->pool->THROW(0, 0, 0, "push_LS: stack overflow");
}
void pop_LS(struct parse_control *pc) {
	if(--pc->sp>=0)
		pc->ls=pc->stack[pc->sp];
	else
		pc->pool->THROW(0, 0, 0, "push_LS: stack underflow");
}
