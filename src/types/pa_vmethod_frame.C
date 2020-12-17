/**	@file
	Parser: method frame class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

#include "pa_vmethod_frame.h"
#include "pa_vcaller_wrapper.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VMETHOD_FRAME_C="$Id: pa_vmethod_frame.C,v 1.43 2020/12/17 19:51:21 moko Exp $" IDENT_PA_VMETHOD_FRAME_H IDENT_PA_VCALLER_WRAPPER_H;

static VVoid void_result; // unique value to be sure the result is changed

// MethodParams: methods

const char *skip_name[]={
	"",
	"continue",
	"break",
	"return"
};

Value& MethodParams::get_processed(Value& value, const char* msg, int index, Request& r) {
	if(!value.get_junction())
		throw Exception(PARSER_RUNTIME, 0, "%s (parameter #%d)", msg, 1+index);
	Value& result=r.process(value);
	if(r.get_skip()){
		const char *skip=skip_name[r.get_skip()];
		r.set_skip(Request::SKIP_NOTHING);
		throw Exception(PARSER_RUNTIME, 0, "%s is not allowed in expression passed to native method (parameter #%d)", skip, 1+index);
	}
	return result;
}

// Should be synced with Value::as_hash
HashStringValue* MethodParams::as_hash(int index, const char* name) {
	Value& value=get(index);
	if(value.get_junction())
		throw Exception(PARSER_RUNTIME, 0, "%s param must not be code (parameter #%d)", name ? name : "options", 1+index);
	if(!value.is_defined()) // empty hash is not defined, but we don't need it anyway
		return 0;
	if(HashStringValue* result=value.get_hash())
		return result;
	if(value.is_string() && value.get_string()->trim().is_empty())
		return 0;
	throw Exception(PARSER_RUNTIME, 0, "%s must have hash representation (parameter #%d)", name ? name : "options", 1+index);
}

Table* MethodParams::as_table(int index, const char* name) {
	Value& value=get(index);
	if(value.get_junction())
		throw Exception(PARSER_RUNTIME, 0, "%s param must not be code (parameter #%d)", name ? name : "options", 1+index);
	if(Table* result=value.get_table())
		return result;
	if(value.is_string() && value.get_string()->trim().is_empty())
		return 0;
	throw Exception(PARSER_RUNTIME, 0, "%s param must have table representation (parameter #%d)", name ? name : "options", 1+index);
}

// VMethodFrame: methods

void VNativeMethodFrame::call(Request &r){
	check_call_type();
	method.native_code(r, fnumbered_params);
}

void VParserMethodFrame::call(Request &r){
	check_call_type();
	r.recoursion_checked_execute(*method.parser_code);
	r.check_skip_return();
}

VParserMethodFrame::VParserMethodFrame(const Method& amethod, VMethodFrame *acaller, Value& aself) : VMethodFrame(amethod, acaller, aself) {
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
		set_my_variable(Symbols::RESULT_SYMBOL, void_result);
}

Value* VParserMethodFrame::get_result_variable() {
	Value* result=my.get(Symbols::RESULT_SYMBOL);
	return result!=&void_result ? result : 0;
}

Value* VParserMethodFrame::get_caller_wrapper(){
	static VCallerWrapper *caller_wrapper_template=0;
	if(!caller())
		return 0;
	if(caller_wrapper_template && &caller_wrapper_template->caller() == caller())
		return caller_wrapper_template;
	return caller_wrapper_template=new VCallerWrapper(*caller());
}
