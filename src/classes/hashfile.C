/** @file
	Parser: @b hashfile parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: hashfile.C,v 1.7 2001/10/24 10:49:47 parser Exp $
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
	
	// file_spec
	const String &file_spec=params->as_string(0, "filename must be string");

	self.set_connection(
		DB_manager->get_connection(
			r.absolute(file_spec), 
			method_name)
	);
}


static void _transaction(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	// body code
	Value& body_code=params->as_junction(0, "body must be code");

	// connection
	DB_Connection& connection=self.get_connection(&method_name);

	// transaction
	Auto_transaction transaction(connection);

	// execute body
	try {
		r.write_assign_lang(r.process(body_code));
	} catch(...) { // process/commit problem
		transaction.mark_to_rollback();
		
		/*re*/throw; 
	}
}

static void remove(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	// key
	const String &key=params->as_string(0, "key must be string");

	// connection
	DB_Connection& connection=self.get_connection(&method_name);

	connection.remove(key);
}

static void _clear(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	
	// file_spec
	const String &file_spec=params->as_string(0, "filename must be string");

	DB_manager->clear_dbfile(r.absolute(file_spec));
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

	// connection
	DB_Connection& connection=self.get_connection(&method_name);

	// transaction
	Auto_transaction transaction(connection);

	// execute body
	try {
		if(expires) { // 'expires' specified? try cached copy...
			if(String *cached_body=connection.get(key)) { // have cached copy?
				r.write_assign_lang(*cached_body);
				// happy with it
				return;
			}
		} else // 'expires'=0, forget cached copy
			connection.remove(key);

		// process
		Value& processed_body=r.process(body_code);
		r.write_assign_lang(processed_body);
		
		// put it to cache if 'expires' specified
		if(expires)
			connection.put(key, processed_body.as_string(), time(0)+(time_t)expires);
	} catch(...) { // process/commit problem
		transaction.mark_to_rollback();
		
		/*re*/throw; 
	}
}

// constructor

MHashfile::MHashfile(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), HASH_CLASS_NAME));

	// ^hashfile::open[filename]
	add_native_method("open", Method::CT_DYNAMIC, _open, 1, 1);
	// ^transaction{code}
	add_native_method("transaction", Method::CT_DYNAMIC, _transaction, 1, 1);
	// ^hashfile.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, remove, 1, 1);
	// ^hashfile:clear[filename]
	add_native_method("clear", Method::CT_STATIC, _clear, 1, 1);
	// ^hash[]
	add_native_method("hash", Method::CT_DYNAMIC, _hash, 0, 0);
	// ^cache[key](seconds){code}
	add_native_method("cache", Method::CT_DYNAMIC, _cache, 3, 3);
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
