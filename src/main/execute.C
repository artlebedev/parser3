/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: execute.C,v 1.103 2001/03/13 14:28:51 paf Exp $
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
#include "pa_vdouble.h"
#include "pa_vbool.h"
#include "pa_vtable.h"

#include <stdio.h>

#define PUSH(value) stack.push(value)
#define POP() static_cast<Value *>(stack.pop())
#define POP_NAME() static_cast<Value *>(stack.pop())->as_string()


char *opcode_name[]={
	// literals
	"VALUE",  "CODE__STORE_PARAM",

	// actions
	"WITH_SELF",	"WITH_ROOT",	"WITH_READ",	"WITH_WRITE",
	"GET_CLASS",
	"CONSTRUCT_VALUE",  "CONSTRUCT_DOUBLE",
	"WRITE",  "STRING__WRITE",
	"GET_ELEMENT",	"GET_ELEMENT__WRITE",
	"CREATE_EWPOOL",	"REDUCE_EWPOOL",
	"CREATE_RWPOOL",	"REDUCE_RWPOOL",
  	"CREATE_SWPOOL",	"REDUCE_SWPOOL",
	"GET_METHOD_FRAME",
	"STORE_PARAM",
	"CALL",

	// expression ops: unary
	"NEG", "INV", "NOT", "DEF", "IN", "FEXISTS",
	// expression ops: binary
	"SUB", "ADD", "MUL", "DIV", "MOD",
	"BIN_AND", "BIN_OR", "BIN_XOR",
	"LOG_AND", "LOG_OR", "LOG_XOR",
	"NUM_LT", "NUM_GT", "NUM_LE", "NUM_GE", "NUM_EQ", "NUM_NE",
	"STR_LT", "STR_GT", "STR_LE", "STR_GE", "STR_EQ", "STR_NE",
	"IS"
};

void dump(int level, const Array& ops) {
	if(0){
		int size=ops.size();
		//fprintf(stderr, "size=%d\n", size);
		for(int i=0; i<size; i++) {
			Operation op;
			op.cast=ops.quick_get(i);
			fprintf(stderr, "%8X\n", op.cast);
		}
	}

	int size=ops.size();
	//fprintf(stderr, "size=%d\n", size);
	for(int i=0; i<size; i++) {
		Operation op;
		op.cast=ops.quick_get(i);
		fprintf(stderr, "%*s%s", level*4, "", opcode_name[op.code]);

		if(op.code==OP_VALUE || op.code==OP_STRING__WRITE) {
			Value *value=static_cast<Value *>(ops.quick_get(++i));
			fprintf(stderr, " \"%s\" %s", value->get_string()->cstr(), value->type());
		}
		fprintf(stderr, "\n");

		if(op.code==OP_CODE__STORE_PARAM) {
			const Array *local_ops=reinterpret_cast<const Array *>(ops.quick_get(++i));
			dump(level+1, *local_ops);
		}
	}
	fflush(stderr);
}

void Request::execute(const Array& ops) {
	if(1) {
		fputs("source----------------------------\n", stderr);
		dump(0, ops);
		fputs("execution-------------------------\n", stderr);
	}

	int size=ops.size();
	//fprintf(stderr, "size=%d\n", size);
	for(int i=0; i<size; i++) {
		Operation op;
		op.cast=ops.quick_get(i);
		fprintf(stderr, "%d:%s", stack.top_index()+1, opcode_name[op.code]); fflush(stderr);

		switch(op.code) {
		// param in next instruction
		case OP_VALUE:
			{
				Value *value=static_cast<Value *>(ops.quick_get(++i));
				fprintf(stderr, " \"%s\" %s", value->get_string()->cstr(), value->type());
				PUSH(value);
				break;
			}
		case OP_CODE__STORE_PARAM:
			{
				VMethodFrame *frame=static_cast<VMethodFrame *>(stack.top_value());
				// code
				const Array *local_ops=reinterpret_cast<const Array *>(ops.quick_get(++i));
				fprintf(stderr, " (%d)\n", local_ops->size());
				dump(1, *local_ops);
				
				Junction& j=*NEW Junction(pool(), 
					*self, 0, 0,
					root, frame, frame, local_ops);
				
				Value *value=NEW VJunction(j);

				// store param
				frame->store_param(frame->name(), value);
				break;
			}
		case OP_GET_CLASS:
			{
				// maybe the do ^class:method[] call, remember the fact
				wcontext->set_somebody_entered_some_class();

				const String& name=POP_NAME();
				VClass *vclass=static_cast<VClass *>(classes().get(name));
				if(!vclass) 
					THROW(0,0,
						&name,
						": undefined class"); 

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
			
		// OTHER ACTIONS BUT WITHs
		case OP_CONSTRUCT_VALUE:
			{
				Value *value=POP();
				const String& name=POP_NAME();
				Value *ncontext=POP();
				ncontext->put_element(name, value);
				value->set_name(name);
				break;
			}
		case OP_CONSTRUCT_EXPR:
			{
				Value *value=POP();
				const String& name=POP_NAME();
				Value *ncontext=POP();
				ncontext->put_element(name, value->get_expr_result());
				value->set_name(name);
				break;
			}
		case OP_WRITE:
			{
				Value *value=POP();
				write_assign_lang(*value);
				break;
			}
		case OP_STRING__WRITE:
			{
				VString *vstring=static_cast<VString *>(ops.quick_get(++i));
				fprintf(stderr, " \"%s\"", vstring->value().cstr());
				write(vstring->value());
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
				write_assign_lang(*value);
				break;
			}


		case OP_CREATE_EWPOOL:
			{
				PUSH(wcontext);
				wcontext=NEW WWrapper(pool(), 0 /* empty */, true /* constructing */);
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
				wcontext=NEW WWrapper(pool(), ncontext, false /* not constructing */);
				break;
			}
		case OP_REDUCE_RWPOOL:
			{
				const String *string=wcontext->get_string();
				Value *value;
				if(string)
					value=NEW VString(*string);
				else
					value=NEW VUnknown(pool());
				wcontext=static_cast<WContext *>(POP());
				rcontext=POP();
				PUSH(value);
				break;
			}
		case OP_CREATE_SWPOOL:
			{
				PUSH(wcontext);
				wcontext=NEW WWrapper(pool(), 0 /* empty */, false /* not constructing */);
				break;
			}
		case OP_REDUCE_SWPOOL:
			{
				// from "$a $b" part of expression taking only string value,
				// ignoring any other content of wcontext
				const String *string=wcontext->get_string();
				Value *value;
				if(string)
					value=NEW VString(*string);
				else
					NEW VUnknown(pool());
				wcontext=static_cast<WContext *>(POP());
				PUSH(value);
				break;
			}

		// CALL
		case OP_GET_METHOD_FRAME:
			{
				Value *value=POP();
				// info: this one's always method-junction, not a code-junction
				Junction *junction=value->get_junction();
				if(!junction)
					THROW(0,0,
						&value->name(),
						"(%s) uncallable, must be method or junction",
							value->type()); 

				VMethodFrame *frame=NEW VMethodFrame(pool(), *junction);
				//frame->set_name(junction->self.name());
				frame->set_name(value->name());
				PUSH(frame);
				break;
			}
		case OP_STORE_PARAM:
			{
				Value *value=POP();
				VMethodFrame *frame=static_cast<VMethodFrame *>(stack.top_value());
				frame->store_param(frame->name(), value);
				break;
			}

		case OP_CALL:
			{
				fprintf(stderr, "->\n");
				VMethodFrame *frame=static_cast<VMethodFrame *>(POP());
				frame->fill_unspecified_params();
				PUSH(self);  
				PUSH(root);  
				PUSH(rcontext);  
				PUSH(wcontext); 
				
				VStateless_class *called_class=frame->junction.self.get_class();
				// is context object or class & is it my class or my parent's class?
				VStateless_class *read_class=rcontext->get_class();
				if(read_class && read_class->is_or_derived_from(*called_class)) // yes
					self=rcontext; // class dynamic call
				else // no, not me or relative of mine (total stranger)
					if(
						wcontext->constructing() && // constructing?
						wcontext->somebody_entered_some_class()) { // ^class:method[..]?
						// yes, this is a constructor call
						if(called_class->name()==TABLE_CLASS_NAME)
							self=NEW VTable(pool());
						else if(called_class->name()==ENV_CLASS_NAME)
							self=NEW VEnv(pool());
						else
							self=NEW VObject(pool(), *called_class);
						frame->write(*self, 
							String::Untaint_lang::NO  // not used, always an object, not string
						);
					} else 
						self=&frame->junction.self; // no, static or simple dynamic call

				frame->set_self(*self);
				root=rcontext=wcontext=frame;
				{
					// take object or class from any wrappers
					// and substitute class alias to the class they are called AS
					Temp_alias temp_alias(*self->get_aliased(), *frame->junction.vclass);

					const Method& method=*frame->junction.method;
					if(method.native_code) { // native code?
						method.check_actual_numbered_params(
							frame->name(), frame->numbered_params());
						(*method.native_code)(*this, 
							frame->name(), frame->numbered_params()); // execute it
					} else // parser code
						execute(*method.parser_code); // execute it
				}
				Value *value=wcontext->result();

				wcontext=static_cast<WContext *>(POP());  
				rcontext=POP();  
				root=POP();  
				self=static_cast<VAliased *>(POP());

				PUSH(value);
				fprintf(stderr, "<-returned");
				break;
			}

		// expression ops: unary
		case OP_NEG:
			{
				Value *operand=POP();
				Value *value=NEW VDouble(pool(), -operand->get_double());
				PUSH(value);
				break;
			}
		case OP_INV:
			{
				Value *operand=POP();
				Value *value=NEW VDouble(pool(), ~(int)operand->get_double());
				PUSH(value);
				break;
			}
		case OP_NOT:
			{
				Value *operand=POP();
				Value *value=NEW VBool(pool(), !operand->get_bool());
				PUSH(value);
				break;
			}
		case OP_DEF:
			{
				Value *operand=POP();
				Value *value=NEW VBool(pool(), operand->get_defined());
				PUSH(value);
				break;
			}
		case OP_IN:
			{
				Value *operand=POP();
				Value *value=NEW VBool(pool(), true/*TODO*/);
				PUSH(value);
				break;
			}
		case OP_FEXISTS:
			{
				Value *operand=POP();
				Value *value=NEW VBool(pool(), true/*TODO*/);
				PUSH(value);
				break;
			}

		// expression ops: binary
		case OP_SUB: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VDouble(pool(), a->get_double() - b->get_double());
				PUSH(value);
				break;
			}
		case OP_ADD: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VDouble(pool(), a->get_double() + b->get_double());
				PUSH(value);
				break;
			}
		case OP_MUL: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VDouble(pool(), a->get_double() * b->get_double());
				PUSH(value);
				break;
			}
		case OP_DIV: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VDouble(pool(), a->get_double() / b->get_double());
				PUSH(value);
				break;
			}
		case OP_MOD: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VDouble(pool(), 
					(int)a->get_double() %
					(int)b->get_double());
				PUSH(value);
				break;
			}
		case OP_BIN_AND:
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VDouble(pool(), 
					(int)a->get_double() &
					(int)b->get_double());
				PUSH(value);
				break;
			}
		case OP_BIN_OR:
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VDouble(pool(), 
					(int)a->get_double() |
					(int)b->get_double());
				PUSH(value);
				break;
			}
		case OP_BIN_XOR:
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VDouble(pool(), 
					(int)a->get_double() ^
					(int)b->get_double());
				PUSH(value);
				break;
			}
		case OP_LOG_AND:
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_bool() && b->get_bool());
				PUSH(value);
				break;
			}
		case OP_LOG_OR:
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_bool() || b->get_bool());
				PUSH(value);
				break;
			}
		case OP_LOG_XOR:
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_bool() ^ b->get_bool());
				PUSH(value);
				break;
			}
		case OP_NUM_LT: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_double() < b->get_double());
				PUSH(value);
				break;
			}
		case OP_NUM_GT: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_double() > b->get_double());
				PUSH(value);
				break;
			}
		case OP_NUM_LE: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_double() <= b->get_double());
				PUSH(value);
				break;
			}
		case OP_NUM_GE: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_double() >= b->get_double());
				PUSH(value);
				break;
			}
		case OP_NUM_EQ: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_double() == b->get_double());
				PUSH(value);
				break;
			}
		case OP_NUM_NE: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->get_double() != b->get_double());
				PUSH(value);
				break;
			}
		case OP_STR_LT: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->as_string() < b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_GT: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->as_string() > b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_LE: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->as_string() <= b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_GE: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->as_string() >= b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_EQ: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->as_string() == b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_NE: 
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), a->as_string() != b->as_string());
				PUSH(value);
				break;
			}
		case OP_IS:
			{
				Value *b=POP();  Value *a=POP();
				Value *value=NEW VBool(pool(), b->as_string() == a->type());
				PUSH(value);
				break;
			}

		default:
			THROW(0,0,
				0,
				"unhandled '%s' opcode", opcode_name[op.code]); 
		}
		fprintf(stderr, "\n");
	}
}

Value *Request::get_element() {
	const String& name=POP_NAME();
	Value *ncontext=POP();
	Value *value=ncontext->get_element(name);

	if(value)
		value=&process(*value, &name); // process possible code-junction
	else {
		value=NEW VUnknown(pool());
		value->set_name(name);
	}

	return value;
}

Value& Request::process(Value& value, const String *name, bool intercept_string) {
	// intercept_string:
	//	true:
	//		they want result=string value, 
	//		possible object result goes to wcontext
	//	false:
	//		they want any result[string|object]
	//		nothing goes to wcontext.
	//		used in (expression) params evaluation

	Value *result;
	Junction *junction=value.get_junction();
	if(junction && junction->code) { // is it a code-junction?
		// process it
		fprintf(stderr, "ja->\n");
		PUSH(self);  
		PUSH(root);  
		PUSH(rcontext);  
		PUSH(wcontext);
		
		WContext *frame;
		if(intercept_string) {
			// almost plain wwrapper about junction wcontext, 
			// BUT intercepts string writes
			frame=NEW VCodeFrame(pool(), *junction->wcontext);  
		} else {
			// plain wwrapper
			frame=NEW WWrapper(pool(), 0 /* empty */, false /*not constructing*/);
		}
		
		wcontext=frame;
		self=&junction->self;
		root=junction->root;
		rcontext=junction->rcontext;
		execute(*junction->code);
		if(intercept_string) {
			// CodeFrame soul:
			//   string writes were intercepted
			//   returning them as the result of getting code-junction
			result=NEW VString(*frame->get_string());
		} else 
			result=frame->result();
		
		wcontext=static_cast<WContext *>(POP());  
		rcontext=POP();  
		root=POP();  
		self=static_cast<VAliased *>(POP());
		
		fprintf(stderr, "<-ja returned");
	} else
		result=&value;

	if(name)
		result->set_name(*name);
	return *result;
}

char *Request::execute_method(Value& aself, const Method& method, bool return_cstr) {
	PUSH(self);  
	PUSH(root);  
	PUSH(rcontext);  
	PUSH(wcontext);
	
	// initialize contexts
	root=rcontext=self=&aself;
	wcontext=NEW WWrapper(pool(), &aself, false /* not constructing */);
	
	// execute!	
	execute(*method.parser_code);
	
	// result
	char *result;
	if(return_cstr)
		result=wcontext->get_string()->cstr(); // chars
	else
		result=0; // ignore result
	
	wcontext=static_cast<WContext *>(POP());  
	rcontext=POP();  
	root=POP();  
	self=static_cast<VAliased *>(POP());
	
	// return
	return result;
}

char *Request::execute_method(Value& aself, 
							  const String& method_name, bool return_cstr) {
	if(Value *value=aself.get_element(method_name))
		if(Junction *junction=value->get_junction())
			if(const Method *method=junction->method) 
				return execute_method(aself, *method, return_cstr);

	return 0;
}
