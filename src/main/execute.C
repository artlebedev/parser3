/** @file
	Parser: executor part of request class.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_EXECUTE_C="$Date: 2003/11/20 16:42:50 $";

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
	"GET_ELEMENT_OR_OPERATOR", "GET_ELEMENT",	"GET_ELEMENT__WRITE",
	"OBJECT_POOL",	"STRING_POOL",
	"STORE_PARAM",
	"PREPARE_TO_CONSTRUCT_OBJECT",	"PREPARE_TO_EXPRESSION", 
	"CALL", "CALL__WRITE",

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
		OPCODE opcode=i.next().code;

		if(opcode==OP_VALUE || opcode==OP_STRING__WRITE) {
			Operation::Origin origin=i.next().origin;
			Value& value=*i.next().value;
			debug_printf(sapi_info, 
				"%*s%s"
				" \"%s\" %s", 
				level*4, "", opcode_name[opcode],
				value.get_string()->cstr(), value.type());
			continue;
		}
		debug_printf(sapi_info, "%*s%s", level*4, "", opcode_name[opcode]);

		switch(opcode) {
		case OP_CURLY_CODE__STORE_PARAM: 
		case OP_EXPR_CODE__STORE_PARAM:
		case OP_CURLY_CODE__CONSTRUCT:
		case OP_NESTED_CODE:
		case OP_OBJECT_POOL:  
		case OP_STRING_POOL:
		case OP_CALL:
		case OP_CALL__WRITE:
			if(ArrayOperation* local_ops=i.next().ops)
				debug_dump(sapi_info, level+1, *local_ops);
		}
	}
}
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
		if(get_interrupted()) {
			set_interrupted(false);
			throw Exception("parser.interrupted",
				0,
				"execution stopped");
		}
		OPCODE opcode=i.next().code;

#ifdef DEBUG_EXECUTE
		debug_printf(sapi_info, "%d:%s", stack.top_index()+1, opcode_name[opcode]);
#endif

		switch(opcode) {
		// param in next instruction
		case OP_VALUE:
			{
				debug_origin=i.next().origin;
				Value& value=*i.next().value;
#ifdef DEBUG_EXECUTE
				debug_printf(sapi_info, " \"%s\" %s", value.get_string()->cstr(), value.type());
#endif
				stack.push(value);
				break;
			}
		case OP_GET_CLASS:
			{
				// maybe they do ^class:method[] call, remember the fact
				wcontext->set_somebody_entered_some_class();

				const String& name=stack.pop().string();
				Value* value=classes().get(name);
				if(!value) 
					throw Exception("parser.runtime",
						&name,
						"class is undefined"); 

				stack.push(*value);
				break;
			}
			
		// OP_WITH
		case OP_WITH_ROOT:
			{
				stack.push(*method_frame);
				break;
			}
		case OP_WITH_SELF: 
			{
				stack.push(get_self());
				break;
			}
		case OP_WITH_READ: 
			{
				stack.push(*rcontext);
				break;
			}
		case OP_WITH_WRITE: 
			{
				if(wcontext==method_frame)
					throw Exception("parser.runtime",
						0,
						"$.name outside of $name[...]");

				stack.push(*wcontext);
				break;
			}
			
		// OTHER ACTIONS BUT WITHs
		case OP_CONSTRUCT_VALUE:
			{
				Value& value=stack.pop().value();
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				ncontext.put_element(name, &value, false);
				break;
			}
		case OP_CONSTRUCT_EXPR:
			{
				// see OP_PREPARE_TO_EXPRESSION
				wcontext->set_in_expression(false);

				Value& value=stack.pop().value();
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				ncontext.put_element(name, &value.as_expr_result(), false);
				break;
			}
		case OP_CURLY_CODE__CONSTRUCT:
			{
				ArrayOperation& local_ops=*i.next().ops;
#ifdef DEBUG_EXECUTE
				debug_printf(sapi_info, " (%d)\n", local_ops.count());
				debug_dump(sapi_info, 1, local_ops);
#endif				
				Value& value=*new VJunction(new Junction( 
					get_self(), 0,
					method_frame, 
					rcontext, 
					wcontext, 
					&local_ops));

				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				ncontext.put_element(name, &value, false);
				break;
			}
		case OP_NESTED_CODE:
			{
				ArrayOperation& local_ops=*i.next().ops;
#ifdef DEBUG_EXECUTE
				debug_printf(sapi_info, " (%d)\n", local_ops.count());
				debug_dump(sapi_info, 1, local_ops);
#endif				
				stack.push(local_ops);
				break;
			}
		case OP_WRITE_VALUE:
			{
				Value& value=stack.pop().value();
				write_assign_lang(value);
				break;
			}
		case OP_WRITE_EXPR_RESULT:
			{
				Value& value=stack.pop().value();
				write_no_lang(value.as_expr_result());

				// must be after write(result) and 
				// see OP_PREPARE_TO_EXPRESSION
				wcontext->set_in_expression(false);
				break;
			}
		case OP_STRING__WRITE:
			{
				i.next(); // ignore origin
				Value* value=i.next().value;
#ifdef DEBUG_EXECUTE
				debug_printf(sapi_info, " \"%s\"", value->get_string()->cstr());
#endif
				write_no_lang(*value->get_string());
				break;
			}
			
		case OP_GET_ELEMENT_OR_OPERATOR:
			{
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				Value& value=get_element(ncontext, name, true);
				stack.push(value);
				break;
			}
		case OP_GET_ELEMENT:
			{
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				Value& value=get_element(ncontext, name, false);
				stack.push(value);
				break;
			}

		case OP_GET_ELEMENT__WRITE:
			{
				const String& name=stack.pop().string();  debug_name=&name;
				Value& ncontext=stack.pop().value();
				Value& value=get_element(ncontext, name, false);
				write_assign_lang(value);
				break;
			}


		case OP_OBJECT_POOL:
			{
				ArrayOperation& local_ops=*i.next().ops;
				
				WContext *saved_wcontext=wcontext;
				String::Language saved_lang=flang;
				flang=String::L_PASS_APPENDED;
				WWrapper local(0/*empty*/, wcontext);
				wcontext=&local;

				execute(local_ops);

				stack.push(wcontext->result().as_value());
				flang=saved_lang;
				wcontext=saved_wcontext;
				break;
			}
			
		case OP_STRING_POOL:
			{
				ArrayOperation& local_ops=*i.next().ops;

				WContext *saved_wcontext=wcontext;
				WWrapper local(0 /*empty*/, wcontext);
				wcontext=&local;

				execute(local_ops);

				Value* value;
				// from "$a $b" part of expression taking only string value,
				// ignoring any other content of wcontext
				if(const String* string=wcontext->get_string())
					value=new VString(*string);
				else
					value=new VVoid;
				stack.push(*value);

				wcontext=saved_wcontext;
				break;
			}

		// CALL
		case OP_STORE_PARAM:
			{
				Value& value=stack.pop().value();
				VMethodFrame& frame=stack.top_value().method_frame();
				// this op is executed from CALL local_ops only, so may skip the check "method_frame_to_fill==0"
				frame.store_param(value);
				break;
			}
		case OP_CURLY_CODE__STORE_PARAM:
		case OP_EXPR_CODE__STORE_PARAM:
			{
				// code
				ArrayOperation& local_ops=*i.next().ops;
				VMethodFrame& frame=stack.top_value().method_frame();
#ifdef DEBUG_EXECUTE
				debug_printf(sapi_info, " (%d)\n", local_ops.count());
				debug_dump(sapi_info, 1, local_ops);
#endif				
				// when they evaluate expression parameter,
				// the object expression result
				// does not need to be written into calling frame
				// it must go into any expressions using that parameter
				// hence, we zero junction.wcontext here, and later
				// in .process we would test that field 
				// in decision "which wwrapper to use"
				Value& value=*new VJunction(new Junction(
					get_self(), 0,
					method_frame, 
					rcontext, 
					opcode==OP_EXPR_CODE__STORE_PARAM?0:wcontext, 
					&local_ops));
				// store param
				// this op is executed from CALL local_ops only, so can not check method_frame_to_fill==0
				frame.store_param(value);
				break;
			}

		case OP_PREPARE_TO_CONSTRUCT_OBJECT:
			{
				wcontext->set_constructing(true);
				break;
			}

		case OP_PREPARE_TO_EXPRESSION:
			{
				wcontext->set_in_expression(true);
				break;
			}

		case OP_CALL:
		case OP_CALL__WRITE:
			{
				//is_debug_junction=true;
				ArrayOperation* local_ops=i.next().ops;
#ifdef DEBUG_EXECUTE
				debug_printf(sapi_info, " (%d)\n", local_ops?local_ops->count():0);
				if(local_ops)
					debug_dump(sapi_info, 1, *local_ops);

				debug_printf(sapi_info, "->\n");
#endif
				Value& value=stack.pop().value();

				Junction* junction=value.get_junction();
				if(!junction) {
					if(value.is("void"))
						throw Exception("parser.runtime",
							0,
							"undefined method");
					else
						throw Exception("parser.runtime",
							0,
							"is '%s', not a method or junction, can not call it",
								value.type());
				}
				/* no check needed, code compiled the way that that's impossible
				// check: 
				//	that this is method-junction, not a code-junction
				// [disabling these contstructions:]
				// $junction{code}
				//  ^junction[]
				if(!junction->method)
					throw Exception("parser.runtime",
						
						"is '%s', it is code junction, can not call it",
							value.type());
				*/

				VMethodFrame frame(*junction, method_frame/*caller*/);
				if(local_ops){ // store param code goes here
					stack.push(frame); // argument for *STORE_PARAM ops
					execute(*local_ops);
					stack.pop().value();
				}
				frame.fill_unspecified_params();
				VMethodFrame *saved_method_frame=method_frame;
				Value* saved_rcontext=rcontext;
				WContext *saved_wcontext=wcontext;
				
				VStateless_class& called_class=*frame.junction.self.get_class();
				Value* new_self=&get_self();
				if(wcontext->get_constructing()) {
					wcontext->set_constructing(false);
					if(frame.junction.method->call_type!=Method::CT_STATIC) {
						// this is a constructor call

						if(Value* value=called_class.create_new_value(fpool)) {
							// some stateless_class creatable derivates
							new_self=value;
						} else 
							throw Exception("parser.runtime",
								0, //&frame.name(),
								"is not a constructor, system class '%s' can be constructed only implicitly", 
									called_class.name().cstr());

						frame.write(*new_self, 
							String::L_CLEAN  // not used, always an object, not string
						);
					} else
						throw Exception("parser.runtime",
							0, //&frame.name(),
							"method is static and can not be used as constructor");
				} else
					new_self=&frame.junction.self;

				frame.set_self(*new_self);

				// see OP_PREPARE_TO_EXPRESSION
				frame.set_in_expression(wcontext->get_in_expression());
				
				rcontext=wcontext=&frame;
				{
					const Method& method=*frame.junction.method;
					Method::Call_type call_type=
						&called_class==new_self ? Method::CT_STATIC : Method::CT_DYNAMIC;
					if(
						method.call_type==Method::CT_ANY ||
						method.call_type==call_type) { // allowed call type?
						method_frame=&frame;
						if(method.native_code) { // native code?
							method.check_actual_numbered_params(
								frame.junction.self, 
								/*frame.name(), */frame.numbered_params());
							method.native_code(
								*this, 
								*frame.numbered_params()); // execute it
						} else // parser code, execute it
							recoursion_checked_execute(/*frame.name(), */*method.parser_code);
					} else
						throw Exception("parser.runtime",
							0, //&frame.name(),
							"is not allowed to be called %s", 
								call_type==Method::CT_STATIC?"statically":"dynamically");

				}
				StringOrValue result=wcontext->result();

				wcontext=saved_wcontext;
				rcontext=saved_rcontext;
				method_frame=saved_method_frame;

				if(opcode==OP_CALL__WRITE) {
					write_assign_lang(result);
				} else { // OP_CALL
					stack.push(result.as_value());
				}
				
#ifdef DEBUG_EXECUTE
				debug_printf(sapi_info, "<-returned");
#endif
				//is_debug_junction=false;
				break;
			}

		// expression ops: unary
		case OP_NEG:
			{
				Value& a=stack.pop().value();
				Value& value=*new VDouble(-a.as_double());
				stack.push(value);
				break;
			}
		case OP_INV:
			{
				Value& a=stack.pop().value();
				Value& value=*new VDouble(~a.as_int());
				stack.push(value);
				break;
			}
		case OP_NOT:
			{
				Value& a=stack.pop().value();
				Value& value=*new VBool(!a.as_bool());
				stack.push(value);
				break;
			}
		case OP_DEF:
			{
				Value& a=stack.pop().value();
				Value& value=*new VBool(a.is_defined());
				stack.push(value);
				break;
			}
		case OP_IN:
			{
				Value& a=stack.pop().value();
				const String& path=a.as_string();
				Value& value=*new VBool(request_info.uri && *request_info.uri && path.this_starts(request_info.uri));
				stack.push(value);
				break;
			}
		case OP_FEXISTS:
			{
				Value& a=stack.pop().value();
				Value& value=*new VBool(file_readable(absolute(a.as_string())));
				stack.push(value);
				break;
			}
		case OP_DEXISTS:
			{
				Value& a=stack.pop().value();
				Value& value=*new VBool(dir_readable(absolute(a.as_string())));
				stack.push(value);
				break;
			}

		// expression ops: binary
		case OP_SUB: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VDouble(a.as_double() - b.as_double());
				stack.push(value);
				break;
			}
		case OP_ADD: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VDouble(a.as_double() + b.as_double());
				stack.push(value);
				break;
			}
		case OP_MUL: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VDouble(a.as_double() * b.as_double());
				stack.push(value);
				break;
			}
		case OP_DIV: 
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
		case OP_MOD: 
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
		case OP_INTDIV:
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
		case OP_BIN_SL:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() <<
					b.as_int());
				stack.push(value);
				break;
			}
		case OP_BIN_SR:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() >>
					b.as_int());
				stack.push(value);
				break;
			}
		case OP_BIN_AND:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() &
					b.as_int());
				stack.push(value);
				break;
			}
		case OP_BIN_OR:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() |
					b.as_int());
				stack.push(value);
				break;
			}
		case OP_BIN_XOR:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VInt(
					a.as_int() ^
					b.as_int());
				stack.push(value);
				break;
			}
		case OP_LOG_AND:
			{
				ArrayOperation& local_ops=stack.pop().ops();  Value& a=stack.pop().value();
				bool result;
				if(a.as_bool()) {
					execute(local_ops);
					Value& b=stack.pop().value();
					result=b.as_bool();
				} else
					result=false;
				Value& value=*new VBool(result);
				stack.push(value);
				break;
			}
		case OP_LOG_OR:
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
				Value& value=*new VBool(result);
				stack.push(value);
				break;
			}
		case OP_LOG_XOR:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VBool(a.as_bool() ^ b.as_bool());
				stack.push(value);
				break;
			}
		case OP_NUM_LT: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=*new VBool(a_double<b_double);
				stack.push(value);
				break;
			}
		case OP_NUM_GT: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=*new VBool(a_double>b_double);
				stack.push(value);
				break;
			}
		case OP_NUM_LE: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=*new VBool(a_double<=b_double);
				stack.push(value);
				break;
			}
		case OP_NUM_GE: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=*new VBool(a_double>=b_double);
				stack.push(value);
				break;
			}
		case OP_NUM_EQ: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=*new VBool(a_double==b_double);
				stack.push(value);
				break;
			}
		case OP_NUM_NE: 
			{
				volatile double b_double=stack.pop().value().as_double();
				volatile double a_double=stack.pop().value().as_double();
				Value& value=*new VBool(a_double!=b_double);
				stack.push(value);
				break;
			}
		case OP_STR_LT: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VBool(a.as_string() < b.as_string());
				stack.push(value);
				break;
			}
		case OP_STR_GT: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VBool(a.as_string() > b.as_string());
				stack.push(value);
				break;
			}
		case OP_STR_LE: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VBool(a.as_string() <= b.as_string());
				stack.push(value);
				break;
			}
		case OP_STR_GE: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VBool(a.as_string() >= b.as_string());
				stack.push(value);
				break;
			}
		case OP_STR_EQ: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VBool(a.as_string() == b.as_string());
				stack.push(value);
				break;
			}
		case OP_STR_NE: 
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VBool(a.as_string() != b.as_string());
				stack.push(value);
				break;
			}
		case OP_IS:
			{
				Value& b=stack.pop().value();  Value& a=stack.pop().value();
				Value& value=*new VBool(a.is(b.as_string().cstr()));
				stack.push(value);
				break;
			}

		default:
			throw Exception(0,
				0,
				"invalid opcode %d", opcode); 
		}
	}
	} catch(...) {
		// record it to stack trace
		if(debug_name)
			exception_trace.push(Trace(debug_name, debug_origin));
		rethrow;
	}
}

/**
	@todo cache|prepare junctions
	@bug ^superbase:method would dynamically call ^base:method if there is any
*/
Value& Request::get_element(Value& ncontext, const String& name, bool can_call_operator) {
	Value* value=0;
	if(can_call_operator) {
		if(Method* method=main_class.get_method(name)) // looking operator of that name FIRST
			value=new VJunction(new Junction(
				main_class, method, 0,0,0, 0));
	}
	if(!value) {
		if(!wcontext->get_constructing() // not constructing
			&& wcontext->get_somebody_entered_some_class() ) // ^class:method
			if(VStateless_class *called_class=ncontext.get_class())
				if(VStateless_class *read_class=rcontext->get_class())
					if(read_class->derived_from(*called_class)) // current derived from called
						if(Value* base=get_self().base()) { // doing DYNAMIC call
							Temp_derived temp_derived(*base, 0); // temporarily prevent go-back-down virtual calls
							value=base->get_element(name, *base, false); // virtual-up lookup starting from parent
							goto value_ready;
						}
	}
	if(!value)
		value=ncontext.get_element(name, ncontext, false);

	if(value && wcontext->get_constructing())
		if(Junction* junction=value->get_junction()) {
			if(junction->self.get_class()!=&ncontext)
				throw Exception("parser.runtime",
					&name,
					"constructor must be declared in class '%s'", 
						ncontext.get_class()->name_cstr());
		}

value_ready:
	if(value)
		value=&process_to_value(*value); // process possible code-junction
	else
		value=new VVoid;

	return *value;
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
	StringOrValue result;
	Junction* junction=input_value.get_junction();
	if(junction && junction->code) { // is it a code-junction?
		// process it
#ifdef DEBUG_EXECUTE
		debug_printf(sapi_info, "ja->\n");
#endif

		if(!junction->method_frame)
			throw Exception("parser.runtime",
				0,
				"junction used outside of context");

		VMethodFrame *saved_method_frame=method_frame;  
		Value* saved_rcontext=rcontext;  
		WContext *saved_wcontext=wcontext;
		
		method_frame=junction->method_frame;
		rcontext=junction->rcontext;

		// for expression method params
		// wcontext is set 0
		// using the fact in decision "which wwrapper to use"
		bool using_code_frame=intercept_string && junction->wcontext;
		if(using_code_frame) {
			// almost plain wwrapper about junction wcontext, 
			// BUT intercepts string writes
			VCodeFrame local(*junction->wcontext, junction->wcontext);
			wcontext=&local;

			// execute it
			recoursion_checked_execute(*junction->code);
			
			// CodeFrame soul:
			//   string writes were intercepted
			//   returning them as the result of getting code-junction
			result.set_string(*wcontext->get_string());
		} else {
			// plain wwrapper
			WWrapper local(0/*empty*/, wcontext);
			wcontext=&local;
		
			// execute it
			recoursion_checked_execute(*junction->code);
		
			result=wcontext->result();
		}
		
		wcontext=saved_wcontext;
		rcontext=saved_rcontext;
		method_frame=saved_method_frame;
		
#ifdef DEBUG_EXECUTE
		debug_printf(sapi_info, "<-ja returned");
#endif
	} else {
		result.set_value(input_value);
	}
	return result;
}

StringOrValue Request::execute_method(VMethodFrame& amethod_frame, const Method& method) {
	VMethodFrame *saved_method_frame=method_frame;  
	Value* saved_rcontext=rcontext;  
	WContext *saved_wcontext=wcontext;
	
	// initialize contexts
	rcontext=wcontext=method_frame=&amethod_frame;
	
	// execute!	
	execute(*method.parser_code);
	
	// result
	StringOrValue result=wcontext->result();
	
	wcontext=saved_wcontext;
	rcontext=saved_rcontext;
	method_frame=saved_method_frame;
	
	// return
	return result;
}

const String* Request::execute_method(Value& aself, 
				      const Method& method, VString* optional_param,
				      bool do_return_string) {
	VMethodFrame *saved_method_frame=method_frame;  
	Value* saved_rcontext=rcontext;  
	WContext *saved_wcontext=wcontext;
	
	Junction local_junction(aself, &method, 0,0,0, 0);
	VMethodFrame local_frame(local_junction, method_frame/*caller*/);
	if(optional_param && local_frame.can_store_param()) {
		local_frame.store_param(*optional_param);
		local_frame.fill_unspecified_params();
	}
	local_frame.set_self(aself);
	rcontext=wcontext=method_frame=&local_frame; 

	// prevent non-string writes for better error reporting
	if(do_return_string)
		wcontext->write(local_frame);
	
	// execute!	
	execute(*method.parser_code);
	
	// result
	const String* result=0;
	if(do_return_string)
		result=&wcontext->result().as_string();
	
	wcontext=saved_wcontext;
	rcontext=saved_rcontext;
	method_frame=saved_method_frame;

	return result;
}

Request::Execute_nonvirtual_method_result 
Request::execute_nonvirtual_method(VStateless_class& aclass, 
				   const String& method_name, VString* optional_param,
				   bool do_return_string) {
	Execute_nonvirtual_method_result result;
	result.method=aclass.get_method(method_name);
	if(result.method)
		result.string=execute_method(aclass, *result.method, optional_param, 
			do_return_string);
	return result;
}

const String* Request::execute_virtual_method(Value& aself, 
					      const String& method_name) {
	if(Value* value=aself.get_element(method_name, aself, false))
		if(Junction* junction=value->get_junction())
			if(const Method *method=junction->method) 
				return execute_method(aself, *method, 0/*no params*/, true);
			
	return 0;
}
