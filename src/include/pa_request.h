/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_request.h,v 1.32 2001/03/10 16:34:35 paf Exp $
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
#define RUN_CLASS_NAME "RUN"
#define AUTO_CLASS_NAME "AUTO"

#define ROOT_CLASS_NAME "ROOT"
#define ENV_CLASS_NAME "ENV"

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS char *source, String *name, VClass *base_class, char *file
#	define COMPILE(source, name, base_class, file) real_compile(source, name, base_class, file)
#else
#	define COMPILE_PARAMS char *source, String *name, VClass *base_class
#	define COMPILE(source, name, base_class, file) real_compile(source, name, base_class)
#endif

class Local_request_exception;

class Request : public Pooled {
public:
	
	Request(Pool& apool);
	~Request() {}

	// global classes
	Hash& classes() { return fclasses; }
	Array& classes_array() { return fclasses_array; }

	// core request processing
	void core();

	VClass *use(char *file, 
		String *name=0, 
		VClass *base_class=0, 
		bool fail_on_read_problem=true); // core.C
	Value& autocalc(Value& value, const String *name=0, 
		bool make_string=true); // execute.C

	void write(const String& astring) {
		wcontext->write(astring, String::Untaint_lang::NO);  // write(const) = clean
	}

	void write(Value& avalue) {
		// appending possible string, assigning untaint language
		wcontext->write(avalue, lang); 
	}

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
	void execute(const Array& ops);

	Value *get_element();

private: // lang&raw 
	
	String::Untaint_lang lang;

private: // lang&raw manipulation

	// TODO
};

#endif
