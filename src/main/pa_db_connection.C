/** @file
	Parser: sql driver connection implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.C,v 1.13 2001/10/26 13:48:19 paf Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_db_connection.h"
#include "pa_db_table.h"
#include "pa_exception.h"
#include "pa_threads.h"
#include "pa_stack.h"
#include "pa_common.h"

// consts

const int EXPIRE_UNUSED_TABLE_SECONDS=60;
const int CHECK_EXPIRED_TABLES_SECONDS=EXPIRE_UNUSED_TABLE_SECONDS*2;

const char *DB_WARNING1="Finding last valid log LSN";

// callbacks

static void db_paniccall(DB_ENV *dbenv, int error) {
	throw Exception(0, 0, 
		0, 
		"db_panic: %s (%d)", 
		strerror(error), error);
}

static void db_errcall(const char *, char *buffer) {
	// were it warning?
	if(strncmp(buffer, DB_WARNING1, strlen(DB_WARNING1))==0)
		return;

	throw Exception(0, 0, 
		0, 
		"db_err: %s", 
		buffer);
}

static void expire_table(const Hash::Key& key, Hash::Val *& value, void *info) {
	DB_Connection& table=*static_cast<DB_Connection *>(value);
	time_t older_dies=reinterpret_cast<time_t>(info);

	if(table.expired(older_dies)) {
		table.~DB_Connection();  value=0;
	}
}

// DB_Connection

DB_Connection::DB_Connection(Pool& apool, const String& adb_home) : Pooled(apool),
	time_used(0),
	fdb_home(adb_home), 
	table_cache(apool),
	prev_expiration_pass_time(0) {
	//_asm  int 3;
	memset(&dbenv, 0, sizeof(dbenv));
	dbenv.db_paniccall=db_paniccall;
	dbenv.db_errcall=db_errcall;

	char DB_DATA_DIR__VALUE[MAX_STRING];
	char DB_LOG_DIR__VALUE[MAX_STRING];
	char DB_TMP_DIR__VALUE[MAX_STRING];

	const char *db_home_cstr=fdb_home.cstr(String::UL_FILE_SPEC);

	snprintf(DB_DATA_DIR__VALUE, MAX_STRING, "DB_DATA_DIR %s", db_home_cstr);
	snprintf(DB_LOG_DIR__VALUE, MAX_STRING, "DB_LOG_DIR %s", db_home_cstr);
	snprintf(DB_TMP_DIR__VALUE, MAX_STRING, "DB_TMP_DIR %s", db_home_cstr);

	char *db_config[] = {
		DB_DATA_DIR__VALUE,
		DB_LOG_DIR__VALUE,
		DB_TMP_DIR__VALUE, 
		0
	};

	u_int32_t flags=
		DB_THREAD
		| DB_CREATE 
		| DB_INIT_MPOOL | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_TXN;

/*	try {
		// 1: trying to open with SOFT RECOVER option set
		check("db_appinit soft recover", &fdb_home, db_appinit(
			db_home_cstr,
			db_config, 
			&dbenv, 
			flags | DB_RECOVER
		));
	} catch(...) {
		try {*/
			// 2: trying to open WITHOUT SOFT RECOVER option set
			check("db_appinit no recover", &fdb_home, db_appinit(
				db_home_cstr,
				db_config, 
				&dbenv, 
				flags
			));
/*		} catch(...) {
			// 3: trying to open with FATAL RECOVER option set
			check("db_appinit fatal recover", &fdb_home, db_appinit(
				db_home_cstr,
				db_config, 
				&dbenv, 
				flags | DB_RECOVER_FATAL
			));
		}
	}*/
}

DB_Connection::~DB_Connection() {
	// close tables
	table_cache.for_each(expire_table, 
		reinterpret_cast<void *>(0/* =in the past = expire[close] all*/));
	
	// destroy connection data
	check("db_appexit", &fdb_home, db_appexit(&dbenv));
}


void DB_Connection::check(const char *operation, const String *source, int error) {
	switch(error) {
	case 0: 
		// no error
		break; 
	
	default:
		throw Exception(0, 0, 
			source, 
			"db %s error: %s (%d)", 
			operation, strerror(error), error);
	}
}

DB_Table_ptr DB_Connection::get_table_ptr(const String& request_file_name, const String *origin) {
	if(!used)
		throw(0, 0,
			origin,
			"note to parser3 programmer: your forgot about DB_Connection_usage");

	// first trying to get cached table
	DB_Table *result=get_table_from_cache(request_file_name);
	if(!result) { // no cached table
		// make global_file_name C-string on global pool
		const char *request_file_name_cstr=request_file_name.cstr(String::UL_AS_IS);
		char *global_file_name_cstr=(char *)malloc(strlen(request_file_name_cstr)+1);
		strcpy(global_file_name_cstr, request_file_name_cstr);
		// make global_file_name string on global pool
		String& global_file_name=*new(this->pool()) String(this->pool(), global_file_name_cstr);
		
		// allocate in global pool 
		// associate with services[request]
		// NOTE: never freed up!
		result=new(this->pool()) DB_Table(this->pool(), global_file_name, *this);
		// cache it
		put_table_to_cache(global_file_name, *result);
	}

	// return auto-it
	return DB_Table_ptr(result);
}
void DB_Connection::clear_dbfile(const String& file_name) {
	// open&clear
	DB *db;
	DB_INFO dbinfo;
	memset(&dbinfo, 0, sizeof(dbinfo));
	check("open(clear)", &file_name, db_open(
		file_name.cstr(String::UL_FILE_SPEC), 
		PA_DB_ACCESS_METHOD, 
		DB_CREATE | DB_TRUNCATE/* used in single thread, no need for |DB_THREAD*/,
		0666, 
		&dbenv, &dbinfo, &db));

	check("close", &file_name, db->close(db, 0/*flags*/));  db=0; 
}

// table cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
DB_Table *DB_Connection::get_table_from_cache(const String& file_name) { 
	SYNCHRONIZED;

	return static_cast<DB_Table *>(table_cache.get(file_name));
}

void DB_Connection::put_table_to_cache(const String& file_name, DB_Table& table) { 
	SYNCHRONIZED;

	table_cache.put(file_name, &table);
}

void DB_Connection::maybe_expire_table_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_TABLES_SECONDS) {
		table_cache.for_each(expire_table, 
			reinterpret_cast<void *>(now-EXPIRE_UNUSED_TABLE_SECONDS));

		prev_expiration_pass_time=now;
	}
}

#endif
