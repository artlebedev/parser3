/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: file.C,v 1.10 2001/03/28 13:21:27 paf Exp $
*/

#include "pa_request.h"
#include "_file.h"
#include "pa_vfile.h"

// consts

const int FIND_MONKEY_MAX_HOPS=10;

// global var

VStateless_class *file_class;

// methods

/// @test mkdirs
static void _save(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value& vfile_name=*static_cast<Value *>(params->get(0));
	// forcing
	// ^save[this body type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be junction");

	// forcing untaint language
	String lfile_name(pool);
	lfile_name.append(vfile_name.as_string(),
		String::UL_FILE_NAME, true);

	// save
	static_cast<VFile *>(r.self)->save(r.absolute(lfile_name));
}

static void _delete(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value& vfile_name=*static_cast<Value *>(params->get(0));
	// forcing
	// ^delete[this body type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be junction");

	// forcing untaint language
	String lfile_name(pool);
	lfile_name.append(vfile_name.as_string(),
		String::UL_FILE_NAME, true);
		
	// unlink
	file_delete(pool, r.absolute(lfile_name));
}

static void _find(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value& vfile_name=*static_cast<Value *>(params->get(0));
	// forcing
	// ^delete[this body type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be junction");

	// forcing untaint language
	String lfile_name(pool);
	lfile_name.append(vfile_name.as_string(),
		String::UL_FILE_NAME, true);

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
		test_name.append(lfile_name, String::UL_NO);
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
			method_name, "not-found param must be junction");
		r.write_pass_lang(r.process(not_found_code));
	}
}

static void _load(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value& vfile_name=*static_cast<Value *>(params->get(0));

	// forcing ^load[this body type]
	r.fail_if_junction_(true, vfile_name, 
		method_name, "file name must not be junction");

	// forcing untaint language
	String lfile_name(pool);
	lfile_name.append(vfile_name.as_string(),
		String::UL_FILE_NAME, true);

	void *data;
	size_t size;
	file_read(pool, r.absolute(lfile_name), data, size, false/*binary*/);

	char *user_file_name=params->size()==1?lfile_name.cstr()
		:static_cast<Value *>(params->get(1))->as_string().cstr();
	
	static_cast<VFile *>(r.self)->set(data, size, user_file_name);
}

// initialize

void initialize_file_class(Pool& pool, VStateless_class& vclass) {
	// ^save[file-name]
	vclass.add_native_method("save", _save, 1, 1);

	// ^delete[file-name]
	vclass.add_native_method("delete", _delete, 1, 1);

	// ^find[file-name]
	// ^find[file-name]{when-not-found}
	vclass.add_native_method("find", _find, 1, 2);

	// ^load[disk-name]
	// ^load[disk-name;user-name]
	// TODO:^load[disk-name;user-name;content-type]
	vclass.add_native_method("load", _load, 1, 2);
}
