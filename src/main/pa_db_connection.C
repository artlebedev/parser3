/** @file
	Parser: Charset connection implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.C,v 1.1 2001/10/22 16:44:42 parser Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_db_connection.h"
#include "pa_exception.h"

void DB_Connection::check(const char *operation, const String *source, int error) {
	switch(error) {
	case 0: 
		// no error
		break; 
	
	case DB_KEYEXIST:
		// DB_KEYEXIST is a "normal" return, so should not be
		// thrown as an error
		break; 

	case DB_RUNRECOVERY:
		// mark as unsafe, so not to cache it
		needs_recovery=true;
		throw Exception(0, 0, 
			&ffile_spec, 
			"db %s error, run recovery utility", 
			operation);
		
	default:
		throw Exception(0, 0, 
			&ffile_spec, 
			"db %s error: %s (%d)", 
			operation, strerror(errno), errno);
	}
}

DB_Connection::DB_Connection(Pool& pool, const String& afile_spec, DB_ENV& adbenv) : Pooled(pool),
	fdbenv(adbenv),
	ffile_spec(afile_spec),
	fservices_pool(0), db(0), needs_recovery(false), tid(0),
	time_used(0) {
}

void DB_Connection::connect() { 
	// open
	DB_INFO dbinfo;
	memset(&dbinfo, 0, sizeof(dbinfo));
	check("open/create", &ffile_spec, db_open(
		ffile_spec.cstr(String::UL_FILE_SPEC), 
		PA_DB_ACCESS_METHOD, 
		DB_CREATE /*| DB_THREAD*/,
		0666, 
		&fdbenv, &dbinfo, &db));
}

///	@test string pieces [get/put preserve lang]
void DB_Connection::put(const String& key, const String& data) {
	// key
	const char *cstr_key=key.cstr(String::UL_AS_IS);
	DBT dbt_key={
		(void *)cstr_key,
		key.size()
	};

	// data
	const char *cstr_data=data.cstr(String::UL_AS_IS);
	DBT dbt_data={
		(void *)cstr_data,
		data.size()
	};
	check("put", &key, db->put(db, tid, &dbt_key, &dbt_data, 0/*flags*/));
}

String *DB_Connection::get(const String& key) {
	// key
	const char *cstr_key=key.cstr(String::UL_AS_IS);
	DBT dbt_key={
		(void *)cstr_key,
		key.size()
	};

	// data
	DBT dbt_data={0}; // must be zeroed
	check("get", &key, db->get(db, tid, &dbt_key, &dbt_data, 0/*flags*/));

	if(dbt_data.size) {
		char *request_data=(char *)malloc(dbt_data.size);
		memcpy(request_data, dbt_data.data, dbt_data.size);
		return NEW String(pool(), request_data, dbt_data.size, true/*tainted*/);
	} else
		return 0;
}

#endif
