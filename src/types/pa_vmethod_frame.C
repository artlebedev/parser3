/**	@file
	Parser: method frame class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

static const char * const IDENT_VSTATELESS_CLASS_C="$Date: 2005/08/09 07:37:12 $";

#include "pa_vmethod_frame.h"
#include "pa_request.h"

// globals

const String result_var_name(RESULT_VAR_NAME);

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
	my(0), fnumbered_params(0),
	fself(0),

	fresult_initial_void(0),
	
	junction(ajunction) {

	if(junction.method->max_numbered_params_count) // this method uses numbered params?
		fnumbered_params=new MethodParams;
	else {
		my=new HashStringValue;

		const Method &method=*junction.method;
		if(method.locals_names) { // are there any local var names?
			// remember them
			// those are flags that fname is local == to be looked up in 'my'
			for(Array_iterator<const String*> i(*method.locals_names); i.has_next(); ) {
				// speedup: not checking for clash with "result" fname
				const String& fname=*i.next();
				set_my_variable(fname, *new VVoid);
			}
		}
		{ // always there is one local: $result
			fresult_initial_void=new VVoid;
			set_my_variable(result_var_name, *fresult_initial_void);
		}
	}
}

Value* VMethodFrame::get_result_variable() {
	Value* result=my?my->get(result_var_name):0;
	return result!=fresult_initial_void ? result : 0;
}
