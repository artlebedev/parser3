/** @file
	Parser: executor part of request class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: execute.C,v 1.211 2002/01/31 12:40:35 paf Exp $
*/

#include "pa_opcode.h"
#include "pa_array.h" 
#include "pa_request.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"
#include "pa_vcode_frame.h"
#include "pa_vmethod_frame.h"
#include "pa_vobject.h"
#include "pa_vdouble.h"
#include "pa_vbool.h"
#include "pa_vtable.h"
#include "pa_vfile.h"
#include "pa_vimage.h"
#include "pa_wwrapper.h"

//#define DEBUG_EXECUTE

const uint ANTI_ENDLESS_EXECUTE_RECOURSION=500;

#ifdef DEBUG_EXECUTE
char *opcode_name[]={
	// literals
	"VALUE",  "CURLY_CODE__STORE_PARAM",  "EXPR_CODE__STORE_PARAM",
	"NESTED_CODE",

	// actions
	"WITH_ROOT",	"WITH_SELF",	"WITH_READ",	"WITH_WRITE",
	"GET_CLASS",
	"CONSTRUCT_VALUE", "CONSTRUCT_EXPR", "CURLY_CODE__CONSTRUCT",
	"WRITE_VALUE",  "WRITE_EXPR_RESULT",  "STRING__WRITE",
	"GET_ELEMENT",	"GET_ELEMENT__WRITE",
	"CREATE_EWPOOL",	"REDUCE_EWPOOL",
  	"CREATE_SWPOOL",	"REDUCE_SWPOOL",
	"GET_METHOD_FRAME",
	"STORE_PARAM",
	"PREPARE_TO_CONSTRUCT_OBJECT",	"CALL",

	// expression ops: unary
	"NEG", "INV", "NOT", "DEF", "IN", "FEXISTS", "DEXISTS",
	// expression ops: binary
	"SUB", "ADD", "MUL", "DIV", "MOD", "INTDIV",
	"BIN_AND", "BIN_OR", "BIN_XOR",
	"LOG_AND", "LOG_OR", "LOG_XOR",
	"NUM_LT", "NUM_GT", "NUM_LE", "NUM_GE", "NUM_EQ", "NUM_NE",
	"STR_LT", "STR_GT", "STR_LE", "STR_GE", "STR_EQ", "STR_NE",
	"IS"
};

void va_debug_printf(Pool& pool, const char *fmt,va_list args) {
	char buf[MAX_STRING];
	vsnprintf(buf, MAX_STRING, fmt, args);
	SAPI::log(pool, "%s", buf);
}

void debug_printf(Pool& pool, const char *fmt, ...) {
    va_list args;
    va_start(args,fmt);
    va_debug_printf(pool,fmt,args);
    va_end(args);
}

void debug_dump(Pool& pool, int level, const Array& ops) {
	Array_iter i(ops);
	while(i.has_next()) {
		Operation op;
		op.cast=i.next();

		if(op.code==OP_VALUE || op.code==OP_STRING__WRITE) {
			Value *value=static_cast<Value *>(i.next());
			debug_printf(pool, 
				"%*s%s"
				" \"%s\" %s", 
				level*4, "", opcode_name[op.code],
				value->get_string()->cstr(), value->type());
			continue;
		}
		debug_printf(pool, "%*s%s", level*4, "", opcode_name[op.code]);

		switch(op.code) {
		case OP_CURLY_CODE__STORE_PARAM: 
		case OP_EXPR_CODE__STORE_PARAM:
		case OP_CURLY_CODE__CONSTRUCT:
		case OP_NESTED_CODE:
			const Array *local_ops=reinterpret_cast<const Array *>(i.next());
			debug_dump(pool, level+1, *local_ops);
		}
	}
}
#endif

#define PUSH(value) stack.push(value)
#define POP() static_cast<Value *>(stack.pop())
#define POP_NAME() static_cast<Value *>(stack.pop())->as_string()
#define POP_CODE() static_cast<Array *>(stack.pop())

void Request::execute(const Array& ops) {
//	_asm int 3;
#ifdef DEBUG_EXECUTE
	debug_printf(pool(), "source----------------------------\n");
	debug_dump(pool(), 0, ops);
	debug_printf(pool(), "execution-------------------------\n");
#endif

	Array_iter i(ops);
	while(i.has_next()) {
		Operation op;
		op.cast=i.next();
#ifdef DEBUG_EXECUTE
		debug_printf(pool(), "%d:%s", stack.top_index()+1, opcode_name[op.code]);
#endif

		Value *value;
		Value *a; Value *b;
		Array *b_code;
		switch(op.code) {
		// param in next instruction
		case OP_VALUE:
			{
				value=static_cast<Value *>(i.next());
#ifdef DEBUG_EXECUTE
				debug_printf(pool(), " \"%s\" %s", value->get_string()->cstr(), value->type());
#endif
				PUSH(value);
				break;
			}
		case OP_CURLY_CODE__STORE_PARAM:
		case OP_EXPR_CODE__STORE_PARAM:
			{
				VMethodFrame *frame=static_cast<VMethodFrame *>(stack.top_value());
				// code
				const Array *local_ops=reinterpret_cast<const Array *>(i.next());
#ifdef DEBUG_EXECUTE
				debug_printf(pool(), " (%d)\n", local_ops->size());
				debug_dump(pool(), 1, *local_ops);
#endif				
				// when they evaluate expression parameter,
				// the object expression result
				// does not need to be written into calling frame
				// it must go into any expressions using that parameter
				// hence, we zero junction.wcontext here, and later
				// in .process we would test that field 
				// in decision "which wwrapper to use"
				Junction& j=*NEW Junction(pool(), 
					*self, 0, 0,
					root, 
					rcontext, 
					op.code==OP_EXPR_CODE__STORE_PARAM?0:wcontext, 
					local_ops);
				
				value=NEW VJunction(j);

				// store param
				frame->store_param(frame->name(), value);
				break;
			}
		case OP_GET_CLASS:
			{
				// maybe they do ^class:method[] call, remember the fact
				wcontext->set_somebody_entered_some_class(true);

				const String& name=POP_NAME();
				value=static_cast<Value *>(classes().get(name));
				if(!value) 
					throw Exception(0,0,
						&name,
						"class is undefined"); 

				PUSH(value);
				break;
			}
			
		// OP_WITH
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
				value=POP();
				const String& name=POP_NAME();
				Value *ncontext=POP();
				ncontext->put_element(name, value);
				value->set_name(name);

				// forget the fact they've entered some $class/object.xxx
				// see OP_GET_ELEMENT
				wcontext->set_somebody_entered_some_object(false);
				wcontext->set_somebody_entered_some_class(false);
				break;
			}
		case OP_CONSTRUCT_EXPR:
			{
				value=POP();
				const String& name=POP_NAME();
				Value *ncontext=POP();
				ncontext->put_element(name, value->as_expr_result());
				value->set_name(name);
				break;
			}
		case OP_CURLY_CODE__CONSTRUCT:
			{
				const Array *local_ops=reinterpret_cast<const Array *>(i.next());
#ifdef DEBUG_EXECUTE
				debug_printf(pool(), " (%d)\n", local_ops->size());
				debug_dump(pool(), 1, *local_ops);
#endif				
				Junction& j=*NEW Junction(pool(), 
					*self, 0, 0,
					root, 
					rcontext, 
					wcontext, 
					local_ops);
				
				value=NEW VJunction(j);
				const String& name=POP_NAME();
				Value *ncontext=POP();
				ncontext->put_element(name, value);
				value->set_name(name);
				break;
			}
		case OP_NESTED_CODE:
			{
				Array *local_ops=static_cast<Array *>(i.next());
#ifdef DEBUG_EXECUTE
				debug_printf(pool(), " (%d)\n", local_ops->size());
				debug_dump(pool(), 1, *local_ops);
#endif				
				PUSH(local_ops);
				break;
			}
		case OP_WRITE_VALUE:
			{
				value=POP();
				write_assign_lang(*value);

				// forget the fact they've entered some ^object/class.xxx or $object/class.xxx
				// see OP_GET_ELEMENT
				wcontext->set_somebody_entered_some_object(false);
				wcontext->set_somebody_entered_some_class(false);
				break;
			}
		case OP_WRITE_EXPR_RESULT:
			{
				value=POP();
				write_expr_result(*value->as_expr_result());
				break;
			}
		case OP_STRING__WRITE:
			{
				VString *vstring=static_cast<VString *>(i.next());
#ifdef DEBUG_EXECUTE
				debug_printf(pool(), " \"%s\"", vstring->string().cstr());
#endif
				write_no_lang(vstring->string());
				break;
			}
			
		case OP_GET_ELEMENT:
			{
				//_asm int 3;
				// uses entered object/class flags to plug in operators
				value=get_element();

				// maybe they do ^object.method[] call, remember the fact
				// must be below get_element() call, it checks it
				wcontext->set_somebody_entered_some_object(true);

				PUSH(value);
				break;
			}

		case OP_GET_ELEMENT__WRITE:
			{
				value=get_element();
				write_assign_lang(*value);
				break;
			}


		case OP_CREATE_EWPOOL:
			{
				PUSH(wcontext);
				PUSH((void *)flang);
				flang=String::UL_PASS_APPENDED;
				wcontext=NEW WWrapper(pool(), 0 /*empty*/);
				break;
			}
		case OP_REDUCE_EWPOOL:
			{
				value=&wcontext->result();
				flang=static_cast<String::Untaint_lang>(reinterpret_cast<int>(POP()));
				wcontext=static_cast<WContext *>(POP());
				PUSH(value);
				break;
			}
			
		case OP_CREATE_SWPOOL:
			{
				PUSH(wcontext);
				wcontext=NEW WWrapper(pool(), 0 /*empty*/);
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
					NEW VVoid(pool());
				wcontext=static_cast<WContext *>(POP());
				PUSH(value);
				break;
			}

		// CALL
		case OP_GET_METHOD_FRAME:
			{
				value=POP();

				// info: 
				//	code compiled so that this one's always method-junction, 
				//	not a code-junction
				Junction *junction=value->get_junction();
				if(!junction)
					throw Exception(0, 0,
						&value->name(),
						"(%s) not a method or junction, can not call it",
							value->type()); 

				VMethodFrame *frame=NEW VMethodFrame(pool(), value->name(), *junction);
				PUSH(frame);
				break;
			}
		case OP_STORE_PARAM:
			{
				value=POP();
				VMethodFrame *frame=static_cast<VMethodFrame *>(stack.top_value());
				frame->store_param(frame->name(), value);
				break;
			}

		case OP_PREPARE_TO_CONSTRUCT_OBJECT:
			{
				wcontext->set_constructing(true);
				break;
			}
		case OP_CALL:
			{
#ifdef DEBUG_EXECUTE
				debug_printf(pool(), "->\n");
#endif
				VMethodFrame *frame=static_cast<VMethodFrame *>(POP());
				frame->fill_unspecified_params();
				PUSH(self);  
				PUSH(root);  
				PUSH(rcontext);  
				PUSH(wcontext); 
				
				VStateless_class *called_class=frame->junction.self.get_class();
				if(wcontext->get_constructing()) {
					wcontext->set_constructing(false);
					if(frame->junction.method->call_type!=Method::CT_STATIC) {
						// this is a constructor call

						if(Value *value=called_class->create_new_value(pool())) {
							// some stateless_class creatable derivates
							self=value;
						} else 
							throw Exception(0, 0,
								&frame->name(),
								"is not a constructor, system class '%s' can be constructed only implicitly", 
								called_class->name().cstr());

						frame->write(*self, 
							String::UL_CLEAN  // not used, always an object, not string
						);
					} else
						throw Exception(0, 0,
							&frame->name(),
							"method is static and can not be used as constructor");
				} else {
					// this is not constructor call

					// not ^name.method call, name:method call; and
					// is context object or class & is it my class or my parent's class and?
					VStateless_class *read_class=rcontext->get_class();
					if(
						!(wcontext->get_somebody_entered_some_object() &&
						!wcontext->get_somebody_entered_some_class()) && 
						read_class && read_class->is_or_derived_from(*called_class)) // yes
						self=rcontext; // dynamic call
					else // no, not me or relative of mine (=total stranger)
						self=&frame->junction.self; // static call
				}

				frame->set_self(*self);
				rcontext=wcontext=frame;
				{
					// take object or class from any wrappers
					// and substitute class alias to the class they are called AS
					Temp_alias temp_alias(*self->get_aliased(), *frame->junction.vclass);

					const Method& method=*frame->junction.method;
					Method::Call_type call_type=
						called_class==self ? Method::CT_STATIC : Method::CT_DYNAMIC;
					if(
						method.call_type==Method::CT_ANY ||
						method.call_type==call_type) { // allowed call type?
						try {
							if(method.native_code) { // native code?
								// root unchanged, so that ^for ^foreach & co may write to locals
								method.check_actual_numbered_params(
									frame->junction.self, 
									frame->name(), frame->numbered_params());
								method.native_code(
									*this, 
									frame->name(), frame->numbered_params()); // execute it
							} else { // parser code
								root=frame;
								{ // anti_endless_execute_recoursion
									if(++anti_endless_execute_recoursion==ANTI_ENDLESS_EXECUTE_RECOURSION) {
										anti_endless_execute_recoursion=0; // give @exception a chance
										throw Exception(0, 0,
											&frame->name(),
											"call canceled - endless recursion detected");
									}
									execute(*method.parser_code); // execute it
									anti_endless_execute_recoursion--;
								}
							}
						} catch(...) {
							// record it to stack trace
							trace.push((void *)&frame->name());
							/*re*/throw;
						}
					} else
						throw Exception(0, 0,
							&frame->name(),
							"is not allowed to be called %s", 
								call_type==Method::CT_STATIC?"statically":"dynamically");

				}
				value=&wcontext->result();

				wcontext=static_cast<WContext *>(POP());  
				rcontext=POP();  
				root=POP();  
				self=static_cast<VAliased *>(POP());

				PUSH(value);
#ifdef DEBUG_EXECUTE
				debug_printf(pool(), "<-returned");
#endif
				break;
			}

		// expression ops: unary
		case OP_NEG:
			{
				Value *operand=POP();
				value=NEW VDouble(pool(), -operand->as_double());
				PUSH(value);
				break;
			}
		case OP_INV:
			{
				Value *operand=POP();
				value=NEW VDouble(pool(), ~operand->as_int());
				PUSH(value);
				break;
			}
		case OP_NOT:
			{
				Value *operand=POP();
				value=NEW VBool(pool(), !operand->as_bool());
				PUSH(value);
				break;
			}
		case OP_DEF:
			{
				Value *operand=POP();
				value=NEW VBool(pool(), operand->is_defined());
				PUSH(value);
				break;
			}
		case OP_IN:
			{
				/// @test String::cmp
				Value *operand=POP();
				const char *path=operand->as_string().cstr();
				value=NEW VBool(pool(), 
					info.uri && strncmp(path, info.uri, strlen(path))==0);
				PUSH(value);
				break;
			}
		case OP_FEXISTS:
			{
				Value *operand=POP();
				value=NEW VBool(pool(), 
					file_readable(absolute(operand->as_string())));
				PUSH(value);
				break;
			}
		case OP_DEXISTS:
			{
				Value *operand=POP();
				value=NEW VBool(pool(), 
					dir_readable(absolute(operand->as_string())));
				PUSH(value);
				break;
			}

		// expression ops: binary
		case OP_SUB: 
			{
				b=POP();  a=POP();
				value=NEW VDouble(pool(), a->as_double() - b->as_double());
				PUSH(value);
				break;
			}
		case OP_ADD: 
			{
				b=POP();  a=POP();
				value=NEW VDouble(pool(), a->as_double() + b->as_double());
				PUSH(value);
				break;
			}
		case OP_MUL: 
			{
				b=POP();  a=POP();
				value=NEW VDouble(pool(), a->as_double() * b->as_double());
				PUSH(value);
				break;
			}
		case OP_DIV: 
			{
				b=POP();  a=POP();

				double a_double=a->as_double();
				double b_double=b->as_double();

				if(b_double == 0) {
					const String *problem_source=&b->as_string();
#ifndef NO_STRING_ORIGIN
					if(!problem_source->origin().file)
						problem_source=&b->name();
#endif
					throw Exception(0, 0,
						problem_source,
						"Division by zero");
				}

				value=NEW VDouble(pool(), a_double / b_double);
				PUSH(value);
				break;
			}
		case OP_MOD: 
			{
				b=POP();  a=POP();

				double a_double=a->as_double();
				double b_double=b->as_double();

				if(b_double == 0) {
					const String *problem_source=&b->as_string();
#ifndef NO_STRING_ORIGIN
					if(!problem_source->origin().file)
						problem_source=&b->name();
#endif
					throw Exception(0, 0,
						problem_source,
						"Modulus by zero");
				}

				value=NEW VDouble(pool(), fmod(a_double, b_double));
				PUSH(value);
				break;
			}
		case OP_INTDIV:
			{
				b=POP();  a=POP();

				int a_int=a->as_int();
				int b_int=b->as_int();

				if(b_int == 0) {
					const String *problem_source=&b->as_string();
#ifndef NO_STRING_ORIGIN
					if(!problem_source->origin().file)
						problem_source=&b->name();
#endif
					throw Exception(0, 0,
						problem_source,
						"Division by zero");
				}

				value=NEW VInt(pool(), a_int / b_int);
				PUSH(value);
				break;
			}
		case OP_BIN_AND:
			{
				b=POP();  a=POP();
				value=NEW VDouble(pool(), 
					a->as_int() &
					b->as_int());
				PUSH(value);
				break;
			}
		case OP_BIN_OR:
			{
				b=POP();  a=POP();
				value=NEW VDouble(pool(), 
					a->as_int() |
					b->as_int());
				PUSH(value);
				break;
			}
		case OP_BIN_XOR:
			{
				b=POP();  a=POP();
				value=NEW VDouble(pool(), 
					a->as_int() ^
					b->as_int());
				PUSH(value);
				break;
			}
		case OP_LOG_AND:
			{
				b_code=POP_CODE();  a=POP();
				bool result;
				if(a->as_bool()) {
					execute(*b_code);
					b=POP();
					result=b->as_bool();
				} else
					result=false;
				value=NEW VBool(pool(), result);
				PUSH(value);
				break;
			}
		case OP_LOG_OR:
			{
				b_code=POP_CODE();  a=POP();
				bool result;
				if(a->as_bool()) 
					result=true;
				else {
					execute(*b_code);
					b=POP();
					result=b->as_bool();
				}
				value=NEW VBool(pool(), result);
				PUSH(value);
				break;
			}
		case OP_LOG_XOR:
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_bool() ^ b->as_bool());
				PUSH(value);
				break;
			}
		case OP_NUM_LT: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_double() < b->as_double());
				PUSH(value);
				break;
			}
		case OP_NUM_GT: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_double() > b->as_double());
				PUSH(value);
				break;
			}
		case OP_NUM_LE: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_double() <= b->as_double());
				PUSH(value);
				break;
			}
		case OP_NUM_GE: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_double() >= b->as_double());
				PUSH(value);
				break;
			}
		case OP_NUM_EQ: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_double() == b->as_double());
				PUSH(value);
				break;
			}
		case OP_NUM_NE: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_double() != b->as_double());
				PUSH(value);
				break;
			}
		case OP_STR_LT: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_string() < b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_GT: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_string() > b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_LE: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_string() <= b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_GE: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_string() >= b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_EQ: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_string() == b->as_string());
				PUSH(value);
				break;
			}
		case OP_STR_NE: 
			{
				b=POP();  a=POP();
				value=NEW VBool(pool(), a->as_string() != b->as_string());
				PUSH(value);
				break;
			}
		case OP_IS:
			{
				//_asm int 3;
				b=POP();  a=POP();
				value=NEW VBool(pool(), b->as_string() == a->type());
				PUSH(value);
				break;
			}

		default:
			throw Exception(0,0,
				0,
				"invalid opcode %d", op.code); 
		}
	}
}

Value *Request::get_element() {
	const String& name=POP_NAME();
	Value *ncontext=POP();
	Value *value=ncontext->get_element(name);
	// operators can not be in form...
	if(!value && 
		!wcontext->get_somebody_entered_some_class () && // $class:xxx
		!wcontext->get_somebody_entered_some_object()) // $object.xxx
		// they can only be in form ^xxx or $xxx
		if(Method* method=OP.get_method(name)) { // maybe operator?
			// as if that method were in self and we have normal dynamic method here
			Junction& junction=*NEW Junction(pool(), 
				*root, self->get_class(), method, 0,0,0,0);
			value=NEW VJunction(junction);
		}
	if(value)
		value=&process(*value, &name); // process possible code-junction
	else {
		value=NEW VVoid(pool());
		value->set_name(name);
	}

	return value;
}

/**	@param intercept_string
	- true:
		they want result=string value, 
		possible object result goes to wcontext
	- false:
		they want any result[string|object]
		nothing goes to wcontext.
		used in @c (expression) params evaluation
*/
Value& Request::process(Value& value, const String *name, bool intercept_string) {
	Value *result;
	Junction *junction=value.get_junction();
	if(junction && junction->code) { // is it a code-junction?
		// process it
#ifdef DEBUG_EXECUTE
		debug_printf(pool(), "ja->\n");
#endif
		PUSH(self);  
		PUSH(root);  
		PUSH(rcontext);  
		PUSH(wcontext);
		
		WContext *frame;
		// for expression method params
		// wcontext is set 0
		// using the fact in decision "which wwrapper to use"
		bool using_code_frame=intercept_string && junction->wcontext;
		if(using_code_frame) {
			// almost plain wwrapper about junction wcontext, 
			// BUT intercepts string writes
			frame=NEW VCodeFrame(pool(), *junction->wcontext);  
		} else {
			// plain wwrapper
			frame=NEW WWrapper(pool(), 0/*empty*/);
		}
		
		//frame->set_name(value.name());
		wcontext=frame;
		self=&junction->self;
		root=junction->root;
		rcontext=junction->rcontext;

		{ // anti_endless_execute_recoursion
			if(++anti_endless_execute_recoursion==ANTI_ENDLESS_EXECUTE_RECOURSION) {
				anti_endless_execute_recoursion=0; // give @exception a chance
				throw Exception(0, 0,
					name,
					"junction evaluation canceled - endless recursion detected");
			}
			execute(*junction->code);
			anti_endless_execute_recoursion--;
		}
		
		if(using_code_frame) {
			// CodeFrame soul:
			//   string writes were intercepted
			//   returning them as the result of getting code-junction
			result=NEW VString(*frame->get_string());
		} else 
			result=&frame->result();
		
		wcontext=static_cast<WContext *>(POP());  
		rcontext=POP();  
		root=POP();  
		self=static_cast<VAliased *>(POP());
		
#ifdef DEBUG_EXECUTE
		debug_printf(pool(), "<-ja returned");
#endif
	} else
		result=&value;

	if(name)
		result->set_name(*name);
	return *result;
}

const String *Request::execute_method(Value& aself, const Method& method,
		bool return_cstr) {
	PUSH(self);  
	PUSH(root);  
	PUSH(rcontext);  
	PUSH(wcontext);
	
	// initialize contexts
	root=rcontext=self=&aself;
	wcontext=NEW WWrapper(pool(), &aself);
	
	// execute!	
	execute(*method.parser_code);
	
	// result
	const String *result=return_cstr ? &wcontext->as_string() : 0;
	
	wcontext=static_cast<WContext *>(POP());  
	rcontext=POP();  
	root=POP();  
	self=static_cast<VAliased *>(POP());
	
	// return
	return result;
}

const String& Request::execute_method(VMethodFrame& amethodFrame, const Method& method) {
	PUSH(self);  
	PUSH(root);  
	PUSH(rcontext);  
	PUSH(wcontext);
	
	// initialize contexts
	root=rcontext=self=&amethodFrame;
	wcontext=&amethodFrame;
	
	// execute!	
	execute(*method.parser_code);
	
	// result
	const String& result=wcontext->as_string();
	
	wcontext=static_cast<WContext *>(POP());  
	rcontext=POP();  
	root=POP();  
	self=static_cast<VAliased *>(POP());
	
	// return
	return result;
}

const String *Request::execute_virtual_method(Value& aself, 
											  const String& method_name) {
	if(Value *value=aself.get_element(method_name))
		if(Junction *junction=value->get_junction())
			if(const Method *method=junction->method) 
				return execute_method(aself, *method, true /*return_cstr*/);
			
	return 0;
}

const String *Request::execute_nonvirtual_method(VStateless_class& aclass, 
												 const String& method_name,
												 bool return_cstr) {
	if(const Method *method=aclass.get_method(method_name))
		return execute_method(aclass, *method, return_cstr);

	return 0;
}
