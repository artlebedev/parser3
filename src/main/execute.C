/*
  $Id: execute.C,v 1.11 2001/02/21 17:36:58 paf Exp $
*/

#include "pa_array.h" 
#include "code.h"
#include "pa_request.h"

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

void dump(int level, const Array& ops) {
	int size=ops.size();
	for(int i=0; i<size; i++) {
		int code=reinterpret_cast<int>(ops.quick_get(i));
		printf("%*s%s", level*4, "", opcode_name[code]);

		if(code==OP_STRING) {
			printf(" \"%s\"", static_cast<const String *>(ops.quick_get(++i))->cstr());
		}
		printf("\n");

		if(code==OP_CODE_ARRAY) {
			const Array *local_ops=reinterpret_cast<const Array *>(ops.quick_get(++i));
			dump(level+1, *local_ops);
		}
	}
}

void Request::execute(Array& ops) {
	int size=ops.size();
	for(int i=0; i<size; i++) {
		int code=reinterpret_cast<int>(ops.quick_get(i));
		printf("%s\n", opcode_name[code]);

		if(code==OP_CODE_ARRAY) {
			const Array *local_ops=reinterpret_cast<const Array *>(ops.quick_get(++i));
			//dump(level+1, *local_ops);
		}

		switch(code) {
		case OP_WITH_WRITE: 
			{
			stack.push(wcontext);
			break;
			}

		case OP_STRING:
			{
			String *string=static_cast<String *>(ops.quick_get(++i));
			stack.push(string);
			break;
			}

		case OP_CONSTRUCT:
			{
			Value *value=static_cast<Value *>(stack.pop());
			String *name=static_cast<String *>(stack.pop());
			Value *ncontext=static_cast<Value *>(stack.pop());
			ncontext->put_element(*name, value);
			break;
			}

		case OP_GET_ELEMENT:
			{
			String *name=static_cast<String *>(stack.pop());
			Value *ncontext=static_cast<Value *>(stack.pop());
			Value *value=ncontext->get_element(*name); // name бывает method, тогда выдаЄт new junction(ј¬“ќ¬џ„»—Ћя“№=false, root,self,rcontext,wcontext,code)
			// name бывает им€ junction, тогда или оставл€ет в покое, или вычисл€ет в зависимости от флага ј¬“ќ¬џ„»—Ћя“№
			stack.push(value);
			break;
			}

		default:
			printf("\tTODO\n");
			break;
		}
	}

	return;
	puts("---------------------------");
	dump(0, ops);
	puts("---------------------------");
}
