/**	@file
	Parser: method frame class.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>\
*/

#include "pa_vmethod_frame.h"
#include "pa_vcaller_wrapper.h"
#include "pa_request.h"
#include "pa_vfile.h"

volatile const char * IDENT_PA_VMETHOD_FRAME_C="$Id: pa_vmethod_frame.C,v 1.53 2024/12/11 02:47:48 moko Exp $" IDENT_PA_VMETHOD_FRAME_H IDENT_PA_VCALLER_WRAPPER_H;

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
	throw Exception(PARSER_RUNTIME, 0, "%s must have hash representation (parameter #%d is '%s')", name ? name : "options", 1+index, value.type());
}

Table* MethodParams::as_table(int index, const char* name) {
	Value& value=get(index);
	if(value.get_junction())
		throw Exception(PARSER_RUNTIME, 0, "%s param must not be code (parameter #%d)", name ? name : "options", 1+index);
	if(Table* result=value.get_table())
		return result;
	if(value.is_string() && value.get_string()->trim().is_empty())
		return 0;
	throw Exception(PARSER_RUNTIME, 0, "%s param must have table representation (parameter #%d is '%s')", name ? name : "options", 1+index, value.type());
}

const String& MethodParams::as_file_name(int index) {
	const String* result=get(index).get_string();
	if(result && !result->is_empty())
		return *result;
	throw Exception(PARSER_RUNTIME, 0, "%s (parameter #%d is '%s')", FILE_NAME_MUST_BE_NE_STRING, 1+index, get(index).type());
}

const String& MethodParams::as_file_spec(int index) {
	if(VFile* vfile=dynamic_cast<VFile *>(&get(index)))
		return vfile->get_element(name_name)->as_string();
	return as_string(index, FILE_NAME_MUST_BE_STRING_OR_FILE);
}

// VMethodFrame: methods

void VNativeMethodFrame::call(Request &r){
	check_call_type();
	method.native_code(r, fnumbered_params);
}

void VParserMethodFrame::call(Request &r){
	check_call_type();
	r.recursion_checked_execute(*method.parser_code);
	r.check_skip_return();
}

VParserMethodFrame::VParserMethodFrame(const Method& amethod, VMethodFrame *acaller, Value& aself) : VMethodFrame(amethod, acaller, aself), my_result(NULL) {
	if(method.locals_names) { // are there any local var names?
		// remember them, those are flags that fname is local == to be looked up in 'my'
		for(ArrayString::Iterator i(*method.locals_names); i; ) {
			// "result" excluded from local variables during compilation, no need to call set_my_variable
			my.put(*i.next(), VString::empty());
		}
	}
}

Value* VParserMethodFrame::get_caller_wrapper(){
	static VCallerWrapper *caller_wrapper_template=0;
	if(!caller())
		return 0;
	if(caller_wrapper_template && &caller_wrapper_template->caller() == caller())
		return caller_wrapper_template;
	return caller_wrapper_template=new VCallerWrapper(*caller());
}
