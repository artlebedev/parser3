/*
  $Id: execute.C,v 1.20 2001/02/22 14:14:07 paf Exp $
*/

#include "pa_array.h" 
#include "code.h"
#include "pa_request.h"
#include "pa_vstring.h"

#include <stdio.h>


char *opcode_name[]={
	"STRING",  "CODE_ARRAY",
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
			VString *vstring=static_cast<VString *>(ops.quick_get(++i));
			printf(" \"%s\"", vstring->get_string()->cstr());
		}
		printf("\n");

		if(code==OP_CODE_ARRAY) {
			const Array *local_ops=reinterpret_cast<const Array *>(ops.quick_get(++i));
			dump(level+1, *local_ops);
		}
	}
}

void Request::execute(Array& ops) {
	if(0) {
		puts("---------------------------");
		dump(0, ops);
		puts("---------------------------");
	}

	int size=ops.size();
	for(int i=0; i<size; i++) {
		int code=reinterpret_cast<int>(ops.quick_get(i));
		printf("%d:%s", stack.top(), opcode_name[code]);

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
		case OP_WITH_READ: 
			{
				stack.push(rcontext);
				break;
			}
		case OP_WITH_ROOT: 
			{
				stack.push(root);
				break;
			}
		case OP_WITH_SELF: 
			{
				stack.push(self);
				break;
			}
			
		case OP_STRING:
			{
				VString *vstring=static_cast<VString *>(ops.quick_get(++i));
				printf(" \"%s\"", vstring->get_string()->cstr());
				stack.push(vstring);
				break;
			}
			
		case OP_CONSTRUCT:
			{
				Value *value=static_cast<Value *>(stack.pop());
				VString *name=static_cast<VString *>(stack.pop());
				Value *ncontext=static_cast<Value *>(stack.pop());
				ncontext->put_element(*name, value);
				break;
			}
			
		case OP_WRITE:
			{
				Value *value=static_cast<Value *>(stack.pop());
				wcontext->write(value);
				break;
			}
			
		case OP_GET_ELEMENT:
			{
				Value *value=get_element();
				stack.push(value);
				break;
			}

		case OP_GET_ELEMENT__WRITE:
			{
				Value *value=get_element();
				wcontext->write(value);
				break;
			}

		case OP_CREATE_EWPOOL:
			{
				stack.push(wcontext);
				wcontext=NEW WContext(pool(), 0 /* empty */);
				break;
			}
		case OP_REDUCE_EWPOOL:
			{
				Value *value=wcontext->value();
				wcontext=static_cast<WContext *>(stack.pop());
				stack.push(value);
				break;
			}
			
		default:
			printf("\tTODO");
			break;
		}
		printf("\n");
	}
}

Value *Request::get_element() {
	VString *name=static_cast<VString *>(stack.pop());
	Value *ncontext=static_cast<Value *>(stack.pop());
	Value *value=ncontext->get_element(*name); // name бывает method, тогда выдаЄт new junction(ј¬“ќ¬џ„»—Ћя“№=false, root,self,rcontext,wcontext,code)
	// name бывает им€ junction, тогда или оставл€ет в покое, или вычисл€ет в зависимости от флага ј¬“ќ¬џ„»—Ћя“№
	return value;
}