/** @file
	Parser: Charset connection implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.C,v 1.3 2001/10/23 14:43:44 parser Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_db_connection.h"
#include "pa_exception.h"

// DB_Connection

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
			source,
			"action failed, RUN RECOVERY UTILITY. db %s error, real filename '%s'", 
			operation, file_spec_cstr);
		
	default:
		throw Exception(0, 0, 
			source, 
			"action failed. db %s error: %s (%d), real filename '%s'", 
			operation, strerror(error), error, file_spec_cstr);
	}
}

DB_Connection::DB_Connection(Pool& pool, const String& afile_spec, DB_ENV& adbenv) : Pooled(pool),
	fdbenv(adbenv),
	ffile_spec(afile_spec), file_spec_cstr(afile_spec.cstr(String::UL_FILE_SPEC)),
	fservices_pool(0), db(0), ftid(0), needs_recovery(false), 
	time_used(0) {
}

void DB_Connection::connect() { 
	// open
	DB_INFO dbinfo;
	memset(&dbinfo, 0, sizeof(dbinfo));
	check("open/create", &ffile_spec, db_open(
		file_spec_cstr, 
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

		String *result=new(*fservices_pool) String(*fservices_pool);
		dbt_to_string(dbt_data, *result);
		return result;
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

DB_Cursor DB_Connection::cursor(const String *source) {
	return DB_Cursor(*this, source);
}

void DB_Connection::dbt_to_string(DBT& dbt, String& result) {
	if(dbt.size) {
		char *request_data=(char *)malloc(dbt.size);
		memcpy(request_data, dbt.data, dbt.size);
		result.APPEND_TAINTED(request_data, dbt.size, file_spec_cstr, 0/*line*/);
	}
}

// DB_Cursor

DB_Cursor::DB_Cursor(
					 DB_Connection& aconnection, 
					 const String *asource) : fsource(asource), fconnection(aconnection), cursor(0) {
	check("cursor", fsource, fconnection.db->cursor(fconnection.db,
		fconnection.ftid, &cursor, 0/*flags*/));
}

DB_Cursor::~DB_Cursor() {
	if(cursor) {
		check("c_close", fsource, cursor->c_close(cursor));  cursor=0;
	}
}

bool DB_Cursor::get(String& key, String& data, u_int32_t flags) {
	DBT dbt_key={0}; // must be zeroed
	DBT dbt_data={0}; // must be zeroed
	
	int error=cursor->c_get(cursor, &dbt_key, &dbt_data, flags);
	if(error==DB_NOTFOUND)
		return false;

	check("c_get", fsource, error);

	dbt_to_string(dbt_key, key);
	dbt_to_string(dbt_data, data);
	return true;
}

#endif
