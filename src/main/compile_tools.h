/*
  $Id: compile_tools.h,v 1.1 2001/02/20 18:45:52 paf Exp $
*/

#ifndef COMPILE_TOOLS
#define COMPILE_TOOLS

#include "code.h"
#include "pa_types.h"

enum lexical_state {
	LS_USER,
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
	void *pool;
#ifndef NO_CSTRING_ORIGIN
	char *source;
	char *file;
	int line;
#endif
	/* state */
	int pending_state/*=0*/;
	void *string/*=string_create(...)*/;
	
#define MAX_LEXICAL_STATES 100
	enum lexical_state ls/*=LS_USER*/;
	int sp/*=0*/;
	enum lexical_state stack[MAX_LEXICAL_STATES];
	int brackets_nestages[MAX_LEXICAL_STATES];
	
	/* output: Array *  */
	void *result;
};

#ifdef __cplusplus
extern "C" {
#endif
	/* New array // return empty array */
	void *N(void *apool);
	
	/* Assembler instruction // append ordinary instruction to result */
	void A(void **result, enum OPCODE acode);

	/* Assembler parametrized instruction // append instruction; append param */
	void AP(void **result, enum OPCODE acode, void *param);

	/* Literal // returns array with 
		// first: OP_STRING instruction
		// second op: string itself
	*/
	void *L(void *astring);
	/* Literal String // return string value from literal array OP+string array */
	void *LS(void *literal);

	/* aPpend code array // append code_array to result */
	void P(void **result, void *code_array);


	void push_LS(struct parse_control *pc);
	void pop_LS(struct parse_control *pc);

	void *string_create(void *pool);

#ifndef NO_STRING_ORIGIN
#	define CSTRING_APPEND_PARAMS void *astring, char *piece, size_t size, char *file, uint line
#	define CSTRING_APPEND(astring, piece, size, file, line) real_cstring_append(astring, piece, size, file, line)
#else
#	define CSTRING_APPEND_PARAMS void *astring, char *piece, size_t size
#	define CSTRING_APPEND(astring, piece, size, file, line) real_cstring_append(astring, piece, size)
#endif
	void real_cstring_append(CSTRING_APPEND_PARAMS);
	char *string_cstr(void *astring);

	void exception(void *pool, 
		void *atype, void *acode,
		void *aproblem_source, 
		char *acomment);

#ifdef __cplusplus
}
#endif

#endif
