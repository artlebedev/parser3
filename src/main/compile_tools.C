/*
  $Id: compile_tools.C,v 1.9 2001/02/22 13:58:43 paf Exp $
*/

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_vstring.h"

Array *L(VString *vstring) {
	// empty ops array
	Array *result=N(vstring->pool());

	// append OP_STRING
	Operation op; op.code=OP_STRING;
	*result+=op.cast;

	// append 'vstring'
	*result+=vstring;

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
