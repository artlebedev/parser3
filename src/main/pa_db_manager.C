/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_manager.C,v 1.6 2001/10/25 13:17:53 paf Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_db_manager.h"
#include "pa_db_connection.h"
#include "pa_exception.h"
#include "pa_threads.h"
#include "pa_stack.h"

// globals

DB_Manager *DB_manager;

// consts

const int EXPIRE_UNUSED_CONNECTION_SECONDS=60;
const int CHECK_EXPIRED_CONNECTIONS_SECONDS=EXPIRE_UNUSED_CONNECTION_SECONDS*2;

// callbacks

static void expire_connection(Array::Item *value, void *info) {
	DB_Connection& connection=*static_cast<DB_Connection *>(value);
	time_t older_dies=reinterpret_cast<time_t>(info);

	if(connection.ping() && connection.expired(older_dies))
		connection.disconnect();
}
static void expire_connections(const Hash::Key& key, Hash::Val *value, void *info) {
	Stack& stack=*static_cast<Stack *>(value);
	for(int i=0; i<=stack.top_index(); i++)
		expire_connection(stack.get(i), info);
}

// DB_Manager

DB_Manager::DB_Manager(Pool& pool) : Pooled(pool),
	connection_cache(pool),
	prev_expiration_pass_time(0) {

}

DB_Manager::~DB_Manager() {
	// close connections
	connection_cache.for_each(expire_connections, 
		reinterpret_cast<void *>(0/* =in the past = expire[close] all*/));
}


DB_Connection& DB_Manager::get_connection(const String& request_db_home,
												   const String& request_origin) {
	if(request_db_home.size()==0)
		throw Exception(0, 0,
			&request_origin,
			"empty DB_HOME specified");

	Pool& pool=request_origin.pool(); // request pool											   

	// first trying to get cached connection
	DB_Connection *result=get_connection_from_cache(request_db_home);

	char *request_db_home_cstr;
	if(result)
		request_db_home_cstr=0; // calm, compiler
	else { // no cached connection
		// make global_db_home C-string on global pool
		request_db_home_cstr=request_db_home.cstr(String::UL_AS_IS);
		char *global_db_home_cstr=(char *)malloc(strlen(request_db_home_cstr)+1);
		strcpy(global_db_home_cstr, request_db_home_cstr);
		// make global_db_home string on global pool
		String& global_db_home=*new(this->pool()) String(this->pool(), global_db_home_cstr);
		
		// allocate in global pool 
		// associate with services[request]
		// NOTE: never freed up!
		result=new(this->pool()) DB_Connection(this->pool(), global_db_home);
	}

	// associate with services[request]  (deassociates at close)
	result->set_services(&pool); 
	// if not connected yet, do that now, when result has services
	if(!result->connected())
		result->connect();
	// return it
	return *result;
}

void DB_Manager::close_connection(const String& db_home, 
										  DB_Connection& connection) {
	// deassociate from services[request]
	connection.set_services(0);
	put_connection_to_cache(db_home, connection);
}


// connection cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
DB_Connection *DB_Manager::get_connection_from_cache(const String& db_home) { 
	SYNCHRONIZED;

	if(Stack *connections=static_cast<Stack *>(connection_cache.get(db_home)))
		while(connections->top_index()>=0) { // there are cached connections to that 'db_home'
			DB_Connection *result=static_cast<DB_Connection *>(connections->pop());
			if(result->connected()) // not expired?
				return result;
		}

	return 0;
}

void DB_Manager::put_connection_to_cache(const String& db_home, 
												 DB_Connection& connection) { 
	SYNCHRONIZED;

	Stack *connections=static_cast<Stack *>(connection_cache.get(db_home));
	if(!connections) { // there are no cached connections to that 'db_home' yet?
		connections=NEW Stack(pool()); // NOTE: never freed up!
		connection_cache.put(db_home, connections);
	}	
	connections->push(&connection);
}

void DB_Manager::maybe_expire_connection_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_CONNECTIONS_SECONDS) {
		connection_cache.for_each(expire_connections, 
			reinterpret_cast<void *>(now-EXPIRE_UNUSED_CONNECTION_SECONDS));

		prev_expiration_pass_time=now;
	}
}

#endif
