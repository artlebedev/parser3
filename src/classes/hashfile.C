/** @file
	Parser: @b hashfile parser class.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/


static const char* IDENT="$Id: hashfile.C,v 1.26 2003/11/06 10:25:40 paf Exp $";

#include "classes.h"

#include "pa_request.h"
#include "pa_vmethod_frame.h"
#include "pa_vhashfile.h"
#include "pa_vhash.h"

// class

class MHashfile : public Methoded {
public: // VStateless_class
	Value *create_new_value() { return new VHashfile; }

public:
	MHashfile();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(hashfile, new MHashfile, 0);

// methods

static void _open(Request& r, MethodParams& params) {
	VHashfile& self=GET_SELF(r, VHashfile);
	
	self.open(r.absolute(params.as_string(0, "filename must be string")));
}

static void _hash(Request& r, MethodParams& params) {
	VHashfile& self=GET_SELF(r, VHashfile);
	
	// write out result
	VHash& result=*new VHash(*self.get_hash());
	r.write_no_lang(result);
}

static void _delete(Request& r, MethodParams& params) {
	VHashfile& self=GET_SELF(r, VHashfile);
	
	// key
	const String &key=params.as_string(0, "key must be string");
	// remove
	self.remove(key);
}

static void _clear(Request& r, MethodParams&) {
	VHashfile& self=GET_SELF(r, VHashfile);
	self.clear();
}

#if LATER
static void _foreach(Request& r, MethodParams& params) {
	VHashfile& self=GET_SELF(r, VHashfile);

	const String& key_var_name=params.as_string(0, "key-var name must be string");
	const String& value_var_name=params.as_string(1, "value-var name must be string");
	Value& body_code=params.as_junction(2, "body must be code");
	Value *delim_maybe_code=params.size()>3?&params.get(3):0;

	bool need_delim=false;
	Value& var_context=*body_code.get_junction()->wcontext;
	VString& vkey=*new(pool) VString(pool);
	VString& vvalue=*new(pool) VString(pool);

	DB_Cursor cursor(*self.get_table_ptr(&method_name), &method_name);
	while(true) {
		String *key;
		String *data;
		if(!cursor.get(pool, key, data, DB_NEXT))
			break;

		if(!key) 
			continue; // expired

		vkey.set_string(*key);
		vvalue.set_string(*data);
		var_context.put_element(key_var_name, &vkey);
		var_context.put_element(value_var_name, &vvalue);

		Value& processed_body=r.process(body_code);
		if(delim_maybe_code) { // delimiter set?
			const String *string=processed_body.get_string();
			if(need_delim && string && string->size()) // need delim & iteration produced string?
				r.write_pass_lang(r.process(*delim_maybe_code));
			need_delim=true;
		}
		r.write_pass_lang(processed_body);
	}
}

#endif
// constructor

MHashfile::MHashfile(): Methoded("hashfile") {
	// ^hashfile::open[db_home;filename]
	add_native_method("open", Method::CT_DYNAMIC, _open, 1, 1);
	// ^hash[]
	add_native_method("hash", Method::CT_DYNAMIC, _hash, 0, 0);
	// ^hashfile.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 1, 1);
	// ^hashfile.clear[]
	add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 0);
	// ^hashfile.foreach[key;value]{code}[delim]
//	add_native_method("foreach", Method::CT_DYNAMIC, _foreach, 2+1, 2+1+1);
}
