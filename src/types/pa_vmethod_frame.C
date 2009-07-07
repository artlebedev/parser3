/**	@file
	Parser: method frame class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

static const char * const IDENT_VSTATELESS_CLASS_C="$Date: 2009/07/07 23:50:06 $";

#include "pa_vmethod_frame.h"
#include "pa_request.h"

// globals

const String result_var_name(RESULT_VAR_NAME), caller_element_name(CALLER_ELEMENT_NAME), self_element_name(SELF_ELEMENT_NAME);
VVoid void_result; // unique value to be sure the result is changed

// MethodParams: methods

Value& MethodParams::get_processed(Value* value, const char* msg, int index, Request& r) {
		return r.process_to_value(as_junction(value, msg, index), false /*do not intercept string*/);
}

// VMethodFrame: methods

VMethodFrame::VMethodFrame(
	const Junction& ajunction/*info: always method-junction*/,
	VMethodFrame *acaller) : 
	WContext(0 /* no parent, junctions can be reattached only up to VMethodFrame */),

	fcaller(acaller),

	my(0),
	fself(0),

	junction(ajunction) {

	put_element_impl=(junction.method->all_vars_local)?&VMethodFrame::put_element_local:&VMethodFrame::put_element_global;

	if(!junction.method->max_numbered_params_count){ // this method uses numbered params?
		my=new HashString<Value*>;

		const Method &method=*junction.method;
		if(method.locals_names) { // are there any local var names?
			// remember them
			// those are flags that fname is local == to be looked up in 'my'
			for(Array_iterator<const String*> i(*method.locals_names); i.has_next(); ) {
				// speedup: not checking for clash with "result" fname
				const String& fname=*i.next();
				set_my_variable(fname, *VVoid::get());
			}
		}
#ifdef OPTIMIZE_RESULT
		if(junction.method->result_optimization!=Method::RO_USE_WCONTEXT)
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
