/** @file
	Parser: sql driver manager decl.
	global sql driver manager, must be thread-safe

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SQL_DRIVER_MANAGER_H
#define PA_SQL_DRIVER_MANAGER_H

#define IDENT_PA_SQL_DRIVER_MANAGER_H "$Id: pa_sql_driver_manager.h,v 1.41 2020/12/15 17:10:32 moko Exp $"


#include "pa_sql_driver.h"
#include "pa_hash.h"
#include "pa_table.h"
#include "pa_string.h"
#include "pa_cache_managers.h"
#include "pa_stack.h"

// defines

#define MAIN_SQL_NAME "SQL"
#define MAIN_SQL_DRIVERS_NAME "drivers"

// forwards

class SQL_Connection;

/// sql driver manager
class SQL_Driver_manager: public Cache_manager {
public:

	typedef HashString<SQL_Driver *> driver_cache_type;
	typedef Stack<SQL_Connection*> connection_cache_element_base_type;
	typedef HashString<connection_cache_element_base_type*> connection_cache_type;

private:
	
	friend class SQL_Connection;

	driver_cache_type driver_cache;
	connection_cache_type connection_cache;

public:

	SQL_Driver_manager();
	override ~SQL_Driver_manager();

	/** 
		connect to specified url, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	SQL_Connection* get_connection(const String& aurl, Table *protocol2driver_and_client, const char* arequest_charset, const char* adocument_root);

private: // driver cache

	SQL_Driver *get_driver_from_cache(driver_cache_type::key_type protocol);
	void put_driver_to_cache(driver_cache_type::key_type protocol, driver_cache_type::value_type driver);

private: // connection cache

	SQL_Connection* get_connection_from_cache(connection_cache_type::key_type url);
	void put_connection_to_cache(connection_cache_type::key_type url, SQL_Connection* connection);
private:
	time_t prev_expiration_pass_time;

private: // for SQL_Connection

	/// caches connection
	void close_connection(connection_cache_type::key_type url, SQL_Connection* connection);

public: // Cache_manager

	override Value* get_status();
	override void maybe_expire_cache();

};

/// global
extern SQL_Driver_manager* SQL_driver_manager;

#endif
