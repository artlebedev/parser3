/*
  $Id: execute.C,v 1.28 2001/02/23 14:18:27 paf Exp $
*/

#include "pa_array.h" 
#include "code.h"
#include "pa_request.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vunknown.h"
#include "pa_vframe.h"

#include <stdio.h>

#define PUSH(value) stack.push(value)
#define POP() static_cast<Value *>(stack.pop())
#define POP_SR() static_cast<Value *>(stack.pop())->as_string()


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
	if(0){
		int size=ops.size();
		//printf("size=%d\n", size);
		for(int i=0; i<size; i++) {
			Operation op;
			op.cast=ops.quick_get(i);
			printf("%8X\n", op.cast);
		}
	}

	int size=ops.size();
	//printf("size=%d\n", size);
	for(int i=0; i<size; i++) {
		Operation op;
		op.cast=ops.quick_get(i);
		printf("%*s%s", level*4, "", opcode_name[op.code]);

		if(op.code==OP_STRING) {
			VString *vstring=static_cast<VString *>(ops.quick_get(++i));
			printf(" \"%s\"", vstring->get_string()->cstr());
		}
		printf("\n");

		if(op.code==OP_CODE_ARRAY) {
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
	//printf("size=%d\n", size);
	for(int i=0; i<size; i++) {
		Operation op;
		op.cast=ops.quick_get(i);
		printf("%d:%s", stack.top(), opcode_name[op.code]);

		if(op.code==OP_CODE_ARRAY) {
			const Array *local_ops=reinterpret_cast<const Array *>(ops.quick_get(++i));
			//dump(level+1, *local_ops);
		}
		
		switch(op.code) {
		case OP_WITH_WRITE: 
			{
				PUSH(wcontext);
				break;
			}
		case OP_WITH_READ: 
			{
				PUSH(rcontext);
				break;
			}
		case OP_WITH_ROOT: 
			{
				PUSH(root);
				break;
			}
		case OP_WITH_SELF: 
			{
				PUSH(self);
				break;
			}
			
		case OP_STRING:
			{
				VString *vstring=static_cast<VString *>(ops.quick_get(++i));
				printf(" \"%s\"", vstring->get_string()->cstr());
				PUSH(vstring);
				break;
			}
			
		case OP_CONSTRUCT:
			{
				Value *value=POP();
				String& name=POP_SR();
				Value *ncontext=POP();
				value->set_name(name);
				ncontext->put_element(name, value);
				break;
			}
			
		case OP_WRITE:
			{
				Value *value=POP();
				wcontext->write(value);
				break;
			}
			
		case OP_GET_ELEMENT:
			{
				Value *value=get_element();
				PUSH(value);
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
				PUSH(wcontext);
				wcontext=NEW WContext(pool(), 0 /* empty */);
				break;
			}
		case OP_REDUCE_EWPOOL:
			{
				Value *value=wcontext->value();
				wcontext=static_cast<WContext *>(POP());
				PUSH(value);
				break;
			}
			
		case OP_CREATE_RWPOOL:
			{
				Value *ncontext=POP();
				PUSH(rcontext);
				rcontext=ncontext;
				PUSH(wcontext);
				wcontext=NEW WContext(pool(), ncontext);
				break;
			}
		case OP_REDUCE_RWPOOL:
			{
				String *string=wcontext->get_string();
				Value *value=NEW VString(string);
				wcontext=static_cast<WContext *>(POP());
				rcontext=POP();
				PUSH(value);
				break;
			}

		case OP_GET_METHOD_FRAME:
			{
				String& name=POP_SR();  // это бывает junction, не name
				Value *ncontext=static_cast<Value *>(stack[0]);
				// [self/class?;params;local;code/native_code](name)
				Method *method=ncontext->get_method(name);
				if(!method)
					THROW(0,0,
						&name,
						"method not found in %s", ncontext->name()->cstr());
				//unless(method) method=operators.get_method[...;code/native_code](name)
				VFrame *frame=NEW VFrame(pool(), *method);
				PUSH(frame);
				break;
			}
		case OP_STORE_PARAM:
			{
				Value *value=POP();
				VFrame *frame=static_cast<VFrame *>(stack[0]);
				frame->store_param(value);
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
	String& name=POP_SR();
	Value *ncontext=POP();
	Value *value=ncontext->get_element(name); // name бывает method, тогда выдаЄт new junction(ј¬“ќ¬џ„»—Ћя“№=false, root,self,rcontext,wcontext,code)
	// name бывает им€ junction, тогда или оставл€ет в покое, или вычисл€ет в зависимости от флага ј¬“ќ¬џ„»—Ћя“№

	if(!value) {
		value=NEW VUnknown(pool());
		value->set_name(name);
	}
	return value;
}