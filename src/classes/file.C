/** @file
	Parser: @b file parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: file.C,v 1.21 2001/04/09 11:30:35 paf Exp $
*/

#include "pa_request.h"
#include "_file.h"
#include "pa_vfile.h"
#include "pa_table.h"
#include "pa_vint.h"

// consts

const int FIND_MONKEY_MAX_HOPS=10;

// global var

VStateless_class *file_class;

// methods

static void _save(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value& vfile_name=*static_cast<Value *>(params->get(0));
	// forcing
	// ^save[this body type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be code");

	// save
	static_cast<VFile *>(r.self)->save(r.absolute(vfile_name.as_string()));
}

static void _delete(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value& vfile_name=*static_cast<Value *>(params->get(0));
	// forcing
	// ^delete[this body type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be code");

	// unlink
	file_delete(pool, r.absolute(vfile_name.as_string()));
}

static void _find(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value& vfile_name=*static_cast<Value *>(params->get(0));
	// forcing
	// ^delete[this body type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be code");

	const String &lfile_name=vfile_name.as_string();

	// passed file name simply exists in current dir
	if(file_readable(r.absolute(lfile_name))) {
		r.write_no_lang(*new(pool) VString(lfile_name));
		return;
	}

	// scan .. dirs for result
	for(int i=0; i<FIND_MONKEY_MAX_HOPS; i++) {
		String test_name(pool);
		for(int j=0; j<i; j++)
			test_name.APPEND_CONST("../");
		test_name.append(lfile_name, String::UL_CLEAN);
		if(file_readable(r.absolute(test_name))) {
			r.write_no_lang(*new(pool) VString(*new(pool) String(test_name)));
			return;
		}
	}

	// not found
	if(params->size()==2) {
		// forcing ..{this body type}
		Value& not_found_code=*static_cast<Value *>(params->get(1));
		r.fail_if_junction_(false, not_found_code, 
			method_name, "not-found param must be code");
		r.write_pass_lang(r.process(not_found_code));
	}
}

static void _load(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value& vfile_name=*static_cast<Value *>(params->get(0));

	// forcing ^load[this body type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be code");

	const String& lfile_name=vfile_name.as_string();

	void *data;  size_t size;
	file_read(pool, r.absolute(lfile_name), data, size, false/*binary*/);

	char *user_file_name=params->size()==1?lfile_name.cstr(String::UL_FILE_NAME)
		:static_cast<Value *>(params->get(1))->as_string().cstr();
	
	static_cast<VFile *>(r.self)->set(true/*tainted*/, data, size, 
		user_file_name, &r.mime_type_of(user_file_name));
}

/// ^exec[file-name]
/// ^exec[file-name;env hash]
/// ^exec[file-name;env hash;cmd;line;arg;s]
/// @test header to $fields. waits for header '\' tricks
static void _cgi(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value& vfile_name=*static_cast<Value *>(params->get(0));
	// forcing [this param type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be code");

	Hash *env=0; 
	if(params->size()>1) {
		Value& venv=*static_cast<Value *>(params->get(1));
		// forcing [this param type]
		r.fail_if_junction_(true, venv, 
			method_name, "env must not be code");
		env=venv.get_hash();
		if(!env)
			PTHROW(0, 0,
				&method_name,
				"env must be hash");
	}

	Array *argv=0;
	if(params->size()>2) {
		argv=new(pool) Array(pool, params->size()-2);
		for(int i=2; i<params->size(); i++)
			*argv+=&static_cast<Value *>(params->get(i))->as_string();
	}

	const String in(pool, r.post_data, r.post_size);
	String out(pool);
	String err(pool);
	int exit_code=SAPI::execute(r.absolute(vfile_name.as_string()), env, argv,
		in, out, err);

	VFile& self=*static_cast<VFile *>(r.self);
	// construct with 'out' body and header
	int delim_size;
	int pos=out.pos("\n\n", delim_size=2);
	if(pos<0)
		pos=out.pos("\r\n\r\n", delim_size=4);
	if(pos<0) {
		delim_size=0; // calm, compiler
		PTHROW(0, 0,
			&method_name,
			"output does not contain CGI header");
	}

	const String& header=out.mid(0, pos);
	const String& body=out.mid(pos+delim_size, out.size());

	// body
	self.set(false/*not tainted*/, body.cstr(String::UL_AS_IS), body.size());

	// todo header to $fields. waits for header '\' tricks

	// $exit-code
	self.fields().put(
		*new(pool) String(pool, "exit-code"),
		new(pool) VInt(pool, exit_code));
	
	// $stderr
	if(err.size()) {
		self.fields().put(
			*new(pool) String(pool, "stderr"),
			new(pool) VString(err));

		SAPI::log(pool, "cgi: %s", err.cstr());
	}
}

// initialize

void initialize_file_class(Pool& pool, VStateless_class& vclass) {
	// ^save[file-name]
	vclass.add_native_method("save", Method::CT_DYNAMIC, _save, 1, 1);

	// ^delete[file-name]
	vclass.add_native_method("delete", Method::CT_STATIC, _delete, 1, 1);

	// ^find[file-name]
	// ^find[file-name]{when-not-found}
	vclass.add_native_method("find", Method::CT_STATIC, _find, 1, 2);

	// ^load[disk-name]
	// ^load[disk-name;user-name]
	vclass.add_native_method("load", Method::CT_DYNAMIC, _load, 1, 2);

	// ^exec[file-name]
	// ^exec[file-name;env hash]
	// ^exec[file-name;env hash;1cmd;2line;3ar;4g;5s]
	vclass.add_native_method("cgi", Method::CT_DYNAMIC, _cgi, 1, 2+5);
}
