/** @file
	Parser: compiled code related decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef CODE_H
#define CODE_H

static const char* IDENT_OPCODE_H="$Id: pa_opcode.h,v 1.19 2002/08/01 11:26:47 paf Exp $";

#include "pa_string.h"
#include "pa_array.h"

class Value;
class Array;

///	Compiled operation code
enum OPCODE {
	//@{ 
	/// @name literals
	OP_VALUE,  OP_CURLY_CODE__STORE_PARAM,  OP_EXPR_CODE__STORE_PARAM,
	OP_NESTED_CODE,
	//@}

	//@{
	/// @name actions
	OP_WITH_ROOT,	OP_WITH_SELF,	OP_WITH_READ,	OP_WITH_WRITE,
	OP_GET_CLASS,
	OP_CONSTRUCT_VALUE, OP_CONSTRUCT_EXPR, OP_CURLY_CODE__CONSTRUCT,
	OP_WRITE_VALUE,  OP_WRITE_EXPR_RESULT, OP_STRING__WRITE,
	OP_GET_ELEMENT_OR_OPERATOR, OP_GET_ELEMENT,	OP_GET_ELEMENT__WRITE,
	OP_OBJECT_POOL,	OP_STRING_POOL,
	OP_STORE_PARAM,
	OP_PREPARE_TO_CONSTRUCT_OBJECT, OP_PREPARE_TO_EXPRESSION, 
	OP_CALL, OP_CALL__WRITE,
	//@}

	//@{
	/// @name expression ops: unary
	OP_NEG, OP_INV, OP_NOT, OP_DEF, OP_IN, OP_FEXISTS, OP_DEXISTS,
	//@}

	//@{
	/// @name expression ops: binary
	OP_SUB, OP_ADD, OP_MUL, OP_DIV, OP_MOD, OP_INTDIV,
	OP_BIN_AND, OP_BIN_OR, OP_BIN_XOR,
	OP_LOG_AND, OP_LOG_OR, OP_LOG_XOR,
	OP_NUM_LT, OP_NUM_GT, OP_NUM_LE, OP_NUM_GE, OP_NUM_EQ, OP_NUM_NE,
	OP_STR_LT, OP_STR_GT, OP_STR_LE, OP_STR_GE, OP_STR_EQ, OP_STR_NE,
	OP_IS
	//@}
};

/** 
	Parser source code got compiled into intermediate form of Operation-s, 
	which is executed afterwards.

	It is compiled into Array of Operation-s.
	Each Operation can be either OPCODE or data pointer, 
	following the literal-instruction.
		- OP_VALUE followed by Value*
		- OP_CURLY_CODE__STORE_PARAM followed by Array*
		- OP_EXPR_CODE__STORE_PARAM followed by Array*
		- OP_NESTED_CODE followed by Array*
*/
union Operation {
	void *cast; ///< casting helper 

	OPCODE code; ///< operation code
	Value *value; ///< not an operation, but rather value stored after argumented op
	Array *array; ///< not an operation, but rather code array stored after argumented op
};

#endif
