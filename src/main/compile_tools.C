/*
  $Id: compile_tools.C,v 1.1 2001/02/20 18:45:52 paf Exp $
*/

#include "compile_tools.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"

void *N(void *apool) {
	Pool& pool=*static_cast<Pool *>(apool);
	return new(pool) Array(pool);
}

void A(void **result, enum OPCODE acode) {
	int code=acode;
	(*static_cast<Array *>(*result))+=reinterpret_cast<Array::Item *>(code);
}

void AP(void **result, enum OPCODE acode, void *param) {
	int code=acode;
	(*static_cast<Array *>(*result))+=reinterpret_cast<Array::Item *>(code);
	(*static_cast<Array *>(*result))+=param;
}

void *L(void *astring) {
	String *string=static_cast<String *>(astring);

	// empty ops array
	void *result=N(&string->pool());

	// append OP_STRING
	int code=OP_STRING;
	*(static_cast<Array *>(result))+=reinterpret_cast<Array::Item *>(code);

	// append 'string'
	*(static_cast<Array *>(result))+=string;

	return result;
}

void *LS(void *literal) {
	return const_cast<void *>(static_cast<Array *>(literal)->get(1));
}

void P(void **result, void *code_array) {
	*(static_cast<Array *>(*result))+=code_array;
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

void *string_create(void *apool) {
	Pool& pool=*static_cast<Pool *>(apool);
	return new(pool) String(pool);
}

void real_cstring_append(CSTRING_APPEND_PARAMS) {
	static_cast<String *>(astring)->APPEND(piece, size, file, line);
}

char *string_cstr(void *astring) {
	return static_cast<String *>(astring)->cstr();
}

/* exception */

void exception(void *pool, 
		void *atype, void *acode,
		void *aproblem_source, 
		char *acomment) {
	static_cast<Pool *>(pool)->exception().raise(
		static_cast<class String *>(atype), 
		static_cast<class String *>(acode), 
		static_cast<class String *>(aproblem_source), 
		acomment);
}
