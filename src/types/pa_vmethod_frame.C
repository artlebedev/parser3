/**	@file
	Parser: method frame class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

#include "pa_vmethod_frame.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VMETHOD_FRAME_C="$Id: pa_vmethod_frame.C,v 1.21 2012/06/08 11:44:02 misha Exp $" IDENT_PA_VMETHOD_FRAME_H;

// globals

const String result_var_name(RESULT_VAR_NAME), caller_element_name(CALLER_ELEMENT_NAME), self_element_name(SELF_ELEMENT_NAME);
VVoid void_result; // unique value to be sure the result is changed

// MethodParams: methods

Value& MethodParams::get_processed(Value* value, const char* msg, int index, Request& r) {
		return r.process_to_value(as_junction(value, msg, index), false /*do not intercept string*/);
}

HashStringValue* MethodParams::as_hash(int index, const char* name) {
	Value* value=get(index);
	if(value) {
		if(value->get_junction())
			throw Exception(PARSER_RUNTIME,
				0,
				"%s param must not be code (parameter #%d)", name ? name : "options", 1+index);
		if(!value->is_defined()) // empty hash is not defined, but we don't need it anyway
			return 0;
		if(HashStringValue* result=value->get_hash())
			return result;
		if(value->is_string() && value->get_string()->trim().is_empty())
			return 0;
	}
	throw Exception(PARSER_RUNTIME,
		0,
		"%s must have hash representation (parameter #%d)", name ? name : "options", 1+index);
}

Table* MethodParams::as_table(int index, const char* name) {
	Value* value=get(index);
	if(value) {
		if(value->get_junction())
			throw Exception(PARSER_RUNTIME,
				0,
				"%s param must not be code (parameter #%d)", name ? name : "options", 1+index);
		if(Table* result=value->get_table())
			return result;
	}
	throw Exception(PARSER_RUNTIME,
		0,
		"%s param must have table representation (parameter #%d)", name ? name : "options", 1+index);
};

// VMethodFrame: methods

VMethodFrame::VMethodFrame(const Method& amethod, VMethodFrame *acaller, Value& aself) : 
	WContext(0 /* no parent, junctions can be reattached only up to VMethodFrame */),
	fcaller(acaller),
	my(0),
	fself(aself),
	method(amethod) {

	put_element_impl=(method.all_vars_local)?&VMethodFrame::put_element_local:&VMethodFrame::put_element_global;

	if(!method.max_numbered_params_count){ // this method uses numbered params?
		my=new HashString<Value*>;

		if(method.locals_names) { // are there any local var names?
			// remember them
			// those are flags that fname is local == to be looked up in 'my'
			for(Array_iterator<const String*> i(*method.locals_names); i.has_next(); ) {
				// speedup: not checking for clash with "result" fname
				const String& fname=*i.next();
				set_my_variable(fname, *VString::empty());
			}
		}
#ifdef OPTIMIZE_RESULT
		if(method.result_optimization!=Method::RO_USE_WCONTEXT)
#endif
			set_my_variable(result_var_name, void_result);
	}
}

Value* VMethodFrame::get_result_variable() {
	if(!my)
		return 0;

	Value* result=my->get(result_var_name);
	return result!=&void_result?result:0;
}
