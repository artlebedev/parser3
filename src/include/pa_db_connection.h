/** @file
	Parser: sql driver connection decl.
	global sql driver connection, must be thread-safe

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.h,v 1.9 2001/10/25 13:17:53 paf Exp $
*/

#ifndef PA_DB_CONNECTION_H
#define PA_DB_CONNECTION_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_db_manager.h"

#ifdef HAVE_DB_H
#	include <db.h>
#endif

// defines

// forwards

class DB_Table;

/// sql driver connection
class DB_Connection : public Pooled {
	friend DB_Table;
public:

	DB_Connection(Pool& pool, const String& db_home);

	void set_services(Pool *aservices_pool) {
		time_used=time(0); // they started to use at this time
		fservices_pool=aservices_pool;
	}
	bool expired(time_t older_dies) {
		return time_used<older_dies;
	}

	void close() {
		DB_manager->close_connection(fdb_home, *this);
	}
	bool connected() { return fconnected; }
	void connect();
	void disconnect();	
	bool ping() { return errors==0; }

	/**
		connect to specified file_name, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	DB_Table& get_table(const String& file_name, const String& request_origin);
	void clear_dbfile(const String& file_name);

private: // table cache

	DB_Table *get_table_from_cache(const String& file_name);
	void put_table_to_cache(const String& file_name, DB_Table& table);
	void maybe_expire_table_cache();
private:
	time_t prev_expiration_pass_time;

private: // for DB_Table

	/// caches table
	void close_table(const String& file_name, DB_Table& table);

private:

	time_t time_used;
	Pool *fservices_pool;

	const String& fdb_home;
	bool fconnected;
	DB_ENV dbenv;
	int errors;
	Hash table_cache;

private:

	void check(const char *operation, const String *source, int error);
};

#endif
