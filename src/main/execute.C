/** @file
	Parser: executor part of request class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
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

volatile const char * IDENT_EXECUTE_C="$Id: execute.C,v 1.369 2012/03/16 09:51:27 moko Exp $" IDENT_PA_OPCODE_H IDENT_PA_OPERATION_H IDENT_PA_VCODE_FRAME_H IDENT_PA_WWRAPPER_H;

//#define DEBUG_EXECUTE

#ifdef DEBUG_EXECUTE
char *opcode_name[]={
	// literals
	"VALUE",  "CURLY_CODE__STORE_PARAM",  "EXPR_CODE__STORE_PARAM",
	"NESTED_CODE",

	// actions
	"WITH_ROOT",	"WITH_SELF",	"WITH_READ",	"WITH_WRITE",
	"VALUE__GET_CLASS",
	"CONSTRUCT_VALUE", "CONSTRUCT_EXPR", "CURLY_CODE__CONSTRUCT",
	"WRITE_VALUE",  "WRITE_EXPR_RESULT",  "STRING__WRITE",
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	"VALUE__GET_ELEMENT_OR_OPERATOR",
#else
	"GET_ELEMENT_OR_OPERATOR",
#endif
	"GET_ELEMENT",
	"GET_ELEMENT__WRITE",
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	"VALUE__GET_ELEMENT",
	"VALUE__GET_ELEMENT__WRITE",
	"WITH_ROOT__VALUE__GET_ELEMENT",
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	"GET_OBJECT_ELEMENT",
	"GET_OBJECT_ELEMENT__WRITE",
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	"GET_OBJECT_VAR_ELEMENT",
	"GET_OBJECT_VAR_ELEMENT__WRITE",
#endif
#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
	"WITH_SELF__VALUE__GET_ELEMENT",
	"WITH_SELF__VALUE__GET_ELEMENT__WRITE",
#endif
	"OBJECT_POOL",	"STRING_POOL",
	"PREPARE_TO_CONSTRUCT_OBJECT",
	"CONSTRUCT_OBJECT",
	"CONSTRUCT_OBJECT__WRITE",
	"PREPARE_TO_EXPRESSION", 
	"CALL", "CALL__WRITE",

#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
	"WITH_ROOT__VALUE__CONSTRUCT_EXPR",
	"WITH_ROOT__VALUE__CONSTRUCT_VALUE",
	"WITH_WRITE__VALUE__CONSTRUCT_EXPR",
	"WITH_WRITE__VALUE__CONSTRUCT_VALUE",
	"WITH_SELF__VALUE__CONSTRUCT_EXPR",
	"WITH_SELF__VALUE__CONSTRUCT_VALUE",
#endif

	// expression ops: unary
	"NEG", "INV", "NOT", "DEF", "IN", "FEXISTS", "DEXISTS",
	// expression ops: binary
	"SUB", "ADD", "MUL", "DIV", "MOD", "INTDIV",
	"BIN_SL", "BIN_SR",
	"BIN_AND", "BIN_OR", "BIN_XOR",
	"LOG_AND", "LOG_OR", "LOG_XOR",
	"NUM_LT", "NUM_GT", "NUM_LE", "NUM_GE", "NUM_EQ", "NUM_NE",
	"STR_LT", "STR_GT", "STR_LE", "STR_GE", "STR_EQ", "STR_NE",
	"IS"
};

const char* debug_value_to_cstr(Value& value){
	const String* string=value.get_string();

	if(string)
		return string->cstr();
	else
		if(value.is_bool())
			return value.as_bool()?"<true>":"<false>";
		else
			return "<value>";
}

void va_debug_printf(SAPI_Info& sapi_info, const char* fmt,va_list args) {
	char buf[MAX_STRING];
	vsnprintf(buf, MAX_STRING, fmt, args);
	SAPI::log(sapi_info, "%s", buf);
}

void debug_printf(SAPI_Info& sapi_info, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	va_debug_printf(sapi_info, fmt, args);
	va_end(args);
}

void debug_dump(SAPI_Info& sapi_info, int level, ArrayOperation& ops) {
	Array_iterator<Operation> i(ops);
	while(i.has_next()) {
		OP::OPCODE opcode=i.next().code;

#if defined(OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT) || defined(OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT)
		if(
			1==0
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
			|| opcode==OP::OP_GET_OBJECT_ELEMENT
			|| opcode==OP::OP_GET_OBJECT_ELEMENT__WRITE
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
			|| opcode==OP::OP_GET_OBJECT_VAR_ELEMENT
			|| opcode==OP::OP_GET_OBJECT_VAR_ELEMENT__WRITE
#endif
		){
			i.next(); // skip origin
			Value& value1=*i.next().value;
			i.next(); // skip origin
			Value& value2=*i.next().value;
			debug_printf(sapi_info, 
				"%*s%s"
				" \"%s\" \"%s\"", 
				level*4, "", opcode_name[opcode],
				debug_value_to_cstr(value1), debug_value_to_cstr(value2));
			continue;
		}
#endif
		if(
			opcode==OP::OP_CONSTRUCT_OBJECT
			|| opcode==OP::OP_CONSTRUCT_OBJECT__WRITE
		){
			i.next(); // skip origin
			Value& value1=*i.next().value;
			i.next(); // skip origin
			Value& value2=*i.next().value;
			debug_printf(sapi_info, 
				"%*s%s"
				" \"%s\" \"%s\"", 
				level*4, "", opcode_name[opcode],
				debug_value_to_cstr(value1), debug_value_to_cstr(value2));

			if(ArrayOperation* local_ops=i.next().ops)
				debug_dump(sapi_info, level+1, *local_ops);
			continue;
		}
		if(
			opcode==OP::OP_VALUE
			|| opcode==OP::OP_STRING__WRITE
			|| opcode==OP::OP_VALUE__GET_CLASS
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
			|| opcode==OP::OP_VALUE__GET_ELEMENT
			|| opcode==OP::OP_VALUE__GET_ELEMENT__WRITE
			|| opcode==OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR
			|| opcode==OP::OP_WITH_ROOT__VALUE__GET_ELEMENT
#endif
#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
			|| opcode==OP::OP_WITH_SELF__VALUE__GET_ELEMENT
			|| opcode==OP::OP_WITH_SELF__VALUE__GET_ELEMENT__WRITE
#endif
#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
			|| opcode==OP::OP_WITH_ROOT__VALUE__CONSTRUCT_EXPR
			|| opcode==OP::OP_WITH_ROOT__VALUE__CONSTRUCT_VALUE
			|| opcode==OP::OP_WITH_WRITE__VALUE__CONSTRUCT_EXPR
			|| opcode==OP::OP_WITH_WRITE__VALUE__CONSTRUCT_VALUE
			|| opcode==OP::OP_WITH_SELF__VALUE__CONSTRUCT_EXPR
			|| opcode==OP::OP_WITH_SELF__VALUE__CONSTRUCT_VALUE
#endif
		) {
			Operation::Origin origin=i.next().origin;
			Value& value=*i.next().value;

			debug_printf(sapi_info, 
				"%*s%s"
				" \"%s\" %s", 
				level*4, "", opcode_name[opcode],
				debug_value_to_cstr(value), value.type());
			continue;
		}

		debug_printf(sapi_info, "%*s%s", level*4, "", opcode_name[opcode]);

		switch(opcode) {
		case OP::OP_CURLY_CODE__STORE_PARAM: 
		case OP::OP_EXPR_CODE__STORE_PARAM:
		case OP::OP_CURLY_CODE__CONSTRUCT:
		case OP::OP_NESTED_CODE:
		case OP::OP_OBJECT_POOL:
		case OP::OP_STRING_POOL:
		case OP::OP_CALL:
		case OP::OP_CALL__WRITE:
			if(ArrayOperation* local_ops=i.next().ops)
				debug_dump(sapi_info, level+1, *local_ops);
		}
	}
}
#define DEBUG_PRINT_STR(str) debug_printf(sapi_info, str);
#define DEBUG_PRINT_STRING(value) debug_printf(sapi_info, " \"%s\" ", value.cstr());
#define DEBUG_PRINT_VALUE_AND_TYPE(value) debug_printf(sapi_info, " \"%s\" %s", debug_value_to_cstr(value), value.type());
#define DEBUG_PRINT_OPS(local_ops) \
					debug_printf(sapi_info, \
					" (%d)\n", local_ops?local_ops->count():0); \
					if(local_ops) debug_dump(sapi_info, 1, *local_ops);

#else
#define DEBUG_PRINT_STR(str)
#define DEBUG_PRINT_STRING(value)
#define DEBUG_PRINT_VALUE_AND_TYPE(value)
#define DEBUG_PRINT_OPS(local_ops)
#endif


// Request

void Request::execute(ArrayOperation& ops) {
	register Stack<StackItem>& stack=this->stack; // helps a lot on MSVC: 'esi'

	const String* debug_name=0;  Operation::Origin debug_origin={0, 0, 0};
	try{
#ifdef DEBUG_EXECUTE
	debug_printf(sapi_info, "source----------------------------\n");
	debug_dump(sapi_info, 0, ops);
	debug_printf(sapi_info, "execution-------------------------\n");
#endif
	for(Array_iterator<Operation> i(ops); i.has_next(); ) {
		OP::OPCODE opcode=i.next().code;

#ifdef DEBUG_EXECUTE
		debug_printf(sapi_info, "%d:%s", stack.top_index()+1, opcode_name[opcode]);
#endif

		switch(opcode) {
		// param in next instruction
		case OP::OP_VALUE:
			{
				debug_origin=i.next().origin;
				Value& value=*i.next().value;

				DEBUG_PRINT_VALUE_AND_TYPE(value)

				stack.push(value);
				break;
			}
		case OP::OP_VALUE__GET_CLASS:
			{
				// maybe they do ^class:method[] call, remember the fact
				wcontext->set_somebody_entered_some_class();

				debug_origin=i.next().origin;
				Value& value=*i.next().value;
				const String& name=*value.get_string(); debug_name=&name;

				DEBUG_PRINT_STRING(name)

				Value* class_value=get_class(name);
				if(!class_value)
					throw Exception(PARSER_RUNTIME,
						&name,
						"class is undefined"); 

				stack.push(*class_value);
				break;
			}
		// OP_WITH
		case OP::OP_WITH_ROOT:
			{
				stack.push(*method_frame);
				break;
			}
		case OP::OP_WITH_SELF:
			{
				stack.push(get_self());
				break;
			}
		case OP::OP_WITH_READ:
			{
				stack.push(*rcontext);
				break;
			}
		case OP::OP_WITH_WRITE:
			{
				if(wcontext==method_frame)
					throw Exception(PARSER_RUNTIME,
						0,
						"$.name outside of $name[...]");

				stack.push(*wcontext);
				break;
			}
			
		// OTHER ACTIONS BUT WITHs

#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
#define DO_CONSTRUCT(context, vvalue) {                                                 \
				debug_origin=i.next().origin;                                           \
				const String& name=*i.next().value->get_string();  debug_name=&name;    \
				DEBUG_PRINT_STRING(name)                                                \
				Value& value=stack.pop().value();                                       \
				put_element( context, name, vvalue );                                   \
				break;                                                                  \
		}
#define DO_CONSTRUCT_VALUE(context) DO_CONSTRUCT(context, &value)
#define DO_CONSTRUCT_EXPR(context) {                                                    \
				wcontext->set_in_expression(false);                                     \
				DO_CONSTRUCT(context, &value.as_expr_result())                          \
		}

		case OP::OP_WITH_WRITE__VALUE__CONSTRUCT_EXPR:
			{
				if(wcontext==method_frame)
					throw Exception(PARSER_RUNTIME, 0, "$.name outside of $name[...]");
				DO_CONSTRUCT_EXPR(*wcontext)
			}

		case OP::OP_WITH_WRITE__VALUE__CONSTRUCT_VALUE:
			{
				if(wcontext==method_frame)
					throw Exception(PARSER_RUNTIME, 0, "$.name outside of $name[...]");
				DO_CONSTRUCT_VALUE(*wcontext)
			}

		case OP::OP_WITH_ROOT__VALUE__CONSTRUCT_EXPR:	DO_CONSTRUCT_EXPR(*method_frame)

		case OP::OP_WITH_ROOT__VALUE__CONSTRUCT_VALUE:	DO_CONSTRUCT_VALUE(*method_frame)

		case OP::OP_WITH_SELF__VALUE__CONSTRUCT_EXPR:	DO_CONSTRUCT_EXPR(get_self())

		case OP::OP_WITH_SELF__VALUE__CONSTRUCT_VALUE:	DO_CONSTRUCT_VALUE(get_self())

#endif // OPTIMIZE_BYTECODE_CONSTRUCT

		case OP::OP_CONSTRUCT_VALUE:
			{
#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				Value& value=stack.pop().value();
#else
				Value& value=stack.pop().value();
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
#endif
				put_element(ncontext, name, &value);

				break;
			}

		case OP::OP_CONSTRUCT_EXPR:
			{
				// see OP_PREPARE_TO_EXPRESSION
				wcontext->set_in_expression(false);

#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				Value& expr=stack.pop().value();
#else
				Value& expr=stack.pop().value();
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
#endif
				Value& value=expr.as_expr_result();
				put_element(ncontext, name, &value);
				break;
			}

		case OP::OP_CURLY_CODE__CONSTRUCT:
			{
				ArrayOperation& local_ops=*i.next().ops;

				DEBUG_PRINT_OPS((&local_ops))

				VJunction& value=*new VJunction(
					get_self(), 0,
					method_frame, 
					rcontext, 
					wcontext, 
					&local_ops);

				wcontext->attach_junction(&value);

				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				if(const VJunction* vjunction=ncontext.put_element(name, &value, false))
					if(vjunction!=PUT_ELEMENT_REPLACED_ELEMENT)
						throw Exception(PARSER_RUNTIME,
							0,
							"property value can not be code, use [] or () brackets");
					
				break;
			}
		case OP::OP_NESTED_CODE:
			{
				ArrayOperation& local_ops=*i.next().ops;

				DEBUG_PRINT_OPS((&local_ops))

				stack.push(local_ops);
				break;
			}
		case OP::OP_WRITE_VALUE:
			{
				Value& value=stack.pop().value();
				write_assign_lang(value);
				break;
			}
		case OP::OP_WRITE_EXPR_RESULT:
			{
				// see OP_PREPARE_TO_EXPRESSION
				wcontext->set_in_expression(false);

				Value& value=stack.pop().value();
				wcontext->write(value.as_expr_result());
				break;
			}
		case OP::OP_STRING__WRITE:
			{
				i.next(); // ignore origin
				Value* value=i.next().value;

				const String& string_value=*value->get_string();

				DEBUG_PRINT_STRING(string_value)

				write_no_lang(string_value);
				break;
			}

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
		case OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR:
			{
				debug_origin=i.next().origin;
				const String& name=*i.next().value->get_string();  debug_name=&name;

				DEBUG_PRINT_STRING(name)

				if(Method* method=main_class.get_method(name)){ // looking operator of that name FIRST
					if(!method->junction_template) method->junction_template=new VJunction(main_class, method);
					stack.push(*method->junction_template);
					break;
				}
				Value& value=get_element(*rcontext, name);
				stack.push(value);
				break;
			}
#else
		case OP::OP_GET_ELEMENT_OR_OPERATOR:
			{
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				if(Method* method=main_class.get_method(name)){ // looking operator of that name FIRST
					if(!method->junction_template) method->junction_template=new VJunction(main_class, method);
					stack.push(*method->junction_template);
					break;
				}
				Value& value=get_element(ncontext, name);
				stack.push(value);
				break;
			}
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
		case OP::OP_GET_OBJECT_ELEMENT:
		case OP::OP_GET_OBJECT_ELEMENT__WRITE:
			{
				debug_origin=i.next().origin;
				const String& context_name=*i.next().value->get_string();  debug_name=&context_name;

				DEBUG_PRINT_STRING(context_name)

				Value& object=get_element(*rcontext, context_name);

				debug_origin=i.next().origin;
				const String& field_name=*i.next().value->get_string();  debug_name=&field_name;

				DEBUG_PRINT_STRING(field_name)

				Value& value=get_element(object, field_name);

				if(opcode==OP::OP_GET_OBJECT_ELEMENT){
					stack.push(value);
				} else {
					write_assign_lang(value);
				}
				break;
			}
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
		case OP::OP_GET_OBJECT_VAR_ELEMENT:
		case OP::OP_GET_OBJECT_VAR_ELEMENT__WRITE:
			{
				debug_origin=i.next().origin;
				const String& context_name=*i.next().value->get_string();  debug_name=&context_name;

				DEBUG_PRINT_STRING(context_name)

				Value& object=get_element(*rcontext, context_name);

				debug_origin=i.next().origin;
				const String& var_name=*i.next().value->get_string();  debug_name=&var_name;

				DEBUG_PRINT_STRING(var_name)

				const String* field=&get_element(*rcontext, var_name).as_string();

				Value& value=get_element(object, *field);

				if(opcode==OP::OP_GET_OBJECT_VAR_ELEMENT){
					stack.push(value);
				} else {
					write_assign_lang(value);
				}
				break;
			}
#endif

		case OP::OP_GET_ELEMENT:
			{
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				Value& value=get_element(ncontext, name);
				stack.push(value);
				break;
			}

		case OP::OP_GET_ELEMENT__WRITE:
			{
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				Value& value=get_element(ncontext, name);
				write_assign_lang(value);
				break;
			}

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
		case OP::OP_VALUE__GET_ELEMENT:
			{
				debug_origin=i.next().origin;
				const String& name=*i.next().value->get_string(); debug_name=&name;

				DEBUG_PRINT_STRING(name)

				Value& value=get_element(*rcontext, name);
				stack.push(value);
				break;
			}

		case OP::OP_VALUE__GET_ELEMENT__WRITE:
			{
				debug_origin=i.next().origin;
				const String& name=*i.next().value->get_string(); debug_name=&name;

				DEBUG_PRINT_STRING(name)

				Value& value=get_element(*rcontext, name);
				write_assign_lang(value);
				break;
			}

		case OP::OP_WITH_ROOT__VALUE__GET_ELEMENT:
			{
				debug_origin=i.next().origin;
				const String& name=*i.next().value->get_string(); debug_name=&name;

				DEBUG_PRINT_STRING(name)

				Value& value=get_element(*method_frame, name);
				stack.push(value);
				break;
			}
#endif

#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
		case OP::OP_WITH_SELF__VALUE__GET_ELEMENT:
		case OP::OP_WITH_SELF__VALUE__GET_ELEMENT__WRITE:
			{
				debug_origin=i.next().origin;
				const String& name=*i.next().value->get_string(); debug_name=&name;

				DEBUG_PRINT_STRING(name)

				Value& value=get_element(get_self(), name);

				if(opcode==OP::OP_WITH_SELF__VALUE__GET_ELEMENT){
					stack.push(value);
				} else {
					write_assign_lang(value);
				}
				break;
			}
#endif

		case OP::OP_OBJECT_POOL:
			{
				ArrayOperation& local_ops=*i.next().ops;
				
				WContext *saved_wcontext=wcontext;
				String::Language saved_lang=flang;
				flang=String::L_PASS_APPENDED;
#ifdef OPTIMIZE_SINGLE_STRING_WRITE
				WObjectPoolWrapper local(wcontext);
#else
				WWrapper local(wcontext);
#endif
				wcontext=&local;

				execute(local_ops);

				stack.push(wcontext->result().as_value());
				flang=saved_lang;
				wcontext=saved_wcontext;
				break;
			}
			
		case OP::OP_STRING_POOL:
			{
				ArrayOperation& local_ops=*i.next().ops;

				WContext *saved_wcontext=wcontext;
				WWrapper local(wcontext);
				wcontext=&local;

				execute(local_ops);

				// from "$a $b" part of expression taking only string value,
				// ignoring any other content of wcontext
				const String* string=wcontext->get_string();
				Value* value=string ? new VString(*string) : new VString();
				stack.push(*value);

				wcontext=saved_wcontext;
				break;
			}

		// CALL
		case OP::OP_CURLY_CODE__STORE_PARAM:
		case OP::OP_EXPR_CODE__STORE_PARAM:
			{
				// code
				ArrayOperation& local_ops=*i.next().ops;

				DEBUG_PRINT_OPS((&local_ops))

				// when they evaluate expression parameter,
				// the object expression result
				// does not need to be written into calling frame
				// it must go into any expressions using that parameter
				// hence, we zero junction.wcontext here, and later
				// in .process we would test that field 
				// in decision "which wwrapper to use"
				VJunction& value=*new VJunction(
					get_self(), 0,
					method_frame, 
					rcontext, 
					opcode==OP::OP_EXPR_CODE__STORE_PARAM?0:wcontext, 
					&local_ops);
#ifndef USE_DESTRUCTORS
				if (opcode!=OP::OP_EXPR_CODE__STORE_PARAM)
					wcontext->attach_junction(&value);
#endif
				// store param
				stack.push(value);
				break;
			}

		case OP::OP_PREPARE_TO_EXPRESSION:
			{
				wcontext->set_in_expression(true);
				break;
			}

#define METHOD_FRAME_ACTION(action)													\
		if(local_ops){																\
			size_t first = stack.top_index();										\
			execute(*local_ops);													\
			frame.store_params((Value**)stack.ptr(first), stack.top_index()-first);	\
			action;																	\
			stack.set_top_index(first);												\
		} else {																	\
			frame.empty_params();													\
			action;																	\
		}

		case OP::OP_CALL:
			{
				ArrayOperation* local_ops=i.next().ops;

				DEBUG_PRINT_OPS(local_ops)
				DEBUG_PRINT_STR("->\n")

				Value& value=stack.pop().value();

				Junction* junction=value.get_junction();
				if(!junction) {
					if(value.is("void"))
						throw Exception(PARSER_RUNTIME,
							0,
							"undefined method");
					else
						throw Exception(PARSER_RUNTIME,
							0,
							"is '%s', not a method or junction, can not call it",
								value.type());
				}

				VMethodFrame frame(*junction->method, method_frame, junction->self);
				METHOD_FRAME_ACTION(op_call(frame));
				stack.push(frame.result().as_value());

				DEBUG_PRINT_STR("<-returned")

				if(get_skip())
					return;
				if(get_interrupted()) {
					set_interrupted(false);
					throw Exception("parser.interrupted",
						0,
						"execution stopped");
				}
				break;
			}

		case OP::OP_CALL__WRITE:
			{
				ArrayOperation* local_ops=i.next().ops;

				DEBUG_PRINT_OPS(local_ops)
				DEBUG_PRINT_STR("->\n")

				Value& value=stack.pop().value();

				Junction* junction=value.get_junction();
				if(!junction) {
					if(value.is("void"))
						throw Exception(PARSER_RUNTIME,
							0,
							"undefined method");
					else
						throw Exception(PARSER_RUNTIME,
							0,
							"is '%s', not a method or junction, can not call it",
							value.type());
				}

#ifdef OPTIMIZE_CALL
				const Method& method=*junction->method;
				if(method.call_optimization==Method::CO_WITHOUT_FRAME){
					if(local_ops){ // store param code goes here
						size_t first = stack.top_index();
						execute(*local_ops);

						MethodParams method_params;
						method_params.store_params((Value**)stack.ptr(first), stack.top_index()-first);
						method.check_actual_numbered_params(junction->self, &method_params);
						method.native_code(*this, method_params); // execute it

						stack.set_top_index(first);
					} else {
						MethodParams method_params;
						method.check_actual_numbered_params(junction->self, &method_params);
						method.native_code(*this, method_params); // execute it
					}
				} else if(method.call_optimization==Method::CO_WITHOUT_WCONTEXT){
					VMethodFrame frame(method, method_frame, junction->self);
					METHOD_FRAME_ACTION(op_call_write(frame));
				} else 
#endif // OPTIMIZE_CALL
				{
					VMethodFrame frame(method, method_frame, junction->self);
					METHOD_FRAME_ACTION(op_call(frame));
					write_pass_lang(frame.result());
				}

				DEBUG_PRINT_STR("<-returned")

				if(get_skip())
					return;
				if(get_interrupted()) {
					set_interrupted(false);
					throw Exception("parser.interrupted",
						0,
						"execution stopped");
				}
				break;
			}

		case OP::OP_CONSTRUCT_OBJECT:
		case OP::OP_CONSTRUCT_OBJECT__WRITE:
			{
				debug_origin=i.next().origin;
				Value& vclass_name=*i.next().value;
				const String& class_name=*vclass_name.get_string(); debug_name=&class_name;

				DEBUG_PRINT_STRING(class_name)

				Value* class_value=get_class(class_name);
				if(!class_value)
					throw Exception(PARSER_RUNTIME,
						&class_name,
						"class is undefined"); 

				debug_origin=i.next().origin;
				Value& vconstructor_name=*i.next().value;
				const String& constructor_name=*vconstructor_name.get_string(); debug_name=&constructor_name;

				DEBUG_PRINT_STRING(constructor_name)

				Junction* constructor_junction=get_element(*class_value, constructor_name).get_junction();
				if(!constructor_junction)
					throw Exception(PARSER_RUNTIME,
						&constructor_name,
						"constructor must be declared in class '%s'", 
						class_value->get_class()->name_cstr());

				ArrayOperation* local_ops=i.next().ops;
				DEBUG_PRINT_OPS(local_ops)
				DEBUG_PRINT_STR("->\n")

				Value &object=construct(*class_value, *constructor_junction->method);
				VConstructorFrame frame(*constructor_junction->method, method_frame, object);
				METHOD_FRAME_ACTION(op_call(frame));
				object.enable_default_setter();

				if(opcode==OP::OP_CONSTRUCT_OBJECT)
					stack.push(frame.result().as_value());
				else
					write_pass_lang(frame.result());

				DEBUG_PRINT_STR("<-returned")
				break;
			}

		// expression ops: unary
		case OP::OP_NEG:
			{
				Value& a=stack.pop().value();
				Value& value=*new VDouble(-a.as_double());
				stack.push(value);
				break;
			}
		case OP::OP_INV:
			{
				Value& a=stack.pop().value();
				Value& value=*new VDouble(~a.as_int());
				stack.push(value);
				break;
			}
		case OP::OP_NOT:
			{
				Value& a=stack.pop().value();
				Value& value=VBool::get(!a.as_bool());
				stack.push(value);
				break;
			}
		case OP::OP_DEF:
			{
				Value& a=stack.pop().value();
				Value& value=VBool::get(a.is_defined());
				stack.push(value);
				break;
			}
		case OP::OP_IN:
			{
				Value& a=stack.pop().value();
				const String& path=a.as_string();
				Value& value=VBool::get(request_info.uri && *request_info.uri && path.this_starts(request_info.uri));
				stack.push(value);
				break;
			}
		case OP::OP_FEXISTS:
			{
				Value& a=stack.pop().value();
				Value& value=VBool::get(file_exist(absolute(a.as_string())));
				stack.push(value);
				break;
			}
		case OP::OP_DEXISTS:
			{
				Value& a=stack.pop().value();
				Value& value=VBool::get(dir_exists(absolute(a.as_string())));
				stack.push(value);
				break;
			}

		// expression ops: binary
		case OP::OP_SUB: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VDouble(a.as_double() - b.as_double());
				stack.push(value);
				break;
			}
		case OP::OP_ADD: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VDouble(a.as_double() + b.as_double());
				stack.push(value);
				break;
			}
		case OP::OP_MUL: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VDouble(a.as_double() * b.as_double());
				stack.push(value);
				break;
			}
		case OP::OP_DIV: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();

				double a_double=a.as_double();
				double b_double=b.as_double();

				if(b_double == 0) {
					//const String* problem_source=b.as_string();
					throw Exception("number.zerodivision",
						0, //problem_source,
						"Division by zero");
				}

				Value& value=*new VDouble(a_double / b_double);
				stack.push(value);
				break;
			}
		case OP::OP_MOD: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();

				double a_double=a.as_double();
				double b_double=b.as_double();

				if(b_double == 0) {
					//const String* problem_source=b.as_string();
					throw Exception("number.zerodivision",
						0, //problem_source,
						"Modulus by zero");
				}

				Value& value=*new VDouble(fmod(a_double, b_double));
				stack.push(value);
				break;
			}
		case OP::OP_INTDIV:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();

				int a_int=a.as_int();
				int b_int=b.as_int();

				if(b_int == 0) {
					//const String* problem_source=b.as_string();
					throw Exception("number.zerodivision",
						0, //problem_source,
						"Division by zero");
				}

				Value& value=*new VInt(a_int / b_int);
				stack.push(value);
				break;
			}
		case OP::OP_BIN_SL:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() <<
					b.as_int());
				stack.push(value);
				break;
			}
		case OP::OP_BIN_SR:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() >>
					b.as_int());
				stack.push(value);
				break;
			}
		case OP::OP_BIN_AND:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() &
					b.as_int());
				stack.push(value);
				break;
			}
		case OP::OP_BIN_OR:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() |
					b.as_int());
				stack.push(value);
				break;
			}
		case OP::OP_BIN_XOR:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() ^
					b.as_int());
				stack.push(value);
				break;
			}
		case OP::OP_LOG_AND:
			{
				ArrayOperation& local_ops=stack.pop().ops();  Value& a=stack.pop().value();
				bool result;
				if(a.as_bool()) {
					execute(local_ops);
					Value& b=stack.pop().value();
					result=b.as_bool();
				} else
					result=false;
				Value& value=VBool::get(result);
				stack.push(value);
				break;
			}
		case OP::OP_LOG_OR:
			{
				ArrayOperation& local_ops=stack.pop().ops();  Value& a=stack.pop().value();
				bool result;
				if(a.as_bool()) 
					result=true;
				else {
					execute(local_ops);
					Value& b=stack.pop().value();
					result=b.as_bool();
				}
				Value& value=VBool::get(result);
				stack.push(value);
				break;
			}
		case OP::OP_LOG_XOR:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=VBool::get(a.as_bool() ^ b.as_bool());
				stack.push(value);
				break;
			}
		case OP::OP_NUM_LT: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=VBool::get(a_double<b_double);
				stack.push(value);
				break;
			}
		case OP::OP_NUM_GT: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=VBool::get(a_double>b_double);
				stack.push(value);
				break;
			}
		case OP::OP_NUM_LE: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=VBool::get(a_double<=b_double);
				stack.push(value);
				break;
			}
		case OP::OP_NUM_GE: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=VBool::get(a_double>=b_double);
				stack.push(value);
				break;
			}
		case OP::OP_NUM_EQ: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=VBool::get(a_double==b_double);
				stack.push(value);
				break;
			}
		case OP::OP_NUM_NE: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=VBool::get(a_double!=b_double);
				stack.push(value);
				break;
			}
		case OP::OP_STR_LT: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=VBool::get(a.as_string() < b.as_string());
				stack.push(value);
				break;
			}
		case OP::OP_STR_GT: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=VBool::get(a.as_string() > b.as_string());
				stack.push(value);
				break;
			}
		case OP::OP_STR_LE: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=VBool::get(a.as_string() <= b.as_string());
				stack.push(value);
				break;
			}
		case OP::OP_STR_GE: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=VBool::get(a.as_string() >= b.as_string());
				stack.push(value);
				break;
			}
		case OP::OP_STR_EQ: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=VBool::get(a.as_string() == b.as_string());
				stack.push(value);
				break;
			}
		case OP::OP_STR_NE: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=VBool::get(a.as_string() != b.as_string());
				stack.push(value);
				break;
			}
		case OP::OP_IS:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=VBool::get(a.is(b.as_string().cstr()));
				stack.push(value);
				break;
			}

		default:
			throw Exception(0,
				0,
				"invalid opcode %d", opcode); 
		}
	}
	} catch(const Exception&) {
		// record it to stack trace
		if(debug_name)
			exception_trace.push(Trace(debug_name, debug_origin));
		rethrow;
	}
}

#define SAVE_CONTEXT								\
	VMethodFrame *saved_method_frame=method_frame;	\
	Value* saved_rcontext=rcontext;					\
	WContext *saved_wcontext=wcontext;

#define RESTORE_CONTEXT					\
	wcontext=saved_wcontext;			\
	rcontext=saved_rcontext;			\
	method_frame=saved_method_frame;


Value& Request::construct(Value &class_value, const Method &method){
	VStateless_class& called_class=*class_value.get_class();

	if(method.call_type!=Method::CT_STATIC) {
		// this is a constructor call
		if(Value* result=called_class.create_new_value(fpool)) {
				// some stateless_class creatable derivates
				return *result;
		} else 
			throw Exception(PARSER_RUNTIME,
				0, //&frame.name(),
				"is not a constructor, system class '%s' can be constructed only implicitly", 
					called_class.name().cstr());
	} else
		throw Exception(PARSER_RUNTIME,
			0, //&frame.name(),
				"method is static and can not be used as constructor");
}

void Request::op_call(VMethodFrame& frame){
	// see OP_PREPARE_TO_EXPRESSION
	frame.set_in_expression(wcontext->get_in_expression());
				
	SAVE_CONTEXT

	rcontext=wcontext=method_frame=&frame;

	Value& self=frame.self();
	const Method& method=frame.method;
	Method::Call_type call_type=self.get_class()==&self ? Method::CT_STATIC : Method::CT_DYNAMIC;

	if(method.call_type==Method::CT_ANY || method.call_type==call_type) { // allowed call type?
		if(method.native_code) { // native code?
			method.check_actual_numbered_params(self, frame.numbered_params());
			method.native_code(*this, *frame.numbered_params()); // execute it
		} else // parser code, execute it
			recoursion_checked_execute(*method.parser_code);
	} else
		throw Exception(PARSER_RUNTIME,
			0,
			"is not allowed to be called %s", 
			call_type==Method::CT_STATIC?"statically":"dynamically");

	RESTORE_CONTEXT
}

void Request::op_call_write(VMethodFrame& frame){
	VMethodFrame *saved_method_frame=method_frame;
	Value* saved_rcontext=rcontext;

	rcontext=&frame;
	method_frame=&frame;

	Value& self=frame.self();
	const Method& method=frame.method;
	Method::Call_type call_type=self.get_class()==&self ? Method::CT_STATIC : Method::CT_DYNAMIC;

	if(method.call_type==Method::CT_ANY || method.call_type==call_type) { // allowed call type?
		if(method.native_code) { // native code?
			method.check_actual_numbered_params(self, frame.numbered_params());
			method.native_code(*this, *frame.numbered_params()); // execute it
		} else // parser code, execute it
			recoursion_checked_execute(*method.parser_code);
	} else
		throw Exception(PARSER_RUNTIME,
			0,
			"is not allowed to be called %s", 
			call_type==Method::CT_STATIC?"statically":"dynamically");
	
	rcontext=saved_rcontext;
	method_frame=saved_method_frame;
}

Value& Request::get_element(Value& ncontext, const String& name) {
	if(wcontext->get_somebody_entered_some_class()) // ^class:method
		if(VStateless_class *called_class=ncontext.get_class())
			if(VStateless_class *read_class=rcontext->get_class())
				if(read_class->derived_from(*called_class)){ // current derived from called
					Value *value=called_class->get_element(get_self(), name);
					return *(value ? &process_to_value(*value) : VVoid::get());
				}

	Value* value=ncontext.get_element(name);

	return *(value ? &process_to_value(*value) : VVoid::get());
}

void Request::put_element(Value& ncontext, const String& name, Value* value) {
	// put_element can return property-setting-junction
	if(const VJunction* vjunction=ncontext.put_element(name, value, false))
		if(vjunction!=PUT_ELEMENT_REPLACED_ELEMENT) {
			const Junction& junction = vjunction->junction();
			VConstructorFrame frame(*junction.method, method_frame /*caller*/, junction.self);

			size_t param_count=frame.method_params_count();

 			if(junction.auto_name){ 
				// default setter
 				if(param_count!=2)
 					throw Exception(PARSER_RUNTIME,
 						0,
 						"default setter method must have TWO parameters (has %d parameters)", param_count);

				Value* params[2] = { new VString(*junction.auto_name), value };
				frame.store_params(params, 2);

				Temp_disable_default_setter temp(junction.self);
				execute_method(frame);
 			} else {
				// setter
 				if(param_count!=1)
 					throw Exception(PARSER_RUNTIME,
 						0,
 						"setter method must have ONE parameter (has %d parameters)", param_count);

				frame.store_params(&value, 1);
				execute_method(frame);
			}
		}
}

StringOrValue Request::process_getter(Junction& junction) {
	VMethodFrame frame(*junction.method, method_frame/*caller*/, junction.self);
	size_t param_count=frame.method_params_count();

	if(junction.auto_name){ 
		// default getter
		Value *param;

		if(param_count){
			if(param_count>1)
				throw Exception(PARSER_RUNTIME,
					0,
					"default getter method can't have more then 1 parameter (has %d parameters)", param_count);
			param=new VString(*junction.auto_name);
			frame.store_params(&param, 1);
		} // no need for else frame.empty_params()

		Temp_disable_default_getter temp(junction.self);
		execute_method(frame);
	} else {
		// getter
		if(param_count!=0)
			throw Exception(PARSER_RUNTIME,
				0,
				"getter method must have no parameters (has %d parameters)", param_count);
		
		// no need for frame.empty_params()
		execute_method(frame);
	}

	return frame.result();
}

/**	@param intercept_string
	- true:
		they want result=string value, 
		possible object result goes to wcontext
	- false:
		they want any result[string|object]
		nothing goes to wcontext.
		used in @c (expression) params evaluation

	using the fact it's either string_ or value_ result requested to speed up checkes
*/

StringOrValue Request::process(Value& input_value, bool intercept_string) {
	Junction* junction=input_value.get_junction();
	if(junction) {
		if(junction->is_getter) { // is it a getter-junction?
			return process_getter(*junction);
		}

		if(junction->code) { // is it a code-junction?
			// process it
			StringOrValue result;

			DEBUG_PRINT_STR("ja->\n")

			if(!junction->method_frame)
				throw Exception(PARSER_RUNTIME,
					0,
					"junction used outside of context");

			SAVE_CONTEXT

			method_frame=junction->method_frame;
			rcontext=junction->rcontext;

			// for expression method params
			// wcontext is set 0
			// using the fact in decision "which wwrapper to use"
			bool using_code_frame=intercept_string && junction->wcontext;
			if(using_code_frame) {
				// almost plain wwrapper about junction wcontext 

				VCodeFrame local(*junction->wcontext);
				wcontext=&local;

				// execute it
				recoursion_checked_execute(*junction->code);

				// CodeFrame soul:
				result=wcontext->result();
			} else {
				// plain wwrapper
				WWrapper local(wcontext);
				wcontext=&local;

				// execute it
				recoursion_checked_execute(*junction->code);

				result=wcontext->result();
			}

			RESTORE_CONTEXT

			DEBUG_PRINT_STR("<-ja returned")

			return result;
		}

		// it is then method-junction, do not explode it
		// just return it as we do for usual objects
	}	

	return input_value;
}

void Request::process_write(Value& input_value) {
	Junction* junction=input_value.get_junction();
	if(junction) {
		if(junction->is_getter) { // is it a getter-junction?
			write_pass_lang(process_getter(*junction));
			return;
		}

		if(junction->code) { // is it a code-junction?
							// process it

			DEBUG_PRINT_STR("ja->\n")

			if(!junction->method_frame)
				throw Exception(PARSER_RUNTIME,
					0,
					"junction used outside of context");

			SAVE_CONTEXT

			method_frame=junction->method_frame;
			rcontext=junction->rcontext;

			// for expression method params
			// wcontext is set 0
			// using the fact in decision "which wwrapper to use"
#ifdef OPTIMIZE_CALL
			if(wcontext==junction->wcontext){
				// no wrappers for wcontext
				recoursion_checked_execute(*junction->code);
				RESTORE_CONTEXT

			} else
#endif
			if(junction->wcontext) {
				// almost plain wwrapper about junction wcontext 
				VCodeFrame local(*junction->wcontext);
				wcontext=&local;

				// execute it
				recoursion_checked_execute(*junction->code);
				RESTORE_CONTEXT
				write_pass_lang(local.result());
			} else {
				// plain wwrapper
				WWrapper local(wcontext);
				wcontext=&local;

				// execute it
				recoursion_checked_execute(*junction->code);
				RESTORE_CONTEXT
				write_pass_lang(local.result());
			}

			DEBUG_PRINT_STR("<-ja returned")

			return;
		}

		// it is then method-junction, do not explode it
		// just return it as we do for usual objects
	}

	write_pass_lang(input_value);
}

void Request::execute_method(VMethodFrame& aframe) {
	SAVE_CONTEXT
	
	// initialize contexts
	rcontext=wcontext=method_frame=&aframe;
	
	// execute!	
	recoursion_checked_execute(*aframe.method.parser_code);
	
	RESTORE_CONTEXT
}

const String* Request::execute_method(Value& aself, 
					const Method& method, Value* optional_param,
					bool do_return_string) {

	VMethodFrame local_frame(method, method_frame/*caller*/, aself);

	if(optional_param && local_frame.method_params_count()>0) {
		local_frame.store_params(&optional_param, 1);
	} else {
		local_frame.empty_params();
	}

	// prevent non-string writes for better error reporting
	if(do_return_string)
		local_frame.write(local_frame);
	
	execute_method(local_frame);
	
	return do_return_string ? local_frame.get_string() : 0;
}

Request::Execute_nonvirtual_method_result 
Request::execute_nonvirtual_method(VStateless_class& aclass, 
					const String& method_name,
					VString* optional_param,
					bool do_return_string) {
	Execute_nonvirtual_method_result result;
	result.method=aclass.get_method(method_name);
	if(result.method)
		result.string=execute_method(aclass, *result.method, optional_param, do_return_string);
	return result;
}

const String* Request::execute_virtual_method(Value& aself, 
					const String& method_name) {
	if(Value* value=aself.get_element(method_name))
		if(Junction* junction=value->get_junction())
			if(const Method *method=junction->method) 
				return execute_method(aself, *method, 0/*no params*/, true);
			
	return 0;
}

const String* Request::get_method_filename(const Method* method){
	if(ArrayOperation* code=method->parser_code)
		if(code){
			Operation::Origin origin={0, 0, 0};
			Array_iterator<Operation> i(*code);
			while( i.has_next() ){
				switch( i.next().code ){
					case OP::OP_CURLY_CODE__STORE_PARAM: 
					case OP::OP_EXPR_CODE__STORE_PARAM:
					case OP::OP_CURLY_CODE__CONSTRUCT:
					case OP::OP_NESTED_CODE:
					case OP::OP_OBJECT_POOL:
					case OP::OP_STRING_POOL:
					case OP::OP_CALL:
					case OP::OP_CALL__WRITE:
						{
							i.next(); // skip local ops
							i.next(); // skip next opcode
							// continue execution
						}
					case OP::OP_CONSTRUCT_OBJECT:
					case OP::OP_CONSTRUCT_OBJECT__WRITE:
					case OP::OP_VALUE:
					case OP::OP_STRING__WRITE:
					case OP::OP_VALUE__GET_CLASS:
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
					case OP::OP_GET_OBJECT_ELEMENT:
					case OP::OP_GET_OBJECT_ELEMENT__WRITE:
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
					case OP::OP_GET_OBJECT_VAR_ELEMENT:
					case OP::OP_GET_OBJECT_VAR_ELEMENT__WRITE:
#endif
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
					case OP::OP_VALUE__GET_ELEMENT:
					case OP::OP_VALUE__GET_ELEMENT__WRITE:
					case OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR:
					case OP::OP_WITH_ROOT__VALUE__GET_ELEMENT:
#endif
#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
					case OP::OP_WITH_SELF__VALUE__GET_ELEMENT:
					case OP::OP_WITH_SELF__VALUE__GET_ELEMENT__WRITE:
#endif
#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
					case OP::OP_WITH_ROOT__VALUE__CONSTRUCT_EXPR:
					case OP::OP_WITH_ROOT__VALUE__CONSTRUCT_VALUE:
					case OP::OP_WITH_WRITE__VALUE__CONSTRUCT_EXPR:
					case OP::OP_WITH_WRITE__VALUE__CONSTRUCT_VALUE:
					case OP::OP_WITH_SELF__VALUE__CONSTRUCT_EXPR:
					case OP::OP_WITH_SELF__VALUE__CONSTRUCT_VALUE:
#endif
						{
							origin=i.next().origin;
							break;
						}
				}
				if(origin.file_no)
					return get_used_filename(origin.file_no);
			}
		}
	return 0;
}

