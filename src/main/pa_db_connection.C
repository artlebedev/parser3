/** @file
	Parser: Charset connection implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.C,v 1.2 2001/10/23 12:41:05 parser Exp $
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
			operation, strerror(error), error);
	}
}

DB_Connection::DB_Connection(Pool& pool, const String& afile_spec, DB_ENV& adbenv) : Pooled(pool),
	fdbenv(adbenv),
	ffile_spec(afile_spec),
	fservices_pool(0), db(0), ftid(0), needs_recovery(false), 
	time_used(0) {
}

void DB_Connection::connect() { 
	// open
	DB_INFO dbinfo;
	memset(&dbinfo, 0, sizeof(dbinfo));
	check("open/create", &ffile_spec, db_open(
		ffile_spec.cstr(String::UL_FILE_SPEC), 
		PA_DB_ACCESS_METHOD, 
		DB_CREATE /* used in single thread, no need for |DB_THREAD*/,
		0666, 
		&fdbenv, &dbinfo, &db));
}
/// @todo this one of reasons of not having ^try for now
void DB_Connection::disconnect() { 
	check("close", &ffile_spec, db->close(db, 0/*flags*/));  db=0; 
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
	check("put", &key, db->put(db, ftid, &dbt_key, &dbt_data, 0/*flags*/));
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
	int error=db->get(db, ftid, &dbt_key, &dbt_data, 0/*flags*/);
	if(error==DB_NOTFOUND)
		return 0;
	else {
		check("get", &key, error);

		if(dbt_data.size) {
			char *request_data=(char *)malloc(dbt_data.size);
			memcpy(request_data, dbt_data.data, dbt_data.size);
			return NEW String(pool(), request_data, dbt_data.size, true/*tainted*/);
		} else
			return NEW String(pool());
	}		
}

void DB_Connection::_delete(const String& key) {
	// key
	const char *cstr_key=key.cstr(String::UL_AS_IS);
	DBT dbt_key={
		(void *)cstr_key,
		key.size()
	};

	int error=db->del(db, ftid, &dbt_key, 0/*flags*/);
	if(error!=DB_NOTFOUND)
		check("del", &key, error);
}

#endif
