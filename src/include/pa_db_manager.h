/** @file
	Parser: sql driver manager decl.
	global sql driver manager, must be thread-safe

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_manager.h,v 1.2 2001/10/23 12:41:05 parser Exp $
*/

#ifndef PA_DB_DRIVER_MANAGER_H
#define PA_DB_DRIVER_MANAGER_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_hash.h"

#ifdef HAVE_DB_H
#	include <db.h>
#endif

// defines

// forwards

class DB_Connection;

/// sql driver manager
class DB_Manager : public Pooled {
	friend DB_Connection;
public:

	DB_Manager(Pool& pool);
	~DB_Manager();

	/** 
		connect to specified file_spec, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	DB_Connection& get_connection(const String& file_spec, const String& request_origin);
	void clear_dbfile(const String& file_spec);

private: // connection cache

	DB_Connection *get_connection_from_cache(const String& file_spec);
	void put_connection_to_cache(const String& file_spec, DB_Connection& connection);
	void maybe_expire_connection_cache();
private:
	time_t prev_expiration_pass_time;

private: // for DB_Connection

	/// caches connection
	void close_connection(const String& file_spec, DB_Connection& connection);

private:

	Hash connection_cache;
	DB_ENV dbenv;

private:

	void check(const char *operation, const String *source, int error);
};

/// global
extern DB_Manager *DB_manager;

#endif
