/** @file
	Parser: Charset connection implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.C,v 1.7 2001/10/24 09:40:01 parser Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_db_connection.h"
#include "pa_exception.h"

// consts

const int DATA_STRING_SERIALIZED_VERSION=0x0100;

// helper types

#ifndef DOXYGEN
struct Data_string_serialized_prolog {
	int version;
	time_t time_to_die;
};
#endif

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

void DB_Connection::key_string_to_dbt(const String& key_string, DBT& key_result) {
	memset(&key_result, 0, sizeof(key_result));
	key_result.data=key_string.cstr(String::UL_AS_IS);
	key_result.size=key_string.size();
}

String& DB_Connection::key_dbt_to_string(const DBT& key_dbt) {
	String& result=*new(*fservices_pool) String(*fservices_pool);
	if(key_dbt.size) {
		char *request_data=(char *)malloc(key_dbt.size);
		memcpy(request_data, key_dbt.data, key_dbt.size);
		result.APPEND_TAINTED(request_data, key_dbt.size, file_spec_cstr, 0/*line*/);
	}
	return result;
}

void DB_Connection::data_string_to_dbt(const String& data_string, time_t time_to_die, 
									   DBT& data_result) {
	memset(&data_result, 0, sizeof(data_result));

	data_string.serialize(
		sizeof(Data_string_serialized_prolog), 
		data_result.data, data_result.size);

	Data_string_serialized_prolog& prolog=
		*static_cast<Data_string_serialized_prolog *>(data_result.data);

	prolog.version=DATA_STRING_SERIALIZED_VERSION;
	prolog.time_to_die=time_to_die;
}

String *DB_Connection::data_dbt_to_string(const DBT& data_dbt) {
	Data_string_serialized_prolog& prolog=
		*static_cast<Data_string_serialized_prolog *>(data_dbt.data);

	if(prolog.version!=DATA_STRING_SERIALIZED_VERSION)
		throw Exception(0, 0,
			&ffile_spec,
			"data string version 0x%04X not equal to 0x%04X, recreate file",
				prolog.version, DATA_STRING_SERIALIZED_VERSION);

	if(prolog.time_to_die/*specified*/ && prolog.time_to_die >= time(0)/*expired*/)
		return 0;

	String& result=*new(*fservices_pool) String(*fservices_pool);
	result.deserialize(
		sizeof(Data_string_serialized_prolog), 
		data_dbt.data, data_dbt.size, file_spec_cstr);
	return &result;
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

void DB_Connection::put(const String& key, const String& data, time_t time_to_die) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);
	DBT dbt_data;  data_string_to_dbt(data, time_to_die, dbt_data);
	check("put", &key, db->put(db, ftid, &dbt_key, &dbt_data, 0/*flags*/));
}

String *DB_Connection::get(const String& key) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);
	DBT dbt_data={0}; // must be zeroed
	int error=db->get(db, ftid, &dbt_key, &dbt_data, 0/*flags*/);
	if(error==DB_NOTFOUND)
		return 0;
	else {
		check("get", &key, error);
		return data_dbt_to_string(dbt_data);
	}		
}

void DB_Connection::_delete(const String& key) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);

	int error=db->del(db, ftid, &dbt_key, 0/*flags*/);
	if(error!=DB_NOTFOUND)
		check("del", &key, error);
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

bool DB_Cursor::get(String *& key, String *& data, u_int32_t flags) {
	DBT dbt_key={0}; // must be zeroed
	DBT dbt_data={0}; // must be zeroed
	
	int error=cursor->c_get(cursor, &dbt_key, &dbt_data, flags);
	if(error==DB_NOTFOUND)
		return false;

	check("c_get", fsource, error);

	if(data=data_dbt_to_string(dbt_data)) // not expired
		key=&key_dbt_to_string(dbt_key);
	else
		key=0;
	return true;
}

#endif
