/*
  $Id: execute.C,v 1.7 2001/02/21 07:41:56 paf Exp $
*/

#include "pa_array.h"
#include "code.h"

#include <stdio.h>

char *opcode_name[]={
	"STRING",
	"CODE_ARRAY",
	"WITH_ROOT",	"WITH_SELF",	"WITH_READ",	"WITH_WRITE",
	"CONSTRUCT",
	"EXPRESSION_EVAL",	"MODIFY_EVAL",
	"WRITE",
	"GET_ELEMENT",	"GET_ELEMENT__WRITE",
	"CREATE_EWPOOL",	"REDUCE_EWPOOL",
	"CREATE_RWPOOL",	"REDUCE_RWPOOL",
	"GET_METHOD_FRAME",
	"CREATE_JUNCTION",
	"STORE_PARAM",
	"CALL"
};

void dump(int level, const Array *ops) {
	if(!ops)
		return;

	int size=ops->size();
	for(int i=0; i<size; i++) {
		int code=reinterpret_cast<int>(ops->raw_get(i));
		printf("%*s%s", level*4, "", opcode_name[code]);

		if(code==OP_STRING) {
			printf(" \"%s\"", static_cast<const String *>(ops->raw_get(++i))->cstr());
		}
		printf("\n");

		if(code==OP_CODE_ARRAY) {
			const Array *local_ops=reinterpret_cast<const Array *>(ops->raw_get(++i));
			dump(level+1, local_ops);
		}
	}
}

void execute(Pool *pool, const Array *ops) {
	if(!ops)
		return;

	puts("---------------------------");
	dump(0, ops);
	puts("---------------------------");
}
