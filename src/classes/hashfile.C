/** @file
	Parser: @b hashfile parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: hashfile.C,v 1.4 2001/10/23 12:53:22 parser Exp $
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

static void _delete(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	// key
	const String &key=params->as_string(0, "key must be string");

	// connection
	DB_Connection& connection=self.get_connection(&method_name);

	connection._delete(key);
}

static void _clear(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	
	// file_spec
	const String &file_spec=params->as_string(0, "filename must be string");

	DB_manager->clear_dbfile(r.absolute(file_spec));
}

// constructor

MHashfile::MHashfile(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), HASH_CLASS_NAME));

	// ^hashfile::open[filename]
	add_native_method("open", Method::CT_DYNAMIC, _open, 1, 1);
	// ^transaction{code}
	add_native_method("transaction", Method::CT_DYNAMIC, _transaction, 1, 1);
	// ^hashfile.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 1, 1);
	// ^hashfile:clear[filename]
	add_native_method("clear", Method::CT_STATIC, _clear, 1, 1);
	// ^pack[]
	add_native_method("pack", Method::CT_DYNAMIC, _pack, 0, 0);
/*
	// ^cache[key](seconds){code}
	add_native_method("cache", Method::CT_DYNAMIC, _cache, 3, 3);
	// ^cancel[]
	add_native_method("cancel", Method::CT_DYNAMIC, _cancel, 0, 0);
	// ^hash[]
	add_native_method("hash", Method::CT_DYNAMIC, _hash, 0, 0);
	*/
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
