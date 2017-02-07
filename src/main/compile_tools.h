/** @file
	Parser: compiler support helper functions decls.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef COMPILE_TOOLS
#define COMPILE_TOOLS

#define IDENT_COMPILE_TOOLS_H "$Id: compile_tools.h,v 1.113 2017/02/07 22:00:40 moko Exp $"

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
	int line;
	int col;
	Pos(int aline, int acol): line(aline), col(acol) {}
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
	VStateless_class* cclass_new;
	ArrayClass* cclasses;
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
	bool explicit_result;
	bool append;
	//@}
	
	/// output: filled input 'methods' and 'error' if any
	char error[MAX_STRING];

	Parse_control(Request& arequest, 
		VStateless_class* aclass,
		const char* asource, const String* amain_alias, 
		uint afile_no,
		int line_no_offset):
		main_alias(amain_alias),
		last_line_end_col(0),

		request(arequest), // input 

		// we were told the class to compile to?
		cclass(aclass), // until changed with @CLASS would consider operators loading
		cclass_new(0), 
		cclasses(new ArrayClass(1)),
		source(asource), 
		file_no(afile_no),
		pos(line_no_offset, 0),

		// initialize state
		trim_bof(true),
		pending_state(0),
		ls(LS_USER),
		ls_sp(0),
		in_call_value(false),
		explicit_result(false),
		append(false) {

		*cclasses+=aclass;
	}

	/// true if exception should be rised
	bool class_add(){
		if(cclass_new){
			cclass=cclass_new;
			*cclasses+=cclass;
			cclass_new=0;
			append=false;
			// append to request's classes
			if(!request.allow_class_replace)
				return request.classes().put_dont_replace(cclass->type(), cclass) != 0;
			request.classes().put(cclass->type(), cclass);
		}
		return false;
	}

	VStateless_class* get_existed_class(VStateless_class* aclass){
		// checking existence of the class during processing @OPTIONS\npartial
		// can't use get_class because it will call @autouse[] if the class wasn't loaded
		if(aclass)
			return request.classes().get(aclass->type());
		return 0;
	}

	bool reuse_existed_class(VStateless_class* aclass){
		if(aclass->is_partial()){
			cclass=aclass;
			cclass_new=0;
			append=true;
			return true;
		} else {
			return false;
		}
	}

	void set_all_vars_local(){
		(cclass_new ? cclass_new : cclass)->set_all_vars_local();
	}

	void set_methods_call_type(Method::Call_type call_type){
		(cclass_new ? cclass_new : cclass)->set_methods_call_type(call_type);
	}

	Method::Call_type get_methods_call_type(){
		return (cclass_new ? cclass_new : cclass)->get_methods_call_type();
	}

	void pos_next_line() {
		pos.line++;
		last_line_end_col=pos.col;
		pos.col=0;
	}
	void pos_next_c(int c) {
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
inline void O(ArrayOperation& result, OP::OPCODE code) {
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

/// aPpend part of 'code_array', starting from offset, to 'result'
inline void P(ArrayOperation& result, ArrayOperation& code_array, int offset, int limit) {
	result.append(code_array, offset, limit);
}

/// append cOde Array
inline void OA(ArrayOperation& result, ArrayOperation* code_array) {
	result+=Operation(code_array); // append 'code_array'
}

inline void OA(ArrayOperation& result, OP::OPCODE code, ArrayOperation* code_array) {
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
	result+=Operation(OP::OP_VALUE);
	result+=Operation(file_no, line, col); // append origin
	result+=Operation(value); // append 'value'

	return &result;
}

/// Literal Array to(2) Value @return Value from literal Array OP+origin+Value
Value* LA2V(ArrayOperation& literal_string_array, int offset=0, OP::OPCODE code=OP::OP_VALUE);

/// Literal Array to(2) String  @return String value from literal Array OP+origin+String array
inline const String* LA2S(ArrayOperation& literal_string_array, int offset=0, OP::OPCODE code=OP::OP_VALUE) {
	if(Value* value=LA2V(literal_string_array, offset, code))
		return value->get_string();
	return 0;
}

inline void change_string_literal_to_write_string_literal(ArrayOperation& literal_string_array) {
	literal_string_array.put(0, OP::OP_STRING__WRITE);
}

void maybe_change_string_literal_to_double_literal(ArrayOperation& literal_string_array);

void change_string_literal_value(ArrayOperation& literal_string_array, const String& new_value);

inline bool change(ArrayOperation& opcodes, int pos, OP::OPCODE find, OP::OPCODE replace) {
	if(pos>=0) {
		Operation& op=opcodes.get_ref(pos);
		if(op.code==find) {
			op.code=replace;
			return true;
		}
	}
	return false;
}

inline void change_or_append(ArrayOperation& opcodes, int pos, OP::OPCODE find, OP::OPCODE replace, OP::OPCODE notfound) {
	if(change(opcodes, pos, find, replace))
		return;

	opcodes+=Operation(notfound);
}

bool change_first(ArrayOperation& opcodes, OP::OPCODE find, OP::OPCODE replace);

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
// OP_VALUE+origin+value+OP_GET_ELEMENT+OP_VALUE+origin+value+OP_GET_ELEMENT => OP_GET_OBJECT_ELEMENT+origin+value+origin+value
inline bool maybe_make_get_object_element(ArrayOperation& opcodes, ArrayOperation& diving_code, size_t divine_count){
	if(divine_count<8)
		return false;

	assert(diving_code[0].code==OP::OP_VALUE);
	if(
		diving_code[3].code==OP::OP_GET_ELEMENT
		&& diving_code[4].code==OP::OP_VALUE
		&& diving_code[7].code==OP::OP_GET_ELEMENT
	){
		O(opcodes, OP::OP_GET_OBJECT_ELEMENT);
		P(opcodes, diving_code, 1/*offset*/, 2/*limit*/); // copy first origin+value
		P(opcodes, diving_code, 5, 2); // second origin+value
		if(divine_count>8)
			P(opcodes, diving_code, 8/*offset*/); // tail
		return true;
	}
	return false;
}
#endif


#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
// OP_VALUE+origin+value+OP_GET_ELEMENT+OP_WITH_READ+OP_VALUE+origin+value+OP_GET_ELEMENT+OP_GET_ELEMENT => OP_GET_OBJECT_VAR_ELEMENT+origin+value+origin+value
inline bool maybe_make_get_object_var_element(ArrayOperation& opcodes, ArrayOperation& diving_code, size_t divine_count){
	if(divine_count!=10)
		return false;

	assert(diving_code[0].code==OP::OP_VALUE);
	if(
		diving_code[3].code==OP::OP_GET_ELEMENT
		&& diving_code[4].code==OP::OP_WITH_READ
		&& diving_code[5].code==OP::OP_VALUE
		&& diving_code[8].code==OP::OP_GET_ELEMENT
		&& diving_code[9].code==OP::OP_GET_ELEMENT
	){
		O(opcodes, OP::OP_GET_OBJECT_VAR_ELEMENT);
		P(opcodes, diving_code, 1/*offset*/, 2/*limit*/); // copy first origin+value
		P(opcodes, diving_code, 6, 2); // second origin+value
		return true;
	}
	return false;
}
#endif


bool maybe_make_self(ArrayOperation& opcodes, ArrayOperation& diving_code, size_t divine_count);

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
bool maybe_append_simple_diving_code(ArrayOperation& code, ArrayOperation& diving_code);

bool is_special_element(ArrayOperation& opcodes);
#endif

#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
inline bool maybe_optimize_construct(ArrayOperation& opcodes, ArrayOperation& var_ops, ArrayOperation& expr_ops){
	size_t expr_count=expr_ops.count();
	OP::OPCODE construct_op=expr_ops[expr_count-1].code;
	size_t construct=(construct_op==OP::OP_CONSTRUCT_VALUE)?0x01:(construct_op==OP::OP_CONSTRUCT_EXPR)?0x02:0x00;
	if(construct){
		P(opcodes, expr_ops, 0/*offset*/, expr_count-1/*limit*/); // copy constructor body without CONSTRUCT_(VALUE|EXPR)

		size_t with=0x00;
		switch(var_ops[0].code){
			case OP::OP_WITH_ROOT:
				{
					with=0x10;
					break;
				}
			case OP::OP_WITH_WRITE:
				{
					with=0x20;
					break;
				}
			case OP::OP_WITH_SELF:
				{
					with=0x30;
					break;
				}
			default: break;
		}

		if(with && var_ops[1].code==OP::OP_VALUE && var_ops.count()==4){
			OP::OPCODE code=OP::OP_VALUE; // calm down compiler. will be reassigned for sure.
			switch( with | construct ) {
				case 0x11:
					{
						code=OP::OP_WITH_ROOT__VALUE__CONSTRUCT_VALUE;
						break;
					}
				case 0x12:
					{
						code=OP::OP_WITH_ROOT__VALUE__CONSTRUCT_EXPR;
						break;
					}
				case 0x21:
					{
						code=OP::OP_WITH_WRITE__VALUE__CONSTRUCT_VALUE;
						break;
					}
				case 0x22:
					{
						code=OP::OP_WITH_WRITE__VALUE__CONSTRUCT_EXPR;
						break;
					}
				case 0x31:
					{
						code=OP::OP_WITH_SELF__VALUE__CONSTRUCT_VALUE;
						break;
					}
				case 0x32:
					{
						code=OP::OP_WITH_SELF__VALUE__CONSTRUCT_EXPR;
						break;
					}
			}
			O(opcodes, code);
			P(opcodes, var_ops, 2/*offset*/, 2/*limit*/); // copy origin+value
		} else {
			P(opcodes, var_ops);
			O(opcodes, construct_op);
		}
		return true;
	}
	return false;
}
#endif

Method::Call_type GetMethodCallType(Parse_control& pc, ArrayOperation& literal_string_array);

void push_LS(Parse_control& pc, lexical_state new_state);
void pop_LS(Parse_control& pc);

#endif
