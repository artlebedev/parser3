/** @file
	Parser: request class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.h,v 1.97 2001/09/24 14:34:25 parser Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_wcontext.h"
#include "pa_value.h"
#include "pa_stack.h"
#include "pa_vclass.h"
#include "pa_vobject.h"
#include "pa_venv.h"
#include "pa_vform.h"
#include "pa_vmath.h"
#include "pa_vrequest.h"
#include "pa_vresponse.h"
#include "pa_vcookie.h"
#include "pa_sql_driver_manager.h"

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
class Methoded;

/// Main workhorse.
class Request : public Pooled {
	friend Temp_lang;
public:

	/// some information from web server
	class Info {
	public:
		const char *document_root;
		const char *path_translated;
		const char *method;
		const char *query_string;
		const char *uri;
		const char *content_type;
		size_t content_length;
		const char *cookie;
		const char *user_agent;
	};
	
	Request(Pool& apool,
		Info& ainfo,
		String::Untaint_lang adefault_lang ///< all tainted data default untainting lang
	);
	~Request() {}

	/// global classes
	Hash& classes() { return fclasses; }

	/**
		core request processing

		BEWARE: may throw exception to you: catch it!
	*/
	void core(
		const char *root_config_filespec, ///< system config filespec
		bool root_config_fail_on_read_problem, ///< fail if system config file not found
		const char *site_config_filespec, ///< site config filespec
		bool site_config_fail_on_read_problem, ///< fail if site config file not found
		bool header_only);

	/// executes ops
	void execute(const Array& ops); // execute.C

	/// compiles the file, maybe forcing it's class @a name and @a base_class.
	VStateless_class *use_file(
		const String& file_name, 
		bool ignore_class_path=false, bool fail_on_read_problem=true,
		const String *name=0, 
		VStateless_class *base_class=0); // core.C
	/// compiles a @a source buffer
	VStateless_class *use_buf(
		const char *source, const char *file,
		VStateless_class *aclass=0, const String *name=0, 
		VStateless_class *base_class=0); // core.C
	/// processes any code-junction there may be inside of @a value
	Value& process(
		Value& value, 
		const String *name=0,
		bool intercept_string=true); // execute.C

	/// appending, sure of clean string inside
	void write_no_lang(const String& astring) {
		wcontext->write(astring, String::UL_CLEAN);
	}
	/// appending string, passing language built into string being written
	void write_pass_lang(const String& astring) {
		wcontext->write(astring, String::UL_PASS_APPENDED); 
	}
	/// appending possible string, assigning untaint language
	void write_assign_lang(Value& avalue) {
		wcontext->write(avalue, flang); 
	}
	/// appending possible string, passing language built into string being written
	void write_pass_lang(Value& avalue) {
		wcontext->write(avalue, String::UL_PASS_APPENDED); 
	}
	/// appending sure value, that would be converted to clean string
	void write_no_lang(Value& avalue) {
		wcontext->write(avalue, String::UL_CLEAN);
	}
	/// appending sure value, not VString
	void write_expr_result(Value& avalue) {
		wcontext->write(avalue); 
	}

	/// returns relative to @a path  path to @a file 
	const String& relative(const char *apath, const String& relative_name);

	/// returns an absolute @a path to relative @a name
	const String& absolute(const String& relative_name);

	/// returns the mime type of 'user_file_name_cstr'
	const String& mime_type_of(const char *user_file_name_cstr);

public:
	
	/// info from web server
	Info& info;
	/// user's post data
	char *post_data;  size_t post_size;

	/// operators are methods of this class
	Methoded& OP;
	/// $env:fields
	VEnv env;
	/// $form:elements
	VForm form;
	/// $math:constants
	VMath math;
	/// $request:elements
	VRequest request;
	/// $response:elements
	VResponse response;
	/// $cookie:elements
	VCookie cookie;

	/// contexts
	Value *self, *root, *rcontext;
	/// contexts
	WContext *wcontext;

	/// 'MAIN' class conglomerat
	VStateless_class *main_class;

	/// connection
	SQL_Connection *connection;
	/// PCRE character tables
	unsigned char *pcre_tables;

	/// classes configured data
	Hash classes_conf;


private: // core data

	/// classes
	Hash fclasses;

	/// already used files to avoid cyclic uses
	Hash used_files;

	/// execution stack
	Stack stack;

	/**	endless execute(execute(... preventing counter 
		@see ANTI_ENDLESS_EXECUTE_RECOURSION
	*/
	uint anti_endless_execute_recoursion;

private: // compile.C

	VStateless_class& real_compile(COMPILE_PARAMS);

private: // execute.C

	const String *execute_method(Value& aself, 
		const Method& method, bool return_cstr=true);
	const String *execute_virtual_method(Value& aself, 
		const String& method_name, bool return_cstr=true);
	const String *execute_nonvirtual_method(VStateless_class& aclass, 
		const String& method_name, bool return_cstr=true);

	Value *get_element();

private: // lang&raw 
	
	String::Untaint_lang flang;

private: // defaults

	const String::Untaint_lang fdefault_lang;
	Value *default_content_type;

private: // mime types

	/// $MAIN:MIME-TYPES
	Table *mime_types;

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

	void output_result(const VFile& body_file, bool header_only);
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

/**
	@b method parameters passed in this array.
	contains handy typecast ad junction/not junction ensurers

*/
class MethodParams : public Array {
public:
	MethodParams(Pool& pool, const String& amethod_name) : Array(pool),
		fmethod_name(amethod_name) {
	}

	/// handy typecast. I long for templates
	Value& get(int index) { 
		return *static_cast<Value *>(Array::get(index)); 
	}
	/// handy is-value-a-junction ensurer
	Value& as_junction(int index, const char *msg) { 
		return get_as(index, true, msg); 
	}
	/// handy value-is-not-a-junction ensurer
	Value& as_no_junction(int index, const char *msg) { 
		return get_as(index, false, msg); 
	}
	/// handy expression auto-processing to double
	double as_double(int index, Request& r) { 
		return get_processed(index, r).as_double(); 
	}
	/// handy expression auto-processing to int
	int as_int(int index, Request& r) { 
		return get_processed(index, r).as_int(); 
	}
	/// handy string ensurer
	const String& as_string(int index, const char *msg) { 
		return as_no_junction(index, msg).as_string(); 
	}

private:

	/// handy value-is/not-a-junction ensurer
	Value& get_as(int index, bool as_junction, const char *msg) { 
		Value& result=get(index);
		if((result.get_junction()!=0) ^ as_junction)
			THROW(0, 0,
				&fmethod_name,
				"%s (parameter #%d)", msg, 1+index);
		return result;
	}

	Value& get_processed(int index, Request& r) {
		return r.process(get(index),
			0/*no name*/,
			false/*don't intercept string*/);
	}

private:

	const String& fmethod_name;

};

#endif
