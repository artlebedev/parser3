/** @file
	Parser: @b file parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: file.C,v 1.28 2001/04/17 19:31:06 paf Exp $
*/

#include "pa_request.h"
#include "_file.h"
#include "pa_vfile.h"
#include "pa_table.h"
#include "pa_vint.h"
#include "pa_exec.h"

// consts

const int FIND_MONKEY_MAX_HOPS=10;

// global var

VStateless_class *file_class;

// methods

static void _save(Request& r, const String&, MethodParams *params) {
	Value& vfile_name=params->get_no_junction(0, "file name must not be code");

	// save
	static_cast<VFile *>(r.self)->save(r.absolute(vfile_name.as_string()));
}

static void _delete(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfile_name=params->get_no_junction(0, "file name must not be code");

	// unlink
	file_delete(pool, r.absolute(vfile_name.as_string()));
}

static void _find(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfile_name=params->get_no_junction(0, "file name must not be code");

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
		Value& not_found_code=params->get_junction(1, "not-found param must be code");
		r.write_pass_lang(r.process(not_found_code));
	}
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfile_name=params->get_no_junction(0, "file name must not be code");

	const String& lfile_name=vfile_name.as_string();

	void *data;  size_t size;
	file_read(pool, r.absolute(lfile_name), data, size, false/*binary*/);

	char *user_file_name=params->size()==1?lfile_name.cstr(String::UL_FILE_NAME)
		:params->get(1).as_string().cstr();
	
	static_cast<VFile *>(r.self)->set(true/*tainted*/, data, size, 
		user_file_name, &r.mime_type_of(user_file_name));
}

static void _stat(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfile_name=params->get_no_junction(0, "file name must not be code");

	const String& lfile_name=vfile_name.as_string();

	size_t size=file_size(r.absolute(lfile_name));
	
	static_cast<VFile *>(r.self)->set(true/*tainted*/, 0/*no bytes*/, size);
}

static void append_env_pair(const Hash::Key& key, Hash::Val *value, void *info) {
	Hash& hash=*static_cast<Hash *>(info);
	hash.put(key, &static_cast<Value *>(value)->as_string());
}
/**
	^exec[file-name]
	^exec[file-name;env hash]
	^exec[file-name;env hash;cmd;line;arg;s]
	@test header to $fields. waits for header '\' tricks
	@todo fix `` in perl - they produced flipping consoles and no output to perl
*/
static void _cgi(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& vfile_name=params->get_no_junction(0, "file name must not be code");

	const String& script_name=r.absolute(vfile_name.as_string());

	Hash env(pool);
	#define PASS(key) \
		String key(pool); \
		if(const char *value=SAPI::get_env(pool, #key)) { \
			key.APPEND_CONST(value); \
			env.put(String(pool, #key), &key); \
		}
	#define INFO(key, value) \
		String value(pool); \
		if(r.info.value) { \
			value.APPEND_CONST(r.info.value); \
			env.put(String(pool, key), &value); \
		}

	// const
	String gateway_interface(pool, "CGI/1.1");
	env.put(String(pool, "GATEWAY_INTERFACE"), &gateway_interface);
	// from Request.info
	INFO("DOCUMENT_ROOT", document_root);
	INFO("PATH_TRANSLATED", path_translated);
	INFO("SERVER_PROTOCOL", method);
	INFO("QUERY_STRING", query_string);
	INFO("REQUEST_URI", uri);
	INFO("CONTENT_TYPE", content_type);
	char content_length_cstr[MAX_NUMBER];  
	snprintf(content_length_cstr, MAX_NUMBER, "%u", r.info.content_length);
	String content_length(pool, content_length_cstr);
	env.put(String(pool, "CONTENT_LENGTH"), &content_length);
	INFO("HTTP_COOKIE", cookie);
	INFO("HTTP_USER_AGENT", user_agent);
	// passing some SAPI:get_env-s
	PASS(SERVER_NAME);
	PASS(SERVER_PORT);
	PASS(HTTP_REFERER);
	PASS(REMOTE_ADDR);
	PASS(REMOTE_HOST);
	PASS(REMOTE_USER);
	// SCRIPT_NAME
	env.put(String(pool, "SCRIPT_NAME"), &script_name);
#ifdef WIN32
	// WIN32 shell
	PASS(COMSPEC);
#endif

	if(params->size()>1) {
		Value& venv=params->get_no_junction(1, "env must not be code");
		if(Hash *user_env=venv.get_hash())
			user_env->for_each(append_env_pair, &env);
		else
			PTHROW(0, 0,
				&method_name,
				"env must be hash");
	}

	Array *argv=0;
	if(params->size()>2) {
		argv=new(pool) Array(pool, params->size()-2);
		for(int i=2; i<params->size(); i++)
			*argv+=&params->get(i).as_string();
	}

	const String in(pool, r.post_data, r.post_size);
	String out(pool);
	String& err=*new(pool) String(pool);
	int exit_code=pa_exec(script_name, &env, argv,
		in, out, err);

	VFile& self=*static_cast<VFile *>(r.self);
	// construct with 'out' body and header
	int delim_size;
	int pos=out.pos("\n\n", delim_size=2);
	if(pos<0)
		pos=out.pos("\r\n\r\n", delim_size=4);
	if(pos<0) {
		delim_size=0; // calm down, compiler
		PTHROW(0, 0,
			&method_name,
			"output does not contain CGI header; exit code=%d; size=%u; text: \"%s\"", 
				exit_code, (uint)out.size(), out.cstr());
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

	// ^stat[disk-name]
	vclass.add_native_method("stat", Method::CT_DYNAMIC, _stat, 1, 1);

	// ^exec[file-name]
	// ^exec[file-name;env hash]
	// ^exec[file-name;env hash;1cmd;2line;3ar;4g;5s]
	vclass.add_native_method("cgi", Method::CT_DYNAMIC, _cgi, 1, 2+5);
}
