/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: root.C,v 1.20 2001/03/12 09:35:01 paf Exp $
*/

#include <string.h>

#include "pa_request.h"
#include "_root.h"

static void _if(Request& r, const String&, Array *params) {
	bool condition=
		r.process(
			*static_cast<Value *>(params->get(0)), 
			0/*no name*/,
			false/*don't intercept string*/).get_bool();
	if(condition) {
		Value& value=r.process(*static_cast<Value *>(params->get(1)));
		r.wcontext->write(value, String::Untaint_lang::PASS_APPENDED);
	} else if(params->size()==3) {
		Value& value=r.process(*static_cast<Value *>(params->get(2)));
		r.wcontext->write(value, String::Untaint_lang::PASS_APPENDED);
	}
}

static void _untaint(Request& r, const String& name, Array *params) {
	const String& lang_name=r.process(*static_cast<Value *>(params->get(0))).as_string();
	String::Untaint_lang lang=static_cast<String::Untaint_lang>(
		untaint_lang_name_to_enum->get_int(lang_name));
	if(!lang)
		R_THROW(0, 0,
			&lang_name,
			"invalid untaint language");

	Temp_lang temp_lang(r, lang);
	Value *value=static_cast<Value *>(params->get(1));
	// forcing ^untaint[]{this param type}
	if(!value->get_junction())
		R_THROW(0, 0,
			&name,
			"body must be junction");

	value=&r.process(*value);
	r.wcontext->write(*value, String::Untaint_lang::PASS_APPENDED);
}
	

static void _process(Request& r, const String& name, Array *params) {
	// evaluate source to process
	const String& source=r.process(*static_cast<Value *>(params->get(0))).as_string();

	// calculate pseudo file name of processed chars
	// would be something like "/some/file(4) process"
	char place[MAX_STRING];
#ifndef NO_STRING_ORIGIN
	const Origin& origin=source.origin();
	snprintf(place, MAX_STRING, "%s(%d) %s", 
		origin.file, 1+origin.line,
		name.cstr());
#else
	strncpy(place, MAX_STRING, name.cstr());
#endif	

	VClass& self_class=*r.self->get_class();
	// temporarily zero @main so to maybe-replace it in processed code
	Temp_method temp_method(self_class, *main_method_name, 0);

	// process source code, append processed methods to 'self' class
	// maybe-define new @main
	r.use_buf(source.cstr(), place, &self_class);

	// maybe-execute @main[]
	if(Value *value=r.self->get_element(*main_method_name)) { // found some 'main' element
		if(Junction *junction=value->get_junction()) // it even has junction!
			if(const Method *method=junction->method) { // and junction is method-junction! call it
				// execute!	
				r.execute(*method->parser_code);
			}
	}
}
	

void initialize_root_class(Pool& pool, VClass& vclass) {
	// ^if(condition){code-when-true}
	// ^if(condition){code-when-true}{code-when-false}
	vclass.add_native_method("if", _if, 2, 3);

	// ^untaint[as-is|sql|js|html|html-typo]{code}
	vclass.add_native_method("untaint", _untaint, 2, 2);

	// ^process[code]
	vclass.add_native_method("process", _process, 1, 1);
}
