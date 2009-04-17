/**	@file
	Parser: method frame class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

static const char * const IDENT_VSTATELESS_CLASS_C="$Date: 2009/04/17 09:56:21 $";

#include "pa_vmethod_frame.h"
#include "pa_request.h"

// globals

const String result_var_name(RESULT_VAR_NAME);
const uint result_var_hash_code(hash_code(result_var_name));

// MethodParams: methods

Value& MethodParams::get_processed(Value* value, const char* msg, int index, Request& r) {
		return r.process_to_value(as_junction(value, msg, index), false /*do not intercept string*/);
}

// VMethodFrame: methods

VMethodFrame::VMethodFrame(
	const Junction& ajunction/*info: always method-junction*/,
	VMethodFrame *acaller) : 
	WContext(0/*empty*/, 0 /* no parent, junctions can be reattached only up to VMethodFrame */),

	fcaller(acaller),

	store_param_index(0),
	my(0),
	fself(0),

	junction(ajunction) {

	put_element_impl=(junction.method->all_vars_local)?&VMethodFrame::put_element_local:&VMethodFrame::put_element_global;

	if(!junction.method->max_numbered_params_count){ // this method uses numbered params?
		my=new HashStringValue;

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
		{ // always there is one local: $result
			set_my_variable(result_var_name, *VVoid::get());
		}
	}
}

Value* VMethodFrame::get_result_variable() {
	if(!my)
		return 0;

	Value* result=my->get_by_hash_code(result_var_hash_code, result_var_name);
	return result!=VVoid::get()?result:0;
}
