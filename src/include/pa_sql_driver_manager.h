/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver_manager.h,v 1.3 2001/04/17 19:00:36 paf Exp $


	global sql driver manager, must be thread-safe
*/

#ifndef PA_SQL_DRIVER_MANAGER_H
#define PA_SQL_DRIVER_MANAGER_H

#include "pa_pool.h"
#include "pa_sql_driver.h"
#include "pa_hash.h"
#include "pa_stack.h"
#include "pa_threads.h"
#include "pa_table.h"
#include "pa_string.h"

class SQL_Connection;

/// sql driver manager
class SQL_Driver_manager : public Pooled {
	friend SQL_Connection;
public:

	SQL_Driver_manager(Pool& pool) : Pooled(pool),
		driver_cache(pool),
		connection_cache(pool) {
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
