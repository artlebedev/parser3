/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: file.C,v 1.2 2001/03/19 20:46:35 paf Exp $
*/

#include "pa_request.h"
#include "_file.h"
#include "pa_vfile.h"

// global var

VStateless_class *file_base_class;

// methods

static void _save(Request& r, const String& method_name, Array *params) {
	//\Pool& pool=r.pool();
	Value *vfile_name=static_cast<Value *>(params->get(0));
	// forcing
	// ^save{this body type}
	r.fail_if_junction_(false, *vfile_name, method_name, "file name must be junction");

	{
		Temp_lang temp_lang(r, String::Untaint_lang::FILE);
		static_cast<VFile *>(r.self)->save(
			r.absolute(r.process(*vfile_name).as_string().cstr()));
	}
}

// initialize

void initialize_file_base_class(Pool& pool, VStateless_class& vclass) {
	// ^save[file-name]
	vclass.add_native_method("save", _save, 1, 1);
}
