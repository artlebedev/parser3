/*
  $Id: compile_tools.C,v 1.6 2001/02/22 09:14:46 paf Exp $
*/

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_vstring.h"

Array *L(String *string) {
	// empty ops array
	Array *result=N(string->pool());

	// append OP_STRING
	Operation op; op.code=OP_STRING;
	*result+=op.cast;

	// append 'string'
	*result+=string;

	return result;
}

String *LA2S(Array *literal_string_array, int offset) {
	return static_cast<String *>(literal_string_array->get(offset+1));
}
Array *LAS2LAVS(Array *literal_array) {
	Operation op; op.code=OP_VSTRING;
	literal_array->put(0, op.cast);

	Value *value=new(literal_array->pool()) 
		VString(static_cast<String *>(literal_array->get(1)));
	literal_array->put(1, value);

	return literal_array;
}


void push_LS(struct parse_control *pc, lexical_state new_state) {
	if(pc->sp<MAX_LEXICAL_STATES) {
		pc->stack[pc->sp++]=pc->ls;  pc->ls=new_state;
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
