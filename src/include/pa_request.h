/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.h,v 1.43 2001/03/12 17:16:49 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_hash.h"
#include "pa_wcontext.h"
#include "pa_value.h"
#include "pa_stack.h"
#include "pa_vclass.h"

#define MAIN_METHOD_NAME "main"
#define AUTO_METHOD_NAME "auto"
#define MAIN_CLASS_NAME "MAIN"

#define ROOT_CLASS_NAME "ROOT"
#define ENV_CLASS_NAME "ENV"
#define TABLE_CLASS_NAME "table"

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS  \
		const char *source, \
		VClass *aclass, const String *name, \
		VClass *base_class, \
		const char *file
#	define COMPILE(source, aclass, name, base_class, file)  \
		real_compile(source, aclass, name, base_class, file)
#else
#	define COMPILE_PARAMS  \
		const char *source, \
		VClass *aclass, const String *name, \
		VClass *base_class
#	define COMPILE(source, aclass, name, base_class, file)  \
		real_compile(source, aclass, name, base_class)
#endif

class Temp_lang;

class Request : public Pooled {
	friend Temp_lang;
public:
	
	Request(Pool& apool);
	~Request() {}

	// global classes
	Hash& classes() { return fclasses; }
	Array& classes_array() { return fclasses_array; }

	// core request processing
	void core();

	void execute(const Array& ops);

	VClass *use_file(
		const char *file, bool fail_on_read_problem=true,
		VClass *aclass=0, const String *name=0, 
		VClass *base_class=0); // core.C
	VClass *use_buf(
		const char *source, const char *file,
		VClass *aclass=0, const String *name=0, 
		VClass *base_class=0); // core.C
	Value& process(
		Value& value, 
		const String *name=0,
		bool intercept_string=true); // execute.C

	void write(const String& astring) {
		wcontext->write(astring, String::Untaint_lang::NO);  // write(const) = clean
	}

	void write_assign_lang(Value& avalue) {
		// appending possible string, assigning untaint language
		wcontext->write(avalue, flang); 
	}
	void write_pass_lang(Value& avalue) {
		// appending possible string, passing language built into string being written
		wcontext->write(avalue, String::Untaint_lang::PASS_APPENDED); 
	}

	void fail_if_junction_(bool is, Value& value, const String& method_name, char *msg);

	char *relative(const char *path, const char *file);
	char *absolute(const char *name);

public:
	
	// default base
	VClass root_class;
	// $ENV:fields here
	VClass env_class;

	// contexts
	Value *self, *root, *rcontext;
	WContext *wcontext;

private: // core data

	// classes
	Hash fclasses;
	Array fclasses_array;

	// execution stack
	Stack stack;

private: // core.C

	char *execute_MAIN();

private: // compile.C

	VClass& real_compile(COMPILE_PARAMS);

private: // execute.C

	char *execute_static_method(VClass& vclass, String& method_name, bool return_cstr);

	Value *get_element();

private: // lang&raw 
	
	String::Untaint_lang flang;

private: // lang manipulation

	String::Untaint_lang set_lang(String::Untaint_lang alang) {
		String::Untaint_lang result=flang;
		flang=alang;
		return result;
	}
	void restore_lang(String::Untaint_lang alang) {
		flang=alang;
	}

private: // web

	const char *document_root;
	const char *page_filespec;

};

class Temp_lang {
	Request& frequest;
	String::Untaint_lang saved_lang;
public:
	Temp_lang(Request& arequest, String::Untaint_lang alang) : 
		frequest(arequest),
		saved_lang(arequest.set_lang(alang)) {
	}
	~Temp_lang() { 
		frequest.restore_lang(saved_lang); 
	}
};

#endif
