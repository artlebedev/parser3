/*
  $Id: execute.C,v 1.41 2001/02/25 10:11:50 paf Exp $
*/

#include "pa_array.h" 
#include "code.h"
#include "pa_request.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vunknown.h"
#include "pa_vcframe.h"
#include "pa_vmframe.h"
#include "pa_vobject.h"

#include <stdio.h>

#define PUSH(value) stack.push(value)
#define POP() static_cast<Value *>(stack.pop())
#define POP_NAME() static_cast<Value *>(stack.pop())->as_string()


char *opcode_name[]={
	"STRING",  "CODE",  "CLASS",
	"WITH_ROOT",	"WITH_SELF",	"WITH_READ",	"WITH_WRITE",
	"CONSTRUCT",
	"EXPRESSION_EVAL",	"MODIFY_EVAL",
	"WRITE",
	"GET_ELEMENT",	"GET_ELEMENT__WRITE",
	"CREATE_EWPOOL",	"REDUCE_EWPOOL",
	"CREATE_RWPOOL",	"REDUCE_RWPOOL",
	"GET_METHOD_FRAME",
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
		if(op.code==OP_CLASS) {
			VClass *vclass=static_cast<VClass *>(ops.quick_get(++i));
			printf(" \"%s\"", vclass->name()->cstr());
		}
		printf("\n");

		if(op.code==OP_CODE) {
			const Array *local_ops=reinterpret_cast<const Array *>(ops.quick_get(++i));
			dump(level+1, *local_ops);
		}
	}
}

void Request::execute(const Array& ops) {
	if(1) {
		puts("source----------------------------");
		dump(0, ops);
		puts("execution-------------------------");
	}

	int size=ops.size();
	//printf("size=%d\n", size);
	for(int i=0; i<size; i++) {
		Operation op;
		op.cast=ops.quick_get(i);
		printf("%d:%s", stack.top(), opcode_name[op.code]);

		switch(op.code) {
			// param in next instruction
		case OP_STRING:
			{
				VString *vstring=static_cast<VString *>(ops.quick_get(++i));
				printf(" \"%s\"", vstring->get_string()->cstr());
				PUSH(vstring);
				break;
			}
		case OP_CODE:
			{
				const Array *local_ops=reinterpret_cast<const Array *>(ops.quick_get(++i));
				printf(" (%d)\n", local_ops->size());
				dump(1, *local_ops);
				Junction& j=*NEW Junction(pool(), 
					*self,
					0,
					root,rcontext,wcontext,local_ops);
				
				Value *value=NEW VJunction(j);
				PUSH(value);
				break;
			}
		case OP_CLASS:
			{
				VClass *vclass=static_cast<VClass *>(ops.quick_get(++i));
				printf(" \"%s\"", vclass->name()->cstr());
		        PUSH(vclass);
				break;
			}
			
			// OP_WITH
		case OP_WITH_SELF: 
			{
				PUSH(self);
				break;
			}
		case OP_WITH_ROOT: 
			{
				PUSH(root);
				break;
			}
		case OP_WITH_READ: 
			{
				PUSH(rcontext);
				break;
			}
		case OP_WITH_WRITE: 
			{
				PUSH(wcontext);
				break;
			}
			
			// ...
		case OP_CONSTRUCT:
			{
				Value *value=POP();
				String& name=POP_NAME();
				Value *ncontext=POP();
				value->set_name(name);
				ncontext->put_element(name, value);
				break;
			}
			// TODO: OP_EXPRESSION_EVAL,	OP_MODIFY_EVAL,
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
				wcontext=NEW WWrapper(pool(), 0 /* empty */);
				break;
			}
		case OP_REDUCE_EWPOOL:
			{
				Value *value=wcontext->result();
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
				wcontext=NEW WWrapper(pool(), ncontext);
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

			// CALL
		case OP_GET_METHOD_FRAME:
			{
				Value *value=POP();
				// [self/class?;params;local;code/native_code](name)
				Junction *junction=value->get_junction();
				if(!junction)
					THROW(0,0,
						value->name(),
						"type is '%s', can not call it (must be method or junction)",
							value->type()); 
				//unless(method) method=operators.get_method[...;code/native_code](name)
				VMethodFrame *frame=NEW VMethodFrame(pool(), *junction);
				PUSH(frame);
				break;
			}
		case OP_STORE_PARAM:
			{
				Value *value=POP();
				VMethodFrame *frame=static_cast<VMethodFrame *>(stack[0]);
				frame->store_param(value);
				break;
			}

		case OP_CALL:
			{
				printf("->\n");
				VMethodFrame *frame=static_cast<VMethodFrame *>(POP());
				frame->fill_unspecified_params();
				PUSH(self);  PUSH(root);  PUSH(rcontext);  PUSH(wcontext); 

				VClass *called_class=frame->junction.self.get_class();
				// variable already constructed?
				if(wcontext->get_class()) { // yes
					// static or dynamic call
					self=&frame->junction.self;
				} else { // no
					// constructor call: $some(^class:method(..))
					self=NEW VObject(pool(), *called_class);
					frame->write(self); 
				}
				frame->set_self(self);

				root=rcontext=wcontext=frame;
				execute(frame->junction.method->code);
				Value *value=wcontext->result();
				wcontext=static_cast<WContext *>(POP());  rcontext=POP();  root=POP();  self=POP();
				wcontext->write(value);
				printf("<-returned");
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
	String& name=POP_NAME();
	Value *ncontext=POP();
	Value *value=ncontext->get_element(name);

	if(value) {
		Junction *junction=value->get_junction();
		if(junction && junction->code) { // is it a code-junction?
			// autocalc it
			printf("ja->\n");
			PUSH(self);  PUSH(root);  PUSH(rcontext);  PUSH(wcontext);
			// almost plain wwrapper about junction wcontext, 
			// BUT intercepts string writes
			VCodeFrame frame(pool(), *junction->wcontext);  wcontext=&frame;
			self=&junction->self;
			root=junction->root;
			rcontext=junction->rcontext;
			execute(*junction->code);
			// CodeFrame soul:
			//   string writes were intercepted
			//   returning them as the result of getting code-junction
			value=NEW VString(frame.get_string());
			wcontext=static_cast<WContext *>(POP());  rcontext=POP();  root=POP();  self=POP();
			printf("<-ja returned");
		}
	} else {
		value=NEW VUnknown(pool());
		value->set_name(name);
	}
	return value;
}
