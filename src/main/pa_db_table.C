/** @file
	Parser: Charset table implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_table.C,v 1.5 2001/10/27 13:00:09 paf Exp $
*/

#include "pa_config_includes.h"
#ifdef DB2

#include "pa_db_table.h"
#include "pa_exception.h"
#include "pa_db_connection.h"

// defines

#define DEADLOCK_POSSIBILITY_REDUCTION_FLAGS DB_RMW

// consts

const int DATA_STRING_SERIALIZED_VERSION=0x0100;

// helper types

#ifndef DOXYGEN
struct Data_string_serialized_prolog {
	int version;
	time_t time_to_die;
};

struct DBT_auto : public DBT {
	DBT_auto() {
		data=0; 
		size=ulen=dlen=doff=0;
		flags=DB_DBT_MALLOC;
	}

	~DBT_auto() {
		if(flags & DB_DBT_MALLOC)
			free(data);
	}
};

#endif

// DB_Table

DB_Table::DB_Table(Pool& pool, const String& afile_name, DB_Connection& aconnection) : Pooled(pool),
	time_used(0), fservices_pool(0), 
	fconnection(aconnection),
	dbenv(aconnection.dbenv),
	ffile_name(afile_name), file_name_cstr(afile_name.cstr(String::UL_FILE_SPEC)),
	db(0) {

	// open
	DB_INFO dbinfo;
	memset(&dbinfo, 0, sizeof(dbinfo));
	check("open/create", &ffile_name, db_open(
		file_name_cstr, 
		PA_DB_ACCESS_METHOD, 
		DB_THREAD 
		| DB_CREATE /* used in single thread, no need for |DB_THREAD*/,
		0666, 
		&dbenv, &dbinfo, &db));
}
/// @todo this one of reasons of not having ^try for now
DB_Table::~DB_Table() { 
	// close
	check("close", &ffile_name, db->close(db, 0/*flags*/));  db=0; 
}

void DB_Table::use() {
	fconnection.use();

	time_used=time(0); // they started to use at this time
	used++;
}
void DB_Table::unuse() {
	used--;

	fconnection.unuse();
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
		throw Exception(0, 0, 
			source,
			"action failed, RUN RECOVERY UTILITY. db %s error, real filename '%s'", 
			operation, file_name_cstr);
		
	default:
		throw Exception(0, 0, 
			source, 
			"action failed. db %s error: %s (%d), real filename '%s'", 
			operation, strerror(error), error, file_name_cstr);
	}
}

void DB_Table::key_string_to_dbt(const String& key_string, DBT& key_result) {
	memset(&key_result, 0, sizeof(key_result));
	key_result.data=key_string.cstr(String::UL_AS_IS);
	key_result.size=key_string.size();
}

String& DB_Table::key_dbt_to_string(Pool& pool, const DBT& key_dbt) {
	String& result=*new(*fservices_pool) String(*fservices_pool);
	if(key_dbt.size) {
		char *request_data=(char *)pool.malloc(key_dbt.size);
		memcpy(request_data, key_dbt.data, key_dbt.size);
		result.APPEND_TAINTED(request_data, key_dbt.size, file_name_cstr, 0/*line*/);
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

String *DB_Table::data_dbt_to_string(Pool& pool, const DBT& data_dbt) {
	Data_string_serialized_prolog& prolog=
		*static_cast<Data_string_serialized_prolog *>(data_dbt.data);

	if(prolog.version!=DATA_STRING_SERIALIZED_VERSION)
		throw Exception(0, 0,
			&ffile_name,
			"data string version 0x%04X not equal to 0x%04X, recreate file",
				prolog.version, DATA_STRING_SERIALIZED_VERSION);

	if(prolog.time_to_die/*specified*/ && prolog.time_to_die <= time(0)/*expired*/)
		return 0;

	String& result=*new(pool) String(pool);
	if(data_dbt.size) {
		char *data=(char *)pool.malloc(data_dbt.size);
		memcpy(data, data_dbt.data, data_dbt.size);
		result.deserialize(
			sizeof(Data_string_serialized_prolog), 
			data, data_dbt.size, file_name_cstr);
	}
	return &result;
}

void DB_Table::put(DB_Transaction *t, const String& key, const String& data, time_t time_to_die) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);
	DBT dbt_data;  data_string_to_dbt(data, time_to_die, dbt_data);
	check("put", &key, db->put(db, t?t->id():0, &dbt_key, &dbt_data, 0/*flags*/));
}

String *DB_Table::get(DB_Transaction *t, Pool& pool, const String& key) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);
	DBT_auto dbt_data;
	int error=db->get(db, t?t->id():0, &dbt_key, &dbt_data, 
		DEADLOCK_POSSIBILITY_REDUCTION_FLAGS);
	if(error==DB_NOTFOUND)
		return 0;
	else {
		check("get", &key, error);
		String *result=data_dbt_to_string(pool, dbt_data);
		if(!result) // save efforts by deleting expired keys
			check("del expired", &key, db->del(db, t?t->id():0, &dbt_key, 0/*flags*/));
		return result;
	}		
}

void DB_Table::remove(DB_Transaction *t, const String& key) {
	DBT dbt_key;  key_string_to_dbt(key, dbt_key);

	int error=db->del(db, t?t->id():0, &dbt_key, 0/*flags*/);
	if(error!=DB_NOTFOUND)
		check("del", &key, error);
}

// DB_Transaction

DB_Transaction::DB_Transaction(Pool& apool, DB_Table& atable, DB_Transaction *& aparent_ref) : 
	fpool(apool), ftable(atable), fparent_ref(aparent_ref), 
		parent(aparent_ref),
		fid(0), marked_to_rollback(false) {

	check("txn_begin", 
		&ftable.file_name(), txn_begin(ftable.dbenv.tx_info, parent?parent->fid:0, &fid));

	aparent_ref=this;
}
DB_Transaction::~DB_Transaction() { 
	fparent_ref=parent;

	if(parent) // all in hands of our parent
		return;

	if(marked_to_rollback)
		check("txn_abort", &ftable.file_name(), txn_abort(fid));
	else
		check("txn_commit", &ftable.file_name(), txn_commit(fid));
}
void DB_Transaction::mark_to_rollback() {
	if(parent)
		parent->mark_to_rollback();
	else
		marked_to_rollback=true;
}

// DB_Cursor

DB_Cursor::DB_Cursor(
					 DB_Table& atable, DB_Transaction *transaction, 
					 const String *asource) : 
	ftable(atable), fsource(asource), 
	cursor(0) {

	check("cursor", fsource, ftable.db->cursor(ftable.db,
		transaction?transaction->id():0, &cursor, 0/*flags*/));
}

DB_Cursor::~DB_Cursor() {
	if(cursor)
		check("c_close", fsource, cursor->c_close(cursor));
}

bool DB_Cursor::get(Pool& pool, String *& key, String *& data, u_int32_t flags) {
	DBT dbt_key={0}; // must be zeroed
	DBT_auto dbt_data;
	
	int error=cursor->c_get(cursor, &dbt_key, &dbt_data, 
		DEADLOCK_POSSIBILITY_REDUCTION_FLAGS
		| flags 
	);
	if(error==DB_NOTFOUND)
		return false;

	check("c_get", fsource, error);

	if(data=data_dbt_to_string(pool, dbt_data)) // not expired
		key=&key_dbt_to_string(pool, dbt_key);
	else {
		// save efforts by deleting expired keys
		remove(0/*flags*/);
		key=0;
	}
	return true;
}

bool DB_Cursor::move(u_int32_t flags) {
	DBT dbt_key={0}; // must be zeroed
	DBT dbt_data={0}; // must be zeroed
	dbt_key.flags=dbt_data.flags=DB_DBT_PARTIAL; // just peep, not actually read [size=0]
	
	int error=cursor->c_get(cursor, &dbt_key, &dbt_data, 
		DEADLOCK_POSSIBILITY_REDUCTION_FLAGS
		| flags 
	);
	if(error==DB_NOTFOUND)
		return false;

	check("c_get", fsource, error);
	return true;
}

void DB_Cursor::remove(u_int32_t flags) {
	check("c_del", fsource,  cursor->c_del(cursor, flags));
}

#endif
