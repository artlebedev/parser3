/** @file
	Parser: @b file parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: file.C,v 1.19 2001/04/05 18:22:56 paf Exp $
*/

#include "pa_request.h"
#include "_file.h"
#include "pa_vfile.h"
#include "pa_table.h"

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
	
	const String *mime_type=0;
	if(params->size()==3)
		mime_type=&static_cast<Value *>(params->get(2))->as_string();
	else {
		if(r.mime_types) {
			if(char *cext=strrchr(user_file_name, '.')) {
				cext++;
				String sext(pool, cext);
				if(r.mime_types->locate(0, sext))
					if(!(mime_type=r.mime_types->item(1)))
						PTHROW(0, 0,
						r.mime_types->origin_string(),
						"MIME-TYPE table column elements must not be empty");
			}
		}
	}

	if(!mime_type)
		mime_type=new(pool) String(pool, "application/octet-stream");

	static_cast<VFile *>(r.self)->set(data, size, user_file_name, mime_type);
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
	// ^load[disk-name;user-name;mime-type]
	vclass.add_native_method("load", Method::CT_DYNAMIC, _load, 1, 3);
}
