/** @file
	Parser: Charset table implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_table.C,v 1.2 2001/10/26 07:06:17 paf Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_db_table.h"
#include "pa_exception.h"

// defines

#define READ_ADDITIONAL_FLAGS DB_RMW

// consts

const int DATA_STRING_SERIALIZED_VERSION=0x0100;

// helper types

#ifndef DOXYGEN
struct Data_string_serialized_prolog {
	int version;
	time_t time_to_die;
};
#endif

// DB_Table

DB_Table::DB_Table(Pool& pool, const String& afile_spec, DB_Connection& aconnection) : Pooled(pool),
	time_used(0), fservices_pool(0), 
	fconnection(aconnection),
	dbenv(aconnection.dbenv),
	ffile_spec(afile_spec), file_spec_cstr(afile_spec.cstr(String::UL_FILE_SPEC)),
	db(0), ftid(0), ftid_has_parent(false), errors(0) {
}

void DB_Table::check(const char *operation, const String *source, int error) {
	switch(error) {
	case 0: 
		// no error
		break; 
	
	case DB_KEYEXIST:
		// DB_KEYEXIST is a "normal" return, so should not be
		// thrown as an error
		break; 

	case DB_RUNRECOVERY:
		errors++;
		throw Exception(0, 0, 
			source,
			"action failed, RUN RECOVERY UTILITY. db %s error, real filename '%s'", 
			operation, file_spec_cstr);
		
	default:
		errors++;
		throw Exception(0, 0, 
			source, 
			"action failed. db %s error: %s (%d), real filename '%s'", 
			operation, strerror(error), error, file_spec_cstr);
	}
}

void DB_Table::key_string_to_dbt(const String& key_string, DBT& key_result) {
	memset(&key_result, 0, sizeof(key_result));
	key_result.data=key_string.cstr(String::UL_AS_IS);
	key_result.size=key_string.size();
}

String& DB_Table::key_dbt_to_string(const DBT& key_dbt) {
	String& result=*new(*fservices_pool) String(*fservices_pool);
	if(key_dbt.size) {
		char *request_data=(char *)malloc(key_dbt.size);
		memcpy(request_data, key_dbt.data, key_dbt.size);
		result.APPEND_TAINTED(request_data, key_dbt.size, file_spec_cstr, 0/*line*/);
	}
	return result;
}

void DB_Table::data_string_to_dbt(const String& data_string, time_t time_to_die, 
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

String *DB_Table::data_dbt_to_string(const DBT& data_dbt) {
	Data_string_serialized_prolog& prolog=
		*static_cast<Data_string_serialized_prolog *>(data_dbt.data);

	if(prolog.version!=DATA_STRING_SERIALIZED_VERSION)
		throw Exception(0, 0,
			&ffile_spec,
			"data string version 0x%04X not equal to 0x%04X, recreate file",
				prolog.version, DATA_STRING_SERIALIZED_VERSION);

	if(prolog.time_to_die/*specified*/ && prolog.time_to_die <= time(0)/*expired*/)
		return 0;

	String& result=*new(*fservices_pool) String(*fservices_pool);
	result.deserialize(
		sizeof(Data_string_serialized_prolog), 
		data_dbt.data, data_dbt.size, file_spec_cstr);
	return &result;
}

void DB_Table::connect() { 
	// open
	DB_INFO dbinfo;
	memset(&dbinfo, 0, sizeof(dbinfo));
	check("open/create", &ffile_spec, db_open(
		file_spec_cstr, 
		PA_DB_ACCESS_METHOD, 
		DB_CREATE /* used in single thread, no need for |DB_THREAD*/,
		0666, 
		&dbenv, &dbinfo, &db));
}
/// @todo this one of reasons of not having ^try for now
void DB_Table::disconnect() { 
	check("close", &ffile_spec, db->close(db, 0/*flags*/));  db=0; 
}

void DB_Table::put(const String& key, const String& data, time_t time_to_die) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);
	DBT dbt_data;  data_string_to_dbt(data, time_to_die, dbt_data);
	check("put", &key, db->put(db, ftid, &dbt_key, &dbt_data, 0/*flags*/));
}

String *DB_Table::get(const String& key) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);
	DBT dbt_data={0}; // must be zeroed
	int error=db->get(db, ftid, &dbt_key, &dbt_data, READ_ADDITIONAL_FLAGS/*flags*/);
	if(error==DB_NOTFOUND)
		return 0;
	else {
		check("get", &key, error);
		String *result=data_dbt_to_string(dbt_data);
		if(!result) // save efforts by deleting expired keys
			check("del expired", &key, db->del(db, ftid, &dbt_key, 0/*flags*/));
		return result;
	}		
}

void DB_Table::remove(const String& key) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);

	int error=db->del(db, ftid, &dbt_key, 0/*flags*/);
	if(error!=DB_NOTFOUND)
		check("del", &key, error);
}

// DB_Cursor

DB_Cursor::DB_Cursor(
					 DB_Table& atable, 
					 const String *asource) : fsource(asource), ftable(atable), cursor(0) {
	check("cursor", fsource, ftable.db->cursor(ftable.db,
		ftable.ftid, &cursor, 0/*flags*/));
}

DB_Cursor::~DB_Cursor() {
	if(cursor) {
		check("c_close", fsource, cursor->c_close(cursor));  cursor=0;
	}
}

bool DB_Cursor::get(String *& key, String *& data, u_int32_t flags) {
	DBT dbt_key={0}; // must be zeroed
	DBT dbt_data={0}; // must be zeroed
	
	int error=cursor->c_get(cursor, &dbt_key, &dbt_data, flags | READ_ADDITIONAL_FLAGS);
	if(error==DB_NOTFOUND)
		return false;

	check("c_get", fsource, error);

	if(data=data_dbt_to_string(dbt_data)) // not expired
		key=&key_dbt_to_string(dbt_key);
	else {
		// save efforts by deleting expired keys
		remove(0/*flags*/);
		key=0;
	}
	return true;
}

void DB_Cursor::remove(u_int32_t flags) {
	check("c_del", fsource,  cursor->c_del(cursor, flags));
}

#endif
