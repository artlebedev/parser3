/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver_manager.h,v 1.9 2001/09/14 15:41:59 parser Exp $


	global sql driver manager, must be thread-safe
*/

#ifndef PA_SQL_DRIVER_MANAGER_H
#define PA_SQL_DRIVER_MANAGER_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_sql_driver.h"
#include "pa_hash.h"
#include "pa_stack.h"
#include "pa_table.h"
#include "pa_string.h"


// defines

#define MAIN_SQL_NAME "SQL"
#define MAIN_SQL_DRIVERS_NAME "drivers"

// forwards

class SQL_Connection;

/**	sql driver manager
		maintains 
		- driver cache  which never expires
		- connection cache  unused connections got expired
*/
class SQL_Driver_manager : public Pooled {
	friend SQL_Connection;
public:

	SQL_Driver_manager(Pool& pool) : Pooled(pool),
		driver_cache(pool),
		connection_cache(pool),
		prev_expiration_pass_time(0) {
	}

	/** 
		connect to specified url, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	SQL_Connection& get_connection(const String& url, Table *protocol2driver_and_client);

private: // driver cache

	SQL_Driver *get_driver_from_cache(const String& protocol);
	void put_driver_to_cache(const String& protocol, SQL_Driver& driver);

private: // connection cache

	SQL_Connection *get_connection_from_cache(const String& url);
	void put_connection_to_cache(const String& url, SQL_Connection& connection);
	void maybe_expire_connection_cache();
private:
	time_t prev_expiration_pass_time;

private: // for SQL_Connection

	/// caches connection
	void close_connection(const String& url, SQL_Connection& connection);

private:

	Hash driver_cache;
	Hash connection_cache;
};

/// global
extern SQL_Driver_manager *SQL_driver_manager;

#endif
