/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_stylesheet_manager.C,v 1.1 2001/10/22 16:44:42 parser Exp $
*/
#include "pa_config_includes.h"
#ifdef XML

#include "pa_stylesheet_manager.h"
#include "pa_stylesheet_connection.h"
#include "ltdl.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_threads.h"
#include "pa_stack.h"

// globals

Stylesheet_manager *stylesheet_manager;

// consts

const int EXPIRE_UNUSED_CONNECTION_SECONDS=5*60;
const int CHECK_EXPIRED_CONNECTION_SECONDS=EXPIRE_UNUSED_CONNECTION_SECONDS*2;

// helpers

static void expire_connection(Array::Item *value, void *info) {
	Stylesheet_connection& connection=*static_cast<Stylesheet_connection *>(value);
	time_t older_dies=reinterpret_cast<time_t>(info);

	if(connection.connected() && connection.expired(older_dies))
		connection.disconnect();
}
static void expire_connections(const Hash::Key& key, Hash::Val *value, void *info) {
	Stack& stack=*static_cast<Stack *>(value);
	for(int i=0; i<=stack.top_index(); i++)
		expire_connection(stack.get(i), info);
}

// Stylesheet_manager

Stylesheet_manager::Stylesheet_manager(Pool& pool) : Pooled(pool),
	connection_cache(pool),
	prev_expiration_pass_time(0) {
}
Stylesheet_manager::~Stylesheet_manager() {
	connection_cache.for_each(expire_connections, 
		reinterpret_cast<void *>((time_t)0/*=in past=expire all*/));
}

Stylesheet_connection& Stylesheet_manager::get_connection(const String& request_file_spec) {
	Pool& pool=request_file_spec.pool(); // request pool											   

	// first trying to get cached stylesheet
	Stylesheet_connection *result=get_connection_from_cache(request_file_spec);
	if(!result) {
		// then just construct it

		// make global_file_spec C-string on global pool
		const char *request_file_spec_cstr=request_file_spec.cstr(String::UL_FILE_SPEC);
		char *global_file_spec_cstr=(char *)malloc(strlen(request_file_spec_cstr)+1);
		strcpy(global_file_spec_cstr, request_file_spec_cstr);
		// make global_file_spec string on global pool
		String& global_file_spec=*new(this->pool()) String(this->pool(), global_file_spec_cstr);

		result=new(this->pool()) Stylesheet_connection(this->pool(), global_file_spec);
	}
	// associate with services[request]  (deassociates at close)
	result->set_services(&pool); 
	// return it
	return *result;
}

void Stylesheet_manager::close_connection(const String& file_spec, 
										  Stylesheet_connection& connection) {
	// deassociate from services[request]
	connection.set_services(0);
	put_connection_to_cache(file_spec, connection);
}


// stylesheet cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
Stylesheet_connection *Stylesheet_manager::get_connection_from_cache(const String& file_spec) { 
	SYNCHRONIZED;

	maybe_expire_connection_cache();

	if(Stack *connections=static_cast<Stack *>(connection_cache.get(file_spec)))
		while(connections->top_index()>=0) { // there are cached stylesheets to that 'file_spec'
			Stylesheet_connection *result=static_cast<Stylesheet_connection *>(connections->pop());
			if(result->connected()) // not expired?
				return result;
		}

	return 0;
}

void Stylesheet_manager::put_connection_to_cache(const String& file_spec, 
												 Stylesheet_connection& connection) { 
	SYNCHRONIZED;

	Stack *connections=static_cast<Stack *>(connection_cache.get(file_spec));
	if(!connections) { // there are no cached stylesheets to that 'file_spec' yet?
		connections=NEW Stack(pool()); // NOTE: never freed up!
		connection_cache.put(file_spec, connections);
	}	
	connections->push(&connection);
}

void Stylesheet_manager::maybe_expire_connection_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_CONNECTION_SECONDS) {
		connection_cache.for_each(expire_connections, 
			reinterpret_cast<void *>(now-EXPIRE_UNUSED_CONNECTION_SECONDS));

		prev_expiration_pass_time=now;
	}
}
#endif
