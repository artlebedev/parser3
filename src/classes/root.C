/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: root.C,v 1.17 2001/03/11 21:41:03 paf Exp $
*/

#include "pa_request.h"
#include "_root.h"

static void _if(Request& r, Array *params) {
	bool condition=
		r.autocalc(
			*static_cast<Value *>(params->get(0)), 
			0/*no name*/,
			false/*don't intercept string*/).get_bool();
	if(condition) {
		Value& value=r.autocalc(*static_cast<Value *>(params->get(1)));
		r.wcontext->write(value, String::Untaint_lang::PASS_APPENDED);
	} else if(params->size()==3) {
		Value& value=r.autocalc(*static_cast<Value *>(params->get(2)));
		r.wcontext->write(value, String::Untaint_lang::PASS_APPENDED);
	}
}

static void _untaint(Request& r, Array *params) {
	const String& lang_name=r.autocalc(*static_cast<Value *>(params->get(0))).as_string();
	String::Untaint_lang lang=static_cast<String::Untaint_lang>(
		untaint_lang_name_to_enum->get_int(lang_name));
	if(!lang)
		R_THROW(0, 0,
			&lang_name,
			"invalid language");

	Temp_lang temp_lang(r, lang);
	Value *value=static_cast<Value *>(params->get(1));
	// forcing ^untaint[]{param type}
	if(!value->get_junction())
		R_THROW(0, 0,
			&value->as_string(),
			"untaint body must be junction");

	value=&r.autocalc(*value);
	r.wcontext->write(*value, String::Untaint_lang::PASS_APPENDED);
}
	

void initialize_root_class(Pool& pool, VClass& vclass) {
	// ^if(condition){code-when-true}
	// ^if(condition){code-when-true}{code-when-false}
	vclass.add_native_method("if", _if, 2, 3);

	// ^untaint[as-is|sql|js|html|html-typo]{code}
	vclass.add_native_method("untaint", _untaint, 2, 2);
}
