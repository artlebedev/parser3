/** @file
	Parser: sql driver manager decl.
	global sql driver manager, must be thread-safe

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_sql_driver_manager.h,v 1.17 2001/11/08 11:04:12 paf Exp $
*/

#ifndef PA_SQL_DRIVER_MANAGER_H
#define PA_SQL_DRIVER_MANAGER_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_sql_driver.h"
#include "pa_hash.h"
#include "pa_table.h"
#include "pa_string.h"
#include "pa_cache_managers.h"

// defines

#define MAIN_SQL_NAME "SQL"
#define MAIN_SQL_DRIVERS_NAME "drivers"

// forwards

class SQL_Connection;

/// sql driver manager
class SQL_Driver_manager : public Pooled, public Cache_manager {
	friend class SQL_Connection;
public:

	SQL_Driver_manager(Pool& apool);
	~SQL_Driver_manager();

	/** 
		connect to specified url, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	SQL_Connection& get_connection(const String& url, const String& request_origin,
		Table *protocol2driver_and_client);

private: // driver cache

	SQL_Driver *get_driver_from_cache(const String& protocol);
	void put_driver_to_cache(const String& protocol, SQL_Driver& driver);

private: // connection cache

	SQL_Connection *get_connection_from_cache(const String& url);
	void put_connection_to_cache(const String& url, SQL_Connection& connection);
private:
	time_t prev_expiration_pass_time;

private: // for SQL_Connection

	/// caches connection
	void close_connection(const String& url, SQL_Connection& connection);

private:

	Hash driver_cache;
	Hash connection_cache;

public: // Cache_manager

	virtual Value& get_status(Pool& pool, const String *source);
	virtual void maybe_expire_cache();

};

/// global
extern SQL_Driver_manager *SQL_driver_manager;

#endif
