/** @file
	Parser: @b table class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vhashfile.C,v 1.2 2001/10/22 13:52:20 parser Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhashfile.h"
#include "pa_threads.h"


#ifdef HAVE_DB_H
#	include <db.h>
#endif

// defines

#define PA_DB_ACCESS_METHOD DB_BTREE

// methods

void VHashfile::check(const char *operation, const String *source, int error) {
	switch(error) {
	case 0: 
		// no error
		break; 
	
	case DB_KEYEXIST:
		// DB_KEYEXIST is a "normal" return, so should not be
        // thrown as an error
		break; 

	case DB_RUNRECOVERY:
		throw Exception(0, 0, 
			&get_file_spec(source), 
			"db %s error, run recovery utility", 
			operation);
		
	default:
		throw Exception(0, 0, 
			&get_file_spec(source), 
			"db %s error: %s (%d)", 
			operation, strerror(errno), errno);
	}
}

/** 
	@test some caching manager of opened files
	@test string pieces [get/put preserve lang]
*/
void VHashfile::put_field(const String& name, Value *value) {
	DB *db;
	// open
	DB_INFO dbinfo;
	memset(&dbinfo, 0, sizeof(dbinfo));
	check("open/create", &name, db_open(
		get_file_spec_cstr(&name), 
		PA_DB_ACCESS_METHOD, 
		DB_CREATE /*| DB_THREAD*/,
		0666, 
		0, &dbinfo, &db));

	try {
		// put

		// key
		const char *cstr_key=name.cstr(String::UL_AS_IS);
		DBT key={
			(void *)cstr_key,
			name.size()
		};

		// data
		const String& string=value->as_string();
		const char *cstr_data=string.cstr(String::UL_AS_IS);
		DBT data={
			(void *)cstr_data,
			string.size()
		};
		check("put", &name, db->put(db, 0/*DB_TXN*/, &key, &data, 0/*flags*/));

	} catch(...) {
		check("close", &name, db->close(db, 0));
		/*re*/throw;
	}

	// close
	check("close", &name, db->close(db, 0));
}

Value *VHashfile::get_field(const String& name) {
	Value *result=0;

	DB *db;
	// open
	DB_INFO dbinfo;
	memset(&dbinfo, 0, sizeof(dbinfo));
	check("open", &name, db_open(
		get_file_spec_cstr(&name), 
		PA_DB_ACCESS_METHOD, 
		0 /*| DB_THREAD*/,
		0, 
		0, &dbinfo, &db));

	try {
		// get

		// key
		const char *cstr_key=name.cstr(String::UL_AS_IS);
		DBT key={
			(void *)cstr_key,
			name.size()
		};

		// data
		DBT data={0}; // must be zeroed
		check("get", &name, db->get(db, 0/*DB_TXN*/, &key, &data, 0/*flags*/));
		
		char *request_data=(char *)malloc(data.size);
		memcpy(request_data, data.data, data.size);
		result=NEW VString(*NEW String(pool(), request_data, data.size, true/*tainted*/));

	} catch(...) {
		check("close", &name, db->close(db, 0));
		/*re*/throw;
	}

	// close
	check("close", &name, db->close(db, 0));
	
	// return
	return result;
}

Hash *VHashfile::get_hash() {
	return 0;
}

#endif
