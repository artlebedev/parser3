/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_manager.C,v 1.1 2001/10/22 16:44:42 parser Exp $
*/

#include "pa_db_manager.h"
#include "ltdl.h"
#include "pa_DB_connection.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_threads.h"
#include "pa_stack.h"

// globals

DB_Manager *DB_manager;

// consts

const int EXPIRE_UNUSED_CONNECTION_SECONDS=60;
const int CHECK_EXPIRED_CONNECTIONS_SECONDS=EXPIRE_UNUSED_CONNECTION_SECONDS*2;


// DB_Manager

/// @test db_paniccall & co
DB_Manager::DB_Manager(Pool& pool) : Pooled(pool),
	connection_cache(pool),
	prev_expiration_pass_time(0) {

	memset(&dbenv, 0, sizeof(dbenv));
	check("db_appinit", db_appinit(
		0/*db_home*/,
		0/*db_config*/, 
		&dbenv, 
		DB_CREATE | DB_INIT_MPOOL | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_TXN));
}

DB_Manager::~DB_Manager() {
	check("db_appexit", db_appexit(&dbenv));
}


void DB_Manager::check(const char *operation, int error) {
	switch(error) {
	case 0: 
		// no error
		break; 
	
	default:
		throw Exception(0, 0, 
			0/*source*/, 
			"db %s error: %s (%d)", 
			operation, strerror(errno), errno);
	}
}

DB_Connection& DB_Manager::get_connection(const String& request_file_spec,
												   const String& request_origin) {
	Pool& pool=request_origin.pool(); // request pool											   

	// first trying to get cached connection
	DB_Connection *result=get_connection_from_cache(request_file_spec);
	if(result && !result->ping()) { // we have some cached connection, is it pingable?
		result->disconnect(); // kill unpingabe=dead connection
		result=0;
	}

	char *request_file_spec_cstr;
	if(result)
		request_file_spec_cstr=0; // calm, compiler
	else { // no cached connection
		// make global_file_spec C-string on global pool
		request_file_spec_cstr=request_file_spec.cstr(String::UL_AS_IS);
		char *global_file_spec_cstr=(char *)malloc(strlen(request_file_spec_cstr)+1);
		strcpy(global_file_spec_cstr, request_file_spec_cstr);
		// make global_file_spec string on global pool
		String& global_file_spec=*new(this->pool()) String(this->pool(), global_file_spec_cstr);
		
		// allocate in global pool 
		// associate with services[request]
		// NOTE: never freed up!
		result=new(this->pool()) DB_Connection(this->pool(), global_file_spec, dbenv);
	}

	// associate with services[request]  (deassociates at close)
	result->set_services(&pool); 
	// if not connected yet, do that now, when result has services
	if(!result->connected())
		result->connect();
	// return it
	return *result;
}

void DB_Manager::close_connection(const String& file_spec, 
										  DB_Connection& connection) {
	// deassociate from services[request]
	connection.set_services(0);
	put_connection_to_cache(file_spec, connection);
}


// connection cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
DB_Connection *DB_Manager::get_connection_from_cache(const String& file_spec) { 
	SYNCHRONIZED;

	maybe_expire_connection_cache();

	if(Stack *connections=static_cast<Stack *>(connection_cache.get(file_spec)))
		while(connections->top_index()>=0) { // there are cached connections to that 'file_spec'
			DB_Connection *result=static_cast<DB_Connection *>(connections->pop());
			if(result->connected()) // not expired?
				return result;
		}

	return 0;
}

void DB_Manager::put_connection_to_cache(const String& file_spec, 
												 DB_Connection& connection) { 
	SYNCHRONIZED;

	Stack *connections=static_cast<Stack *>(connection_cache.get(file_spec));
	if(!connections) { // there are no cached connections to that 'file_spec' yet?
		connections=NEW Stack(pool()); // NOTE: never freed up!
		connection_cache.put(file_spec, connections);
	}	
	connections->push(&connection);
}

static void expire_connection(Array::Item *value, void *info) {
	DB_Connection& connection=*static_cast<DB_Connection *>(value);
	time_t older_dies=reinterpret_cast<time_t>(info);

	if(connection.connected() && connection.expired(older_dies))
		connection.disconnect();
}
static void expire_connections(const Hash::Key& key, Hash::Val *value, void *info) {
	Stack& stack=*static_cast<Stack *>(value);
	for(int i=0; i<=stack.top_index(); i++)
		expire_connection(stack.get(i), info);
}
void DB_Manager::maybe_expire_connection_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_CONNECTIONS_SECONDS) {
		connection_cache.for_each(expire_connections, 
			reinterpret_cast<void *>(now-EXPIRE_UNUSED_CONNECTION_SECONDS));

		prev_expiration_pass_time=now;
	}
}
