/** @file
	Parser: sql driver connection implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_db_connection.C,v 1.29 2001/11/05 12:08:11 paf Exp $

	developed with LIBDB 2.7.4
*/

#include "pa_config_includes.h"
#ifdef DB2

#include "pa_db_connection.h"
#include "pa_db_table.h"
#include "pa_exception.h"
#include "pa_threads.h"
#include "pa_stack.h"
#include "pa_common.h"
#include "pa_vtable.h"

// defines

// consts

/// @test increase
const int DB_CHECKPOINT_MINUTES=1; 

const int EXPIRE_UNUSED_TABLE_SECONDS=60;
const int CHECK_EXPIRED_TABLES_SECONDS=EXPIRE_UNUSED_TABLE_SECONDS*2;

// callbacks

static void db_paniccall(DB_ENV *dbenv, int error) {
	throw Exception(0, 0, 
		0, 
		"db_panic: %s (%d)", 
		strerror(error), error);
}

static void db_errcall(const char *, char *buffer) {
	throw Exception(0, 0, 
		0, 
		"db_err: %s", 
		buffer);
}

static void expire_table(const Hash::Key& key, Hash::Val *& value, void *info) {
	DB_Table& table=*static_cast<DB_Table *>(value);
	time_t older_dies=reinterpret_cast<time_t>(info);

	if(table.expired(older_dies)) {
		table.~DB_Table();  value=0;
	}
}

// DB_Connection

DB_Connection::DB_Connection(Pool& apool, const String& adb_home) : Pooled(apool),
	time_used(0),
	fdb_home(adb_home), used(0),
	table_cache(apool),
	prev_expiration_pass_time(0) {
	//_asm  int 3;
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
		(parser_multithreaded?DB_THREAD:0)
		| DB_CREATE 
		| DB_INIT_MPOOL | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_TXN;

	// prepare dbenv structure
	memset(&dbenv, 0, sizeof(dbenv));
	
	// error handlers
#if DB_VERSION_MINOR >= 7
	dbenv.db_paniccall=db_paniccall;
#endif
	dbenv.db_errcall=db_errcall;

	// lockdetector flags
	dbenv.lk_detect=DB_LOCK_RANDOM;

	// init
	check("db_appinit", &fdb_home, db_appinit(
		db_home_cstr,
		db_config, 
		&dbenv, 
		flags
		));

	// after some hang noticed this to be null
	if(!dbenv.tx_info) 
		throw Exception(0, 0,
			&fdb_home,
			"transaction system failed to initialize");
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
		if(error<0)
			throw Exception(0, 0, 
				source, 
				"db %s error: %d", 
				operation, error);
		else
			throw Exception(0, 0, 
				source, 
				"db %s error: %s (%d)", 
				operation, strerror(error), error);
	}
}

DB_Table_ptr DB_Connection::get_table_ptr(const String& request_file_name, const String *source) {
	// checkpoint
	{ 
		int error=txn_checkpoint(dbenv.tx_info, 0/*kbyte*/, DB_CHECKPOINT_MINUTES/*min*/);
		/*
		DB_INCOMPLETE if there were pages that needed to be written 
		but that memp_sync was unable to write immediately. 
		In this case, the txn_checkpoint call should be retried. 

		we'll just ignore that
		*/
		if(error!=DB_INCOMPLETE)
			check("checkpoint", source, error);
	}

	// first trying to get cached table
	DB_Table *result=get_table_from_cache(request_file_name);
	if(!result) { // no cached table
		// make global_file_name C-string on global pool
		const char *request_file_name_cstr=request_file_name.cstr();
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

// table cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
DB_Table *DB_Connection::get_table_from_cache(const String& file_name) { 
	SYNCHRONIZED;

	maybe_expire_table_cache();

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

static void add_table_to_status_table(const Hash::Key& key, Hash::Val *& value, void *info) {
	DB_Table& db_table=*static_cast<DB_Table *>(value);
	Table& status_table=*static_cast<Table *>(info);
	Pool& pool=status_table.pool();

	Array& row=*new(pool) Array(pool);

	// name
	row+=&db_table.file_name();
	// time
	time_t time_stamp=db_table.get_time_used();
	const char *unsafe_time_cstr=ctime(&time_stamp);
	int time_buf_size=strlen(unsafe_time_cstr);
	char *safe_time_buf=(char *)pool.malloc(time_buf_size);
	memcpy(safe_time_buf, unsafe_time_cstr, time_buf_size);
	row+=new(pool) String(pool, safe_time_buf, time_buf_size);
	// users
	char *users_buf=(char *)pool.malloc(MAX_NUMBER);
	row+=new(pool) String(pool, 
		users_buf, snprintf(users_buf, MAX_NUMBER, "%d", db_table.get_users_count()));

	status_table+=&row;
}
Value& DB_Connection::get_status(Pool& pool, const String *source) {
	Array& columns=*new(pool) Array(pool);
	columns+=new(pool) String(pool, "name");
	columns+=new(pool) String(pool, "time");
	columns+=new(pool) String(pool, "users");
	Table& table=*new(pool) Table(pool, 0, &columns, table_cache.size());

	table_cache.for_each(add_table_to_status_table, &table);

	return *new(pool) VTable(pool, &table);
}

#endif
