/*
  $Id: compile_tools.C,v 1.13 2001/02/24 08:28:37 paf Exp $
*/

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_vstring.h"

void PVS(Array/*<op>*/ *result, VString *vstring) {
	// append OP_STRING
	Operation op; op.code=OP_STRING;
	*result+=op.cast;

	// append 'vstring'
	*result+=vstring;
}

void PCA(Array/*<op>*/ *result, Array/*<op>*/ *code_array) {
	// append OP_CODE
	Operation op; op.code=OP_CODE;
	*result+=op.cast;

	// append 'vstring'
	*result+=code_array;
}


Array *L(VString *vstring) {
	// empty ops array
	Array *result=N(vstring->pool());

	// append 'vstring' to 'result'
	PVS(result, vstring);

	return result;
}

String *LA2S(Array *literal_string_array, int offset) {
	Operation op;
	op.cast=literal_string_array->get(offset+0);
	if(op.code!=OP_STRING)
		return 0;

	return static_cast<VString *>(literal_string_array->get(offset+1))->get_string();
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
