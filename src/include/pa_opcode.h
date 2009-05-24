/** @file
	Parser: compiled code related decls.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef OPCODE_H
#define OPCODE_H

namespace OP {

static const char * const IDENT_OPCODE_H="$Date: 2009/05/24 07:32:09 $";

#define OPTIMIZE_BYTECODE_GET_CLASS
#define OPTIMIZE_BYTECODE_GET_ELEMENT				// $a ^a
#define OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT		// $a.b ^a.b
#define OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT	// $a.$b ^a.$b
//#define OPTIMIZE_BYTECODE_STRING_POOL
#define OPTIMIZE_BYTECODE_CUT_REM_OPERATOR			// cut rem with all params

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
#define OPTIMIZE_BYTECODE_CONSTRUCT					// $a(1), $.a(1), $a[b], $.a[b]
													// $a($b), $.a($b), $a[$b], $.a[$b]
													// $a($b.c), $.a($b.c), $a[$b.c], $.a[$b.c] 
													// $a($b.$c), $.a($b.$c), $a[$b.$c], $.a[$b.$c]
//#define OPTIMIZE_BYTECODE_CALL_CONSTRUCT            // $a(^b[]), $.a(^b[]), $a[^b[]], $.a[^b[]]
#endif

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
#ifdef OPTIMIZE_BYTECODE_GET_CLASS
	OP_VALUE__GET_CLASS,
#else
	OP_GET_CLASS,
#endif
	OP_CONSTRUCT_VALUE, OP_CONSTRUCT_EXPR, OP_CURLY_CODE__CONSTRUCT,
	OP_WRITE_VALUE,	OP_WRITE_EXPR_RESULT,	OP_STRING__WRITE,
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	OP_VALUE__GET_ELEMENT_OR_OPERATOR,
#else
	OP_GET_ELEMENT_OR_OPERATOR,
#endif
	OP_GET_ELEMENT,
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	OP_GET_OBJECT_ELEMENT,				// $a.b & ^a.b
	OP_GET_OBJECT_ELEMENT__WRITE,		// $a.b & ^a.b
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	OP_GET_OBJECT_VAR_ELEMENT,			// $a.$b & ^a.$b
	OP_GET_OBJECT_VAR_ELEMENT__WRITE,	// $a.$b & ^a.$b
#endif
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	OP_VALUE__GET_ELEMENT,
#endif
	OP_GET_ELEMENT__WRITE,
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	OP_VALUE__GET_ELEMENT__WRITE,
#endif
	OP_OBJECT_POOL,	OP_STRING_POOL,
	OP_PREPARE_TO_CONSTRUCT_OBJECT, OP_PREPARE_TO_EXPRESSION, 
	OP_CALL, OP_CALL__WRITE,

#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
	OP_ROOT_CONSTRUCT_EXPR,							// $a(1)
	OP_ROOT_ELEMENT_CONSTRUCT_EXPR,					// $a($b)
	OP_ROOT_OBJECT_ELEMENT_CONSTRUCT_EXPR,			// $a($b.c)
	OP_ROOT_OBJECT_VAR_ELEMENT_CONSTRUCT_EXPR,	// $a($b.$c)


	OP_ROOT_CONSTRUCT_VALUE,						// $a[b]
	OP_ROOT_ELEMENT_CONSTRUCT_VALUE,				// $a[$b]
	OP_ROOT_OBJECT_ELEMENT_CONSTRUCT_VALUE,			// $a[$b.c]
	OP_ROOT_OBJECT_VAR_ELEMENT_CONSTRUCT_VALUE,	// $a[$b.$c]


	OP_WRITE_CONSTRUCT_EXPR,						// $.a(1)
	OP_WRITE_ELEMENT_CONSTRUCT_EXPR,				// $.a($b)
	OP_WRITE_OBJECT_ELEMENT_CONSTRUCT_EXPR,			// $.a($b.c)
	OP_WRITE_OBJECT_VAR_ELEMENT_CONSTRUCT_EXPR,	// $.a($b.$c)


	OP_WRITE_CONSTRUCT_VALUE,						// $.a[b]
	OP_WRITE_ELEMENT_CONSTRUCT_VALUE,				// $.a[$b]
	OP_WRITE_OBJECT_ELEMENT_CONSTRUCT_VALUE,		// $.a[$b.c]
	OP_WRITE_OBJECT_VAR_ELEMENT_CONSTRUCT_VALUE,	// $.a[$b.$c]
#endif

#ifdef OPTIMIZE_BYTECODE_CALL_CONSTRUCT
	OP_ROOT_CALL_CONSTRUCT_EXPR,					// $a(^b[])
	//OP_ROOT_CALL_OBJECT_ELEMENT_CONSTRUCT_EXPR,		// $a(^b.c[])

	OP_ROOT_CALL_CONSTRUCT_VALUE,					// $a[^b[]]
	//OP_ROOT_CALL_OBJECT_ELEMENT_CONSTRUCT_VALUE,	// $a[^b.c[]]

	OP_WRITE_CALL_CONSTRUCT_EXPR,					// $.a(^b[])
	//OP_WRITE_CALL_OBJECT_ELEMENT_CONSTRUCT_EXPR,	// $.a(^b.c[])

	OP_WRITE_CALL_CONSTRUCT_VALUE,					// $.a[^b[]]
	//OP_WRITE_CALL_OBJECT_ELEMENT_CONSTRUCT_VALUE,	// $.a[^b.c[]]
#endif
	//@}

	//@{
	/// @name expression ops: unary
	OP_NEG, OP_INV, OP_NOT, OP_DEF, OP_IN, OP_FEXISTS, OP_DEXISTS,
	//@}

	//@{
	/// @name expression ops: binary
	OP_SUB, OP_ADD, OP_MUL, OP_DIV, OP_MOD, OP_INTDIV,
	OP_BIN_SL, OP_BIN_SR,
	OP_BIN_AND, OP_BIN_OR, OP_BIN_XOR,
	OP_LOG_AND, OP_LOG_OR, OP_LOG_XOR,
	OP_NUM_LT, OP_NUM_GT, OP_NUM_LE, OP_NUM_GE, OP_NUM_EQ, OP_NUM_NE,
	OP_STR_LT, OP_STR_GT, OP_STR_LE, OP_STR_GE, OP_STR_EQ, OP_STR_NE,
	OP_IS
	//@}
};
};

#endif
