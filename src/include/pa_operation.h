/** @file
	Parser: compiled code related decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef OPERATION_H
#define OPERATION_H

#define IDENT_PA_OPERATION_H "$Id: pa_operation.h,v 1.11 2020/12/15 17:10:31 moko Exp $"

#include "pa_array.h"
#include "pa_opcode.h"
#include "pa_value.h"

// forwards

union Operation;
typedef Array<Operation> ArrayOperation;

/** 
	Parser source code got compiled into intermediate form of Operation-s, 
	which are executed afterwards.

	It is compiled into Array of Operation-s.
	Each Operation can be either OPCODE or data pointer, 
	following the literal-instruction.
		- OP_VALUE followed by Origin, followed by Value*
		- OP_CURLY_CODE__STORE_PARAM followed by ArrayOperation*
		- OP_EXPR_CODE__STORE_PARAM followed by ArrayOperation*
		- OP_NESTED_CODE followed by ArrayOperation*
*/
union Operation {
	struct Origin {
		uint file_no:8; ///< file number (max: 255): index in Request::file_list
		uint line:8+8; ///< line number (max: 64535)
		uint col:8; ///< column number (max: 255)

		static Origin create(uint afile_no, uint aline, uint acol) {
			Origin result={afile_no, aline, acol};
			return result;
		}
	};

	OP::OPCODE code; ///< operation code
	Origin origin; ///< not an operation, but rather debug information: [OP_VALUE; debug_info; Value*]
	Value* value; ///< not an operation, but rather value stored after argumented op
	ArrayOperation* ops; ///< not an operation, but rather code array stored after argumented op

	/// needed to fill unused Array entries
	Operation() {}
	Operation(OP::OPCODE acode): code(acode) {}
	Operation(uint afile_no, uint aline, uint acol): 
		origin(Origin::create(afile_no, aline, acol)) {}
	Operation(Value* avalue): value(avalue) {}
	Operation(ArrayOperation* aops): ops(aops) {}	
};

// defines

#define OPERATIONS_PER_OPVALUE 3

#endif
