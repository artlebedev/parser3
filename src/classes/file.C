/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: file.C,v 1.4 2001/03/20 06:45:16 paf Exp $
*/

#include "pa_request.h"
#include "_file.h"
#include "pa_vfile.h"

// global var

VStateless_class *file_class;

// methods

static void _save(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value *vfile_name=static_cast<Value *>(params->get(0));
	// forcing
	// ^save[this body type]
	r.fail_if_junction_(true, *vfile_name, 
		method_name, "file name must not be junction");

	// forcing untaint language
	String lfile_name(pool);
	lfile_name.append(vfile_name->as_string(),
		String::Untaint_lang::FILE_NAME, true);
		
	static_cast<VFile *>(r.self)->save(r.absolute(lfile_name.cstr()));
}

// initialize

void initialize_file_class(Pool& pool, VStateless_class& vclass) {
	// ^save[file-name]
	vclass.add_native_method("save", _save, 1, 1);
}
