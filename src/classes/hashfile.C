/** @file
	Parser: @b hashfile parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: hashfile.C,v 1.19 2001/12/24 09:05:34 paf Exp $
*/

#include "pa_config_includes.h"
#include "classes.h"
#ifdef DB2

#include "pa_request.h"
#include "pa_vhashfile.h"
#include "pa_vhash.h"

// defines

#define HASH_CLASS_NAME "hashfile"

// class

class MHashfile : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VHashfile(pool); }

public:
	MHashfile(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _open(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	self.assign(
		r.absolute(params->as_string(0, "DB_HOME must be string")), 
		params->as_string(1, "filename must be string")
	);
}

static void _transaction(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	// body code
	Value& body_code=params->as_junction(0, "body must be code");

	// table
	DB_Table_ptr table_ptr=self.get_table_ptr(&method_name);

	// transaction
	DB_Transaction transaction(pool, *table_ptr, self.current_transaction);

	// execute body
	try {
		r.write_assign_lang(r.process(body_code));
	} catch(...) { // process/commit problem
		transaction.mark_to_rollback();
		
		/*re*/throw; 
	}
}

static void _hash(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	// write out result
	VHash& result=*new(pool) VHash(pool, *self.get_hash(&method_name));
	result.set_name(method_name);
	r.write_no_lang(result);
}

static void _cache(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	// key, expires, body code
	const String &key=params->as_string(0, "key must be string");
	time_t lifespan=(time_t)params->as_double(1, "lifespan must be number", r);
	Value& body_code=params->as_junction(2, "body must be code");

	DB_Table_ptr table_ptr=self.get_table_ptr(&method_name);

		if(lifespan) { // 'lifespan' specified? try cached copy...
		if(String *cached_body=table_ptr->get(self.current_transaction, pool, key, lifespan)) { // have cached copy?
			r.write_assign_lang(*cached_body);
			// happy with it
			return;
		}
	} else // 'lifespan'=0, forget cached copy
		table_ptr->remove(self.current_transaction, key);

	// save
	Autosave_marked_to_cancel_cache saved(self);

	// process
	Value& processed_body=r.process(body_code);
	r.write_assign_lang(processed_body);
	
	// put it to cache if 'lifespan' specified & never called ^delete[]
	if(lifespan && !self.marked_to_cancel_cache())
		table_ptr->put(self.current_transaction, key, processed_body.as_string(), lifespan);
}

static void _delete(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	if(params->size()==0)
		self.mark_to_cancel_cache();
	else {
		// key
		const String &key=params->as_string(0, "key must be string");
		// remove
		self.get_table_ptr(&method_name)->remove(self.current_transaction, key);
	}
}

static void _clear(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	DB_Cursor cursor(
		*self.get_table_ptr(&method_name), 
		self.current_transaction, &method_name);

	while(true) {
		if(!cursor.move(DB_NEXT))
			break;

		cursor.remove(0/*flags*/);
	}
}

static void _foreach(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);

	const String& key_var_name=params->as_string(0, "key-var name must be string");
	const String& value_var_name=params->as_string(1, "value-var name must be string");
	Value& body_code=params->as_junction(2, "body must be code");
	Value *delim_maybe_code=params->size()>3?&params->get(3):0;

	bool need_delim=false;
	Value& var_context=*body_code.get_junction()->wcontext;
	VString& vkey=*new(pool) VString(pool);
	VString& vvalue=*new(pool) VString(pool);

	DB_Cursor cursor(
		*self.get_table_ptr(&method_name), 
		self.current_transaction, &method_name);
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

// constructor

MHashfile::MHashfile(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), HASH_CLASS_NAME));

	// ^hashfile::open[db_home;filename]
	add_native_method("open", Method::CT_DYNAMIC, _open, 2, 2);
	// ^transaction{code}
	add_native_method("transaction", Method::CT_DYNAMIC, _transaction, 1, 1);
	// ^hash[]
	add_native_method("hash", Method::CT_DYNAMIC, _hash, 0, 0);
	// ^cache[key](seconds){code}
	add_native_method("cache", Method::CT_DYNAMIC, _cache, 3, 3);
	// ^hashfile.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 0, 1);
	// ^hashfile.clear[]
	add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 0);
	// ^hashfile.foreach[key;value]{code}[delim]
	add_native_method("foreach", Method::CT_DYNAMIC, _foreach, 2+1, 2+1+1);
}

// global variable

Methoded *hashfile_base_class;

#endif

// creator

Methoded *MHashfile_create(Pool& pool) {
	return 
#ifdef DB2
		hashfile_base_class=new(pool) MHashfile(pool)
#else
		0
#endif
	;
}
