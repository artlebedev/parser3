/*
  $Id: compile_tools.C,v 1.3 2001/02/21 06:21:19 paf Exp $
*/

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"

Array *L(String *string) {
	// empty ops array
	Array *result=N(&string->pool());

	// append OP_STRING
	Operation op; op.code=OP_STRING;
	*result+=reinterpret_cast<Array::Item *>(op.cast);

	// append 'string'
	*result+=string;

	return result;
}

const String *LA2S(Array *literal_string_array) {
	return static_cast<const String *>(literal_string_array->get(1));
}


void push_LS(struct parse_control *pc) {
	if(pc->sp<MAX_LEXICAL_STATES) {
		pc->stack[pc->sp++]=pc->ls;  pc->ls=LS_VAR_NAME_SIMPLE;
	} else
		(static_cast<Pool *>(pc->pool))->exception().raise(0, 0, 0, 
			"push_LS: stack overflow");
}
void pop_LS(struct parse_control *pc) {
	if(--pc->sp>=0)
		pc->ls=pc->stack[pc->sp];
	else
		(static_cast<Pool *>(pc->pool))->exception().raise(0, 0, 0, 
			"push_LS: stack underflow");
}
