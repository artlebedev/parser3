/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: code.h,v 1.22 2001/03/16 09:26:42 paf Exp $
*/

#ifndef CODE_H
#define CODE_H

#include "pa_string.h"
#include "pa_array.h"

enum OPCODE {
	// literals
	OP_VALUE,  OP_CODE__STORE_PARAM,

	// actions
	OP_WITH_SELF,	OP_WITH_ROOT,	OP_WITH_READ,	OP_WITH_WRITE,
	OP_GET_CLASS,
	OP_CONSTRUCT_VALUE, OP_CONSTRUCT_EXPR,
	OP_WRITE_VALUE,  OP_WRITE_EXPR_RESULT, OP_STRING__WRITE,
	OP_GET_ELEMENT,	OP_GET_ELEMENT__WRITE,
	OP_CREATE_EWPOOL,	OP_REDUCE_EWPOOL,
	OP_CREATE_RWPOOL,	OP_REDUCE_RWPOOL,
  	OP_CREATE_SWPOOL,	OP_REDUCE_SWPOOL,
	OP_GET_METHOD_FRAME,
	OP_STORE_PARAM,
	OP_CALL,

	// expression ops: unary
	OP_NEG, OP_INV, OP_NOT, OP_DEF, OP_IN, OP_FEXISTS,
	// expression ops: binary
	OP_SUB, OP_ADD, OP_MUL, OP_DIV, OP_MOD,
	OP_BIN_AND, OP_BIN_OR, OP_BIN_XOR,
	OP_LOG_AND, OP_LOG_OR, OP_LOG_XOR,
	OP_NUM_LT, OP_NUM_GT, OP_NUM_LE, OP_NUM_GE, OP_NUM_EQ, OP_NUM_NE,
	OP_STR_LT, OP_STR_GT, OP_STR_LE, OP_STR_GE, OP_STR_EQ, OP_STR_NE,
	OP_IS
};

union Operation {
	void *cast; // casting helper 

	OPCODE code;
	String *string;
	Array *array;
};

#endif
