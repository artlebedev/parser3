/*
  $Id: compile_tools.h,v 1.10 2001/02/22 13:58:43 paf Exp $
*/

#ifndef COMPILE_TOOLS
#define COMPILE_TOOLS

#include "code.h"
#include "pa_types.h"
#include "pa_pool.h"
#include "pa_array.h"
#include "pa_string.h"
#include "pa_vstring.h"

enum lexical_state {
	LS_USER,
	LS_DEF_NAME,
	LS_DEF_PARAMS,
	LS_DEF_LOCALS,
	LS_DEF_COMMENT,
	LS_VAR_NAME_SIMPLE,
	LS_VAR_NAME_CURLY,
	LS_VAR_ROUND,
	LS_VAR_CURLY,
	LS_METHOD_NAME,
	LS_METHOD_ROUND,
	LS_METHOD_CURLY,
	LS_METHOD_AFTER
};
struct parse_control {
	/* input */
	Pool *pool;
	Array *methods;
#ifndef NO_CSTRING_ORIGIN
	char *source;
	char *file;
	int line, col;
#endif
	/* state */
	int pending_state/*=0*/;
	String *string/*=new(pool) String(pool)*/;
	
#define MAX_LEXICAL_STATES 100
	enum lexical_state ls/*=LS_USER*/;
	int sp/*=0*/;
	enum lexical_state stack[MAX_LEXICAL_STATES];
	int brackets_nestages[MAX_LEXICAL_STATES];
	
	/* output: filled input 'methods' and 'error' if any */
	char error[MAX_STRING];
};

/* New array // return empty array */
inline Array/*<op>*/ *N(Pool& pool) {
	return new(pool) Array/*<op>*/(pool);
}

/* Assembler instruction // append ordinary instruction to ops */
inline void OP(Array/*<op>*/ *result, enum OPCODE code) {
	Operation op; op.code=code;
	*result+=op.cast;
}

/* Argument String // append String to ops*/
inline void AS(Array/*<op>*/ *result, String *string) {
	*result+=string;
}
/* Argument Array // append Array to ops */
inline void AA(Array/*<op>*/ *result, Array/*<op>*/ *array) {
	*result+=array;
}
/* Argument Eval_expression // append eval_expression to ops */
inline void AE(Array/*<op>*/ *result, char *eval_expression) {
	*result+=eval_expression;
}

inline void P(Array/*<op>*/ *result, Array *code_array) {
	result->append_array(*code_array);
}


/* Literal // returns array with 
// first: OP_STRING instruction
// second op: string itself
*/
Array *L(VString *vstring);
/* Literal Array to(2) String // return string value from literal array OP+string array */
String *LA2S(Array *literal_string_array, int offset=0);

void push_LS(struct parse_control *pc, lexical_state new_state);
void pop_LS(struct parse_control *pc);

#endif
