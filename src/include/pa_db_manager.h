/** @file
	Parser: sql driver manager decl.
	global sql driver manager, must be thread-safe

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_manager.h,v 1.3 2001/10/25 13:17:53 paf Exp $
*/

#ifndef PA_DB_MANAGER_H
#define PA_DB_MANAGER_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_hash.h"

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
		connect to specified db_home, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	DB_Connection& get_connection(const String& db_home, const String& request_origin);

private: // connection cache, never expires

	DB_Connection *get_connection_from_cache(const String& db_home);
	void put_connection_to_cache(const String& db_home, DB_Connection& connection);
	void maybe_expire_connection_cache();
private:
	time_t prev_expiration_pass_time;

private: // for DB_Connection

	/// caches connection
	void close_connection(const String& db_home, DB_Connection& connection);

private:

	Hash connection_cache;

};

/// global
extern DB_Manager *DB_manager;

#endif
