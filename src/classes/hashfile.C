/** @file
	Parser: @b hashfile parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"

#include "pa_request.h"
#include "pa_vmethod_frame.h"
#include "pa_vhashfile.h"
#include "pa_vhash.h"

volatile const char * IDENT_HASHFILE_C="$Id: hashfile.C,v 1.64 2020/12/15 17:10:28 moko Exp $";

// class

class MHashfile : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& apool) { return new VHashfile(apool); }
public:
	MHashfile();
};

// global variable

DECLARE_CLASS_VAR(hashfile, new MHashfile);

// defines for statics

#define OPEN_DATA_NAME "HASHFILE-OPEN-DATA"

// methods

typedef HashString<bool> HashStringBool;

static void _open(Request& r, MethodParams& params) {
	HashStringBool* file_list=static_cast<HashStringBool*>(r.classes_conf.get(OPEN_DATA_NAME));
	if(!file_list) {
		file_list=new HashStringBool();
		r.classes_conf.put(OPEN_DATA_NAME, file_list);
	}

	const String& file_spec=r.full_disk_path(params.as_string(0, FILE_NAME_MUST_BE_STRING));
	if(file_list->get(file_spec))
		throw Exception(PARSER_RUNTIME,
			0,
			"this hashfile is already opened, use existing variable");
	file_list->put(file_spec, true);

	VHashfile& self=GET_SELF(r, VHashfile);
	self.open(file_spec);
}

static void _hash(Request& r, MethodParams&) {
	VHashfile& self=GET_SELF(r, VHashfile);
	
	// write out result
	VHash& result=*new VHash(*self.get_hash());
	r.write(result);
}

static void _delete(Request& r, MethodParams& params) {
	VHashfile& self=GET_SELF(r, VHashfile);
	
	if(!params.count()) {
		// ^hashfile.delete[] asked to delete hashfile itself
		self.delete_files();
		return;
	}
	// key
	const String &key=params.as_string(0, "key must be string");
	// remove
	self.remove(key);
}

static void _clear(Request& r, MethodParams&) {
	VHashfile& self=GET_SELF(r, VHashfile);
	self.delete_files();
}

#ifndef DOXYGEN
struct Foreach_info {
	Request* r;
	const String* key_var_name;
	const String* value_var_name;
	Value* body_code;
	Value* delim_maybe_code;

	Value* var_context;
	bool need_delim;
};
#endif
static bool one_foreach_cycle(
				const String::Body key,
				const String& value,
				void* ainfo) {
	Foreach_info& info=*static_cast<Foreach_info*>(ainfo);
	if(info.key_var_name){
		VString* vkey=new VString(*new String(key, String::L_TAINTED));
		info.r->put_element(*info.var_context, *info.key_var_name, vkey);
	}
	if(info.value_var_name){
		VString* vvalue=new VString(value);
		info.r->put_element(*info.var_context, *info.value_var_name, vvalue);
	}

	Value& sv_processed=info.r->process(*info.body_code);
	TempSkip4Delimiter skip(*info.r);

	const String* s_processed=sv_processed.get_string();
	if(info.delim_maybe_code && s_processed && !s_processed->is_empty()) { // delimiter set and we have body
		if(info.need_delim) // need delim & iteration produced string?
			info.r->write(info.r->process(*info.delim_maybe_code));
		else
			info.need_delim=true;
	}

	info.r->write(sv_processed);

	return skip.check_break();
}
static void _foreach(Request& r, MethodParams& params) {
	InCycle temp(r);

	const String& key_var_name=params.as_string(0, "key-var name must be string");
	const String& value_var_name=params.as_string(1, "value-var name must be string");

	Foreach_info info={
		&r,
		key_var_name.is_empty()? 0 : &key_var_name,
		value_var_name.is_empty()? 0 : &value_var_name,
		&params.as_junction(2, "body must be code"),
		/*delimiter*/params.count()>3 ? &params[3] : 0,
		/*var_context*/r.get_method_frame()->caller(),
		false
	};

	VHashfile& self=GET_SELF(r, VHashfile);
	self.for_each(one_foreach_cycle, &info);
}

static bool one_cleanup_cycle(const String::Body, const String&, void*) {
	return false;
}
static void _cleanup(Request& r, MethodParams&) {
	VHashfile& self=GET_SELF(r, VHashfile);

	self.for_each(one_cleanup_cycle, 0);
}

static void _release(Request& r, MethodParams&) {
	VHashfile& self=GET_SELF(r, VHashfile);
	self.close();
}

// constructor

MHashfile::MHashfile(): Methoded("hashfile") {
	// ^hashfile::open[filename]
	add_native_method("open", Method::CT_DYNAMIC, _open, 1, 1);
	// ^hashfile.hash[]
	add_native_method("hash", Method::CT_DYNAMIC, _hash, 0, 0);
	// ^hashfile.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 0, 1);
	// ^hashfile.clear[] -- for backward compatibility. use .delete[] instead.
	add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 0);
	// ^hashfile.release[]
	add_native_method("release", Method::CT_DYNAMIC, _release, 0, 0);
	// ^hashfile.cleanup[]
	add_native_method("cleanup", Method::CT_DYNAMIC, _cleanup, 0, 0);
	add_native_method("defecate", Method::CT_DYNAMIC, _cleanup, 0, 0);
	// ^hashfile.foreach[key;value]{code}[delim]
	add_native_method("foreach", Method::CT_DYNAMIC, _foreach, 2+1, 2+1+1);
}
