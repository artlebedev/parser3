/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_db_manager.C,v 1.13 2001/11/05 14:19:55 paf Exp $
*/

#include "pa_config_includes.h"
#ifdef DB2

#include "pa_db_manager.h"
#include "pa_db_connection.h"
#include "pa_exception.h"
#include "pa_threads.h"
#include "pa_stack.h"
#include "pa_vhash.h"

// globals

DB_Manager *DB_manager;

// consts

const int EXPIRE_UNUSED_CONNECTION_SECONDS=60;
const int CHECK_EXPIRED_CONNECTIONS_SECONDS=EXPIRE_UNUSED_CONNECTION_SECONDS*2;

// callbacks

static void expire_connection(const Hash::Key& key, Hash::Val *& value, void *info) {
	if(DB_Connection *connection=static_cast<DB_Connection *>(value)) {
		time_t older_dies=reinterpret_cast<time_t>(info);

		if(connection->expired(older_dies)) {
			connection->~DB_Connection();  value=0;
		}
	}
}

// DB_Manager

DB_Manager::DB_Manager(Pool& apool) : Pooled(apool),
	connection_cache(apool),
	prev_expiration_pass_time(0) {

	status_providers->put(*NEW String(pool(), "db"), this);
}

DB_Manager::~DB_Manager() {
	// close connections
	connection_cache.for_each(expire_connection, 
		reinterpret_cast<void *>(0/* =in the past = expire[close] all*/));
}

/// @test subpools mechanizm. one connection, one subpool. ~connection destructs it
DB_Connection_ptr DB_Manager::get_connection_ptr(const String& request_db_home,
												   const String *source) {
	if(request_db_home.size()==0)
		throw Exception(0, 0,
			source,
			"empty DB_HOME specified");

	// first trying to get cached connection
	DB_Connection *result=get_connection_from_cache(request_db_home);
	if(!result) { // no cached connection
		// make global_db_home C-string on global pool
		const char *request_db_home_cstr=request_db_home.cstr();
		char *global_db_home_cstr=(char *)malloc(strlen(request_db_home_cstr)+1);
		strcpy(global_db_home_cstr, request_db_home_cstr);
		// make global_db_home string on global pool
		String& global_db_home=*new(this->pool()) String(this->pool(), global_db_home_cstr);
		
		// allocate in global pool 
		// NOTE: never freed up!
		result=new(this->pool()) DB_Connection(this->pool(), global_db_home);
		// cache it
		put_connection_to_cache(global_db_home, *result);
	}

	// return auto-it
	return DB_Connection_ptr(result);
}

// connection cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
DB_Connection *DB_Manager::get_connection_from_cache(const String& db_home) { 
	SYNCHRONIZED;

	maybe_expire_connection_cache();

	return static_cast<DB_Connection *>(connection_cache.get(db_home));
}

void DB_Manager::put_connection_to_cache(const String& db_home, 
												 DB_Connection& connection) { 
	SYNCHRONIZED;

	connection_cache.put(db_home, &connection);
}

void DB_Manager::maybe_expire_connection_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_CONNECTIONS_SECONDS) {
		connection_cache.for_each(expire_connection, 
			reinterpret_cast<void *>(now-EXPIRE_UNUSED_CONNECTION_SECONDS));

		prev_expiration_pass_time=now;
	}
}

static void add_connection_to_status_status_cache(const Hash::Key& key, Hash::Val *value, void *info) {
	DB_Connection& connection=*static_cast<DB_Connection *>(value);
	VHash& status_cache=*static_cast<VHash *>(info);
	Pool& pool=status_cache.pool();
	
	// file => tables table
	status_cache.hash(0).put(connection.db_home(), &connection.get_status(pool, 0));
}
Value& DB_Manager::get_status(Pool& pool, const String *source) {
	VHash& result=*new(pool) VHash(pool);
	
	// db_homes
	{
		VHash& status_cache=*new(pool) VHash(pool);
		connection_cache.for_each(add_connection_to_status_status_cache, &status_cache);

		result.hash(source).put(*new(pool) String(pool, "cache"), &status_cache);
	}

	return result;
}

#endif
