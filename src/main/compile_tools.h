/** @file
	Parser: compiler support helper functions decls.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef COMPILE_TOOLS
#define COMPILE_TOOLS

static const char * const IDENT_COMPILE_TOOLS_H="$Date: 2004/02/11 15:33:15 $";

#include "pa_opcode.h"
#include "pa_types.h"
#include "pa_vstring.h"
#include "pa_request.h"

/// used to track source column number
#define TAB_SIZE 8

enum lexical_state {
	LS_USER, LS_NAME_SQUARE_PART,
	LS_USER_COMMENT,
	LS_DEF_NAME,
	LS_DEF_PARAMS,
	LS_DEF_LOCALS,
	LS_DEF_COMMENT,
	LS_DEF_SPECIAL_BODY,
	LS_EXPRESSION_STRING_QUOTED,
	LS_EXPRESSION_STRING_APOSTROFED,
	LS_EXPRESSION_VAR_NAME_WITH_COLON, LS_EXPRESSION_VAR_NAME_WITHOUT_COLON,
	LS_EXPRESSION_COMMENT,
	LS_VAR_NAME_SIMPLE_WITH_COLON, LS_VAR_NAME_SIMPLE_WITHOUT_COLON,
	LS_VAR_NAME_CURLY,
	LS_VAR_ROUND,
	LS_VAR_SQUARE,
	LS_VAR_CURLY,
	LS_METHOD_NAME,
	LS_METHOD_SQUARE,
	LS_METHOD_CURLY,
	LS_METHOD_ROUND,
	LS_METHOD_AFTER
};

struct Pos {
	uint line;
	uint col;
	//Pos(uint aline, uint acol): line(aline), col(acol) {}
	Pos(): line(0), col(0) {}

	void clear() { line=col=0; }
	operator bool() { return col!=0; }
};

/// compiler status
class Parse_control {
	const String* main_alias;
	uint last_line_end_col;
public:
	const String& alias_method(const String& name);
	//@{
	/// @name input
	Request& request;
	VStateless_class* cclass;
	const char* source;
	uint file_no;
	Pos pos;
	//@}
	//@{
	/// @name state; initially
	bool trim_bof;
	int pending_state; ///< i=0
	String::Body string; ///< lexical string accumulator
	Pos string_start;
	
#define MAX_LEXICAL_STATES 100
	enum lexical_state ls; ///< =LS_USER;
	int ls_sp; ///< =0
	enum lexical_state ls_stack[MAX_LEXICAL_STATES];
	int brackets_nestages[MAX_LEXICAL_STATES]; ///< brackets nestage on each state

	bool in_call_value;
	//@}
	
	/// output: filled input 'methods' and 'error' if any
	char error[MAX_STRING];

	Parse_control(Request& arequest, 
		VStateless_class* aclass,
		const char* asource, const String* amain_alias, 
		uint afile_no):
		main_alias(amain_alias),
		last_line_end_col(0),

		request(arequest), // input 

		// we were told the class to compile to?
		cclass(aclass), // until changed with @CLASS would consider operators loading
		source(asource), 
		file_no(afile_no),

		// initialize state
		trim_bof(true),
		pending_state(0),
		ls(LS_USER),
		ls_sp(0),
		in_call_value(false) {}

	void pos_next_line() {
		pos.line++;
		last_line_end_col=pos.col;
		pos.col=0;
	}
	void pos_next_c(char c) {
		if(c=='\t')
			pos.col=(pos.col+TAB_SIZE)&~(TAB_SIZE-1);
		else
			pos.col++;
	}
	/// not precise in case of \t in the middle of the text
	void pos_prev_c() {
		if(pos.col==0) {
			--pos.line;  pos.col=last_line_end_col;
		} else
			--pos.col;
	}
	void ungetc() {
		source--;
		pos_prev_c();
	}
};

/// New array // return empty array
inline ArrayOperation* N() {
	return new ArrayOperation;
}

/// Assembler instruction // append ordinary instruction to ops
inline void O(ArrayOperation& result, OPCODE code) {
	result+=Operation(code);
}

/// aPpend 'code_array' to 'result'
inline void P(ArrayOperation& result, ArrayOperation& code_array) {
	result.append(code_array);
}
/// aPpend part of 'code_array', starting from offset, to 'result'
inline void P(ArrayOperation& result, ArrayOperation& code_array, int offset) {
	result.append(code_array, offset);
}

/// append cOde Array
inline void OA(ArrayOperation& result, OPCODE code, ArrayOperation* code_array) {
	result+=Operation(code); // append OP_CODE
	result+=Operation(code_array); // append 'code_array'
}

/**
	Value Literal // returns array with 
	- first op: OP_VALUE instruction
	- second op: origin (debug information)
	- third op: string itself
*/
inline ArrayOperation* VL(Value* value, uint file_no, uint line, uint col) {
	// empty ops array
	ArrayOperation& result=*N();

	// append 'value' to 'result'
	result+=Operation(OP_VALUE);
	result+=Operation(file_no, line, col); // append origin
	result+=Operation(value); // append 'value'

	return &result;
}

/// Literal Array to(2) Value @return Value from literal Array OP+origin+Value
Value* LA2V(ArrayOperation& literal_string_array, int offset=0);
/// Literal Array to(2) String  @return String value from literal Array OP+origin+String array
inline const String* LA2S(ArrayOperation& literal_string_array, int offset=0) {
	if(Value* value=LA2V(literal_string_array, offset))
		return value->get_string();
	return 0;
}

inline void change_string_literal_to_write_string_literal(ArrayOperation& literal_string_array) {
	literal_string_array.put(0, OP_STRING__WRITE);
}


void change_string_literal_to_double_literal(ArrayOperation& literal_string_array);

void change_string_literal_value(ArrayOperation& literal_string_array, const String& new_value);

void changetail_or_append(ArrayOperation& opcodes, 
						  OPCODE find, bool with_argument, OPCODE replace, OPCODE notfound);


void push_LS(Parse_control& pc, lexical_state new_state);
void pop_LS(Parse_control& pc);

#endif
