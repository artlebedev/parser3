/** @file
	Parser: @b hashfile parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: hashfile.C,v 1.12 2001/10/27 10:14:45 paf Exp $
*/

#include "pa_config_includes.h"
#include "classes.h"
#ifdef HAVE_LIBDB

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
		params->as_string(0, "DB_HOME must be string"), 
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
	double expires=params->as_double(1, "expires must be number", r);
	Value& body_code=params->as_junction(2, "body must be code");

	// transaction
	DB_Transaction transaction(pool, *self.get_table_ptr(&method_name), self.current_transaction);

	// execute body
	try {
		if(expires) { // 'expires' specified? try cached copy...
			if(String *cached_body=transaction.get(key)) { // have cached copy?
				r.write_assign_lang(*cached_body);
				// happy with it
				return;
			}
		} else // 'expires'=0, forget cached copy
			transaction.remove(key);

		// save
		Autosave_marked_to_cancel_cache saved(self);

		// process
		Value& processed_body=r.process(body_code);
		r.write_assign_lang(processed_body);
		
		// put it to cache if 'expires' specified & never called ^delete[]
		if(expires && !self.marked_to_cancel_cache())
			transaction.put(key, processed_body.as_string(), time(0)+(time_t)expires);
	} catch(...) { // process/commit problem
		transaction.mark_to_rollback();
		
		/*re*/throw; 
	}
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
}

// global variable

Methoded *hashfile_base_class;

#endif

// creator

Methoded *MHashfile_create(Pool& pool) {
	return 
#ifdef HAVE_LIBDB
		hashfile_base_class=new(pool) MHashfile(pool)
#else
		0
#endif
	;
}
