/** @file
	Parser: @b exec parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: exec.C,v 1.2 2001/04/09 11:04:08 paf Exp $
*/

#include "pa_config_includes.h"
#include "pa_sapi.h"
#include "_exec.h"
#include "pa_request.h"

VStateless_class *exec_class;

/// ^exec[file-name]
/// ^exec[file-name;env hash]
/// ^exec[file-name;env hash;cmd;line;arg;s]
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
	SAPI::execute(r.absolute(vfile_name.as_string()), env, argv,
		in, out, err);
	r.write_pass_lang(out);
	if(err.size())
		SAPI::log(pool, "cgi:%s", err.cstr());
}

// initialize

void initialize_exec_class(Pool& pool, VStateless_class& vclass) {
	// ^exec[file-name]
	// ^exec[file-name;env hash]
	// ^exec[file-name;env hash;1cmd;2line;3ar;4g;5s]
	vclass.add_native_method("cgi", Method::CT_STATIC, _cgi, 1, 2+5);
}
