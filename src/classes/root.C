/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: root.C,v 1.22 2001/03/12 10:21:23 paf Exp $
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
		r.write_pass_lang(value);
	} else if(params->size()==3) {
		Value& value=r.process(*static_cast<Value *>(params->get(2)));
		r.write_pass_lang(value);
	}
}

static void _untaint(Request& r, const String& method_name, Array *params) {
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
			&method_name,
			"body must be junction");

	value=&r.process(*value);
	r.write_pass_lang(*value);
}
	

static void _process(Request& r, const String& method_name, Array *params) {
	// evaluate source to process
	const String& source=r.process(*static_cast<Value *>(params->get(0))).as_string();

	// calculate pseudo file name of processed chars
	// would be something like "/some/file(4) process"
	char place[MAX_STRING];
#ifndef NO_STRING_ORIGIN
	const Origin& origin=source.origin();
	snprintf(place, MAX_STRING, "%s(%d) %s", 
		origin.file, 1+origin.line,
		method_name.cstr());
#else
	strncpy(place, MAX_STRING, method_name.cstr());
#endif	

	VClass& self_class=*r.self->get_class();
	{
		// temporary zero @main so to maybe-replace it in processed code
		Temp_method temp_method(self_class, *main_method_name, 0);
		
		// process source code, append processed methods to 'self' class
		// maybe-define new @main
		r.use_buf(source.cstr(), place, &self_class);
		
		// maybe-execute @main[]
		if(const Method *method=self_class.get_method(*main_method_name)) {
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
