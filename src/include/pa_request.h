/** @file
	Parser: request class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.h,v 1.63 2001/03/19 19:17:43 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_wcontext.h"
#include "pa_value.h"
#include "pa_stack.h"
#include "pa_vclass.h"
#include "pa_vobject.h"
#include "pa_venv.h"
#include "pa_vform.h"
#include "pa_vrequest.h"
#include "pa_vresponse.h"
#include "pa_vcookie.h"

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS  \
		const char *source, \
		VStateless_class *aclass, const String *name, \
		VStateless_class *base_class, \
		const char *file
#	define COMPILE(source, aclass, name, base_class, file)  \
		real_compile(source, aclass, name, base_class, file)
#else
#	define COMPILE_PARAMS  \
		const char *source, \
		VStateless_class *aclass, const String *name, \
		VStateless_class *base_class
#	define COMPILE(source, aclass, name, base_class, file)  \
		real_compile(source, aclass, name, base_class)
#endif

class Temp_lang;

/// Main workhorse.
class Request : public Pooled {
	friend Temp_lang;
public:

	/// some information from web server
	struct Info {
		const char *document_root;
		const char *path_translated;
		const char *method;
		const char *query_string;
		const char *uri;
		const char *content_type;
		size_t content_length;
		const char *cookie;
	};
	
	Request(Pool& apool,
		Info& ainfo,
		String::Untaint_lang adefault_lang ///< all tainted data default untainting lang
	);
	~Request() {}

	/// global classes
	Hash& classes() { return fclasses; }

	/// core request processing
	void core(Exception& system_exception,
		const char *sys_auto_path1,
		const char *sys_auto_path2);

	/// executes ops
	void execute(const Array& ops);

	/// compiles the file, maybe forcing it's class \a name and \a base_class.
	VStateless_class *use_file(
		const char *file, bool fail_on_read_problem=true,
		const String *name=0, 
		VStateless_class *base_class=0); // core.C
	/// compiles a \a source buffer
	VStateless_class *use_buf(
		const char *source, const char *file,
		VStateless_class *aclass=0, const String *name=0, 
		VStateless_class *base_class=0); // core.C
	/// processes any code-junction there may be inside of \a value
	Value& process(
		Value& value, 
		const String *name=0,
		bool intercept_string=true); // execute.C

	/// write(const) = clean
	void write(const String& astring) {
		wcontext->write(astring, String::Untaint_lang::NO);
	}
	/// appending, sure of clean string inside
	void write_no_lang(String& astring) {
		wcontext->write(astring, String::Untaint_lang::NO);
	}
	/// appending string, passing language built into string being written
	void write_pass_lang(String& astring) {
		wcontext->write(astring, String::Untaint_lang::PASS_APPENDED); 
	}
	/// appending possible string, assigning untaint language
	void write_assign_lang(Value& avalue) {
		wcontext->write(avalue, flang); 
	}
	/// appending possible string, passing language built into string being written
	void write_pass_lang(Value& avalue) {
		wcontext->write(avalue, String::Untaint_lang::PASS_APPENDED); 
	}
	/// appending sure value, that would be converted to clean string
	void write_no_lang(Value& avalue) {
		wcontext->write(avalue, String::Untaint_lang::NO);
	}
	/// appending sure value, not VString
	void write_expr_result(Value& avalue) {
		wcontext->write(avalue); 
	}

	/// handy is-value-a-junction ensurer
	void fail_if_junction_(bool is, Value& value, const String& method_name, char *msg);

	/// returns relative to \a path  path to \a file 
	char *relative(const char *path, const char *file);

	/// returns an absolute \a path to relative \a name
	char *absolute(const char *name);

public:
	
	/// info from web server
	Info& info;

	/// default base
	VClass ROOT;
	/// $env:fields here
	VEnv env;
	/// $form:elements here
	VForm form;
	/// $request:elements here
	VRequest request;
	/// $response:
	VResponse response;
	/// $cookie:
	VCookie cookie;

	/// contexts
	Value *self, *root, *rcontext;
	/// contexts
	WContext *wcontext;

private: // core data

	// classes
	Hash fclasses;

	// execution stack
	Stack stack;

private: // compile.C

	VStateless_class& real_compile(COMPILE_PARAMS);

private: // execute.C

	const String *execute_method(Value& aself, const Method& method, 
		bool return_cstr=true);
	const String *execute_method(Value& aself, const String& method_name, 
		bool return_cstr=true);

	Value *get_element();

private: // lang&raw 
	
	String::Untaint_lang flang;

private: // defaults

	const String::Untaint_lang fdefault_lang;
	Value *fdefault_content_type;

private: // lang manipulation

	String::Untaint_lang set_lang(String::Untaint_lang alang) {
		String::Untaint_lang result=flang;
		flang=alang;
		return result;
	}
	void restore_lang(String::Untaint_lang alang) {
		flang=alang;
	}

private:

	void output_result(const String& body_string);
};

///	Auto-object used for temporary changing Request::flang.
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
