/*
  $Id: code.h,v 1.8 2001/02/24 14:20:51 paf Exp $
*/

#ifndef CODE_H
#define CODE_H

#include "pa_string.h"
#include "pa_array.h"

enum OPCODE {
	OP_STRING,  OP_CODE,
	OP_WITH_SELF,	OP_WITH_ROOT,	OP_WITH_READ,	OP_WITH_WRITE,
	OP_CONSTRUCT,
	OP_EXPRESSION_EVAL,	OP_MODIFY_EVAL,
	OP_WRITE,
	OP_GET_ELEMENT,	OP_GET_ELEMENT__WRITE,
	OP_CREATE_EWPOOL,	OP_REDUCE_EWPOOL,
	OP_CREATE_RWPOOL,	OP_REDUCE_RWPOOL,
	OP_GET_METHOD_FRAME,
	OP_STORE_PARAM,
	OP_CALL
};

union Operation {
	void *cast; // casting helper 

	OPCODE code;
	String *string;
	Array *array;
};

#endif
