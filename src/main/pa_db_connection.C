/** @file
	Parser: sql driver connection implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.C,v 1.12 2001/10/26 07:07:01 paf Exp $
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

static void expire_table(Array::Item *value, void *info) {
	DB_Table& table=*static_cast<DB_Table *>(value);
	time_t older_dies=reinterpret_cast<time_t>(info);

	if(table.connected() && table.expired(older_dies))
		table.disconnect();
}
static void expire_tables(const Hash::Key& key, Hash::Val *value, void *info) {
	Stack& stack=*static_cast<Stack *>(value);
	for(int i=0; i<=stack.top_index(); i++)
		expire_table(stack.get(i), info);
}

// DB_Connection

DB_Connection::DB_Connection(Pool& pool, const String& adb_home) : Pooled(pool),
	time_used(0), fservices_pool(0), 
	fdb_home(adb_home), fconnected(false), errors(0),
	table_cache(pool),
	prev_expiration_pass_time(0) {
}

void DB_Connection::connect() {
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
		DB_CREATE 
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

	fconnected=true;
}

void DB_Connection::disconnect() {
	// close tables
	table_cache.for_each(expire_tables, 
		reinterpret_cast<void *>(0/* =in the past = expire[close] all*/));
	
	// destroy connection data
	check("db_appexit", &fdb_home, db_appexit(&dbenv));  fconnected=false;
}


void DB_Connection::check(const char *operation, const String *source, int error) {
	switch(error) {
	case 0: 
		// no error
		break; 
	
	default:
		errors++;
		throw Exception(0, 0, 
			source, 
			"db %s error: %s (%d)", 
			operation, strerror(error), error);
	}
}

DB_Table& DB_Connection::get_table(const String& request_file_name,
												   const String& request_origin) {
	Pool& pool=request_origin.pool(); // request pool											   

	// first trying to get cached table
	DB_Table *result=get_table_from_cache(request_file_name);
	if(result && !result->ping()) { // we have some cached table, is it pingable?
		result->disconnect(); // kill unpingabe=dead table
		result=0;
	}

	char *request_file_name_cstr;
	if(result)
		request_file_name_cstr=0; // calm, compiler
	else { // no cached table
		// make global_file_name C-string on global pool
		request_file_name_cstr=request_file_name.cstr(String::UL_AS_IS);
		char *global_file_name_cstr=(char *)malloc(strlen(request_file_name_cstr)+1);
		strcpy(global_file_name_cstr, request_file_name_cstr);
		// make global_file_name string on global pool
		String& global_file_name=*new(this->pool()) String(this->pool(), global_file_name_cstr);
		
		// allocate in global pool 
		// associate with services[request]
		// NOTE: never freed up!
		result=new(this->pool()) DB_Table(this->pool(), global_file_name, *this);
	}

	// associate with services[request]  (deassociates at close)
	result->set_services(&pool); 
	// if not connected yet, do that now, when result has services
	if(!result->connected())
		result->connect();
	// return it
	return *result;
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

void DB_Connection::close_table(const String& file_name, 
										  DB_Table& table) {
	// deassociate from services[request]
	table.set_services(0);
	put_table_to_cache(file_name, table);
}


// table cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
DB_Table *DB_Connection::get_table_from_cache(const String& file_name) { 
	SYNCHRONIZED;

	maybe_expire_table_cache();

	if(Stack *tables=static_cast<Stack *>(table_cache.get(file_name)))
		while(tables->top_index()>=0) { // there are cached tables to that 'file_name'
			DB_Table *result=static_cast<DB_Table *>(tables->pop());
			if(result->connected()) // not expired?
				return result;
		}

	return 0;
}

void DB_Connection::put_table_to_cache(const String& file_name, 
												 DB_Table& table) { 
	SYNCHRONIZED;

	Stack *tables=static_cast<Stack *>(table_cache.get(file_name));
	if(!tables) { // there are no cached tables to that 'file_name' yet?
		tables=NEW Stack(pool()); // NOTE: never freed up!
		table_cache.put(file_name, tables);
	}	
	tables->push(&table);
}

void DB_Connection::maybe_expire_table_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_TABLES_SECONDS) {
		table_cache.for_each(expire_tables, 
			reinterpret_cast<void *>(now-EXPIRE_UNUSED_TABLE_SECONDS));

		prev_expiration_pass_time=now;
	}
}

#endif
