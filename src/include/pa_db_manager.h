/** @file
	Parser: sql driver manager decl.
	global sql driver manager, must be thread-safe

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_db_manager.h,v 1.8 2001/11/05 11:46:24 paf Exp $
*/

#ifndef PA_DB_MANAGER_H
#define PA_DB_MANAGER_H

#include "pa_config_includes.h"
#include "pa_hash.h"
#include "pa_db_connection.h"
#include "pa_status_provider.h"

// defines

// forwards

/// sql driver manager
class DB_Manager : public Pooled, public Status_provider {
	friend class DB_Connection;
public:

	DB_Manager(Pool& apool);
	~DB_Manager();

	/** 
		connect to specified db_home, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	DB_Connection_ptr get_connection_ptr(const String& db_home, const String *source);

private: // connection cache, never expires

	DB_Connection *get_connection_from_cache(const String& db_home);
	void put_connection_to_cache(const String& db_home, DB_Connection& connection);
	void maybe_expire_connection_cache();
private:
	time_t prev_expiration_pass_time;

private:

	Hash connection_cache;

public: // Status_provider

	virtual Value& get_status(Pool& pool, const String *source);

};

/// global
extern DB_Manager *DB_manager;

#endif
