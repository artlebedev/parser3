/** @file
	Parser: compiler support helper functions decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile_tools.h,v 1.41 2001/06/28 07:33:38 parser Exp $
*/

#ifndef COMPILE_TOOLS
#define COMPILE_TOOLS

#include "pa_config_includes.h"
#include "pa_opcode.h"
#include "pa_types.h"
#include "pa_vstring.h"
#include "pa_request.h"

enum lexical_state {
	LS_USER,
	LS_COMMENT,
	LS_DEF_NAME,
	LS_DEF_PARAMS,
	LS_DEF_LOCALS,
	LS_DEF_COMMENT,
	LS_DEF_SPECIAL_BODY,
	LS_EXPRESSION_STRING,
	LS_EXPRESSION_VAR_NAME,
	LS_VAR_NAME_SIMPLE,
	LS_VAR_NAME_CURLY,
	LS_VAR_NAME_NO_COLON,
	LS_VAR_ROUND,
	LS_VAR_SQUARE,
	LS_VAR_CURLY,
	LS_METHOD_NAME,
	LS_METHOD_SQUARE,
	LS_METHOD_CURLY,
	LS_METHOD_ROUND,
	LS_METHOD_AFTER
};
/// compiler status
struct parse_control {
	//@{
	/// @name input
	Pool *pool;
	Request *request;
	VStateless_class *cclass;
#ifndef NO_CSTRING_ORIGIN
	const char *source;
	const char *file;
	int line, col;
#endif
	//@}
	//@{
	/// @name state; initially
	int pending_state; ///< i=0
	String *string; ///< =new(pool) String(pool)
	
#define MAX_LEXICAL_STATES 100
	enum lexical_state ls; ///< =LS_USER;
	int sp; ///< =0
	enum lexical_state stack[MAX_LEXICAL_STATES];
	int brackets_nestages[MAX_LEXICAL_STATES]; ///< brackets nestage on each state
	//@}
	
	/// output: filled input 'methods' and 'error' if any
	char error[MAX_STRING];
};

/// New array // return empty array
inline Array/*<Operation>*/ *N(Pool& pool) {
	return new(pool) Array/*<Operation>*/(pool);
}

/// Assembler instruction // append ordinary instruction to ops
inline void O(Array/*<Operation>*/ *result, enum OPCODE code) {
	Operation op; op.code=code;
	*result+=op.cast;
}

/// Argument Eval_expression // append eval_expression to ops
inline void AE(Array/*<Operation>*/ *result, char *eval_expression) {
	*result+=eval_expression;
}

/// aPpend 'code_array' to 'result'
inline void P(Array/*<Operation>*/ *result, Array *code_array) {
	result->append_array(*code_array);
}
/// aPpend part of 'code_array', starting from offset, to 'result'
inline void P(Array/*<Operation>*/ *result, Array *code_array, int offset) {
	result->append_array(*code_array, offset);
}
/// aPpend 'vstring' to 'result'
void PV(Array/*<Operation>*/ *result, VString *vstring);
/// aPpend 'simple Code_Array' to result
void PCA(Array/*<Operation>*/ *result, Array/*<Operation>*/ *code_array);
/// aPpend 'expression Code_Array' to result
void PEA(Array/*<Operation>*/ *result, Array/*<Operation>*/ *code_array);

/**
	Value Literal // returns array with 
	- first: OP_VALUE instruction
	- second op: string itself
*/
Array *VL(Value *value);
/// Literal Array to(2) Value @return Value from literal Array OP+Value
Value *LA2V(Array *literal_string_array, int offset=0);
/// Literal Array to(2) String  @return String value from literal Array OP+String array
inline const String *LA2S(Array *literal_string_array, int offset=0) {
	if(Value *value=LA2V(literal_string_array, offset))
		return value->get_string();
	return 0;
}
void change_string_literal_to_double_literal(Array *literal_string_array);
void change_string_literal_to_write_string_literal(Array *literal_string_array);

void push_LS(parse_control& pc, lexical_state new_state);
void pop_LS(parse_control& pc);

#endif
