/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_stylesheet_manager.C,v 1.12 2002/02/08 07:27:49 paf Exp $
*/
#include "pa_config_includes.h"
#ifdef XML

#include "pa_stylesheet_manager.h"
#include "pa_stylesheet_connection.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_threads.h"
#include "pa_stack.h"
#include "pa_vhash.h"
#include "pa_vtable.h"

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

Stylesheet_manager::Stylesheet_manager(Pool& apool) : Cache_manager(apool),
	connection_cache(apool),
	prev_expiration_pass_time(0) {
	}
Stylesheet_manager::~Stylesheet_manager() {
	connection_cache.for_each(expire_connections, 
		reinterpret_cast<void *>(time(0)+1/*=in future=expire all*/));
}

Stylesheet_connection_ptr Stylesheet_manager::get_connection(const String& request_file_spec) {
	Pool& pool=request_file_spec.pool(); // request pool											   

	// first trying to get cached stylesheet
	Stylesheet_connection *connection=get_connection_from_cache(request_file_spec);
	if(!connection) {
		// then just construct it

		// make global_file_spec C-string on global pool
		const char *request_file_spec_cstr=request_file_spec.cstr(String::UL_FILE_SPEC);
		char *global_file_spec_cstr=(char *)malloc(strlen(request_file_spec_cstr)+1);
		strcpy(global_file_spec_cstr, request_file_spec_cstr);
		// make global_file_spec string on global pool
		String& global_file_spec=*new(this->pool()) String(this->pool(), global_file_spec_cstr);

		connection=new(this->pool()) Stylesheet_connection(this->pool(), global_file_spec);
	}
	// associate with services[request]  (deassociates at close)
	connection->set_services(&pool); 
	// return autoclosing object for it
	return Stylesheet_connection_ptr(connection);
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

void Stylesheet_manager::maybe_expire_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_CONNECTION_SECONDS) {
		connection_cache.for_each(expire_connections, 
			reinterpret_cast<void *>(now-EXPIRE_UNUSED_CONNECTION_SECONDS));

		prev_expiration_pass_time=now;
	}
}

static void add_connection_to_status_cache_table(Array::Item *value, void *info) {
	Stylesheet_connection& connection=*static_cast<Stylesheet_connection *>(value);
	Table& table=*static_cast<Table *>(info);

	if(connection.connected()) {
		Pool& pool=table.pool();
		Array& row=*new(pool) Array(pool);

		// file
		row+=&connection.file_spec();
		// time
		time_t time_stamp=connection.get_time_used();
		const char *unsafe_time_cstr=ctime(&time_stamp);
		int time_buf_size=strlen(unsafe_time_cstr);
		char *safe_time_buf=(char *)pool.malloc(time_buf_size);
		memcpy(safe_time_buf, unsafe_time_cstr, time_buf_size);
		row+=new(pool) String(pool, safe_time_buf, time_buf_size);

		table+=&row;
	}
}
static void add_connections_to_status_cache_table(const Hash::Key& key, Hash::Val *value, void *info) {
	Stack& stack=*static_cast<Stack *>(value);
	Array_iter iter(stack);
	for(int countdown=stack.top_index(); countdown-->=0; )
		add_connection_to_status_cache_table(iter.next(), info);
}
Value& Stylesheet_manager::get_status(Pool& pool, const String *source) {
	VHash& result=*new(pool) VHash(pool);

	// cache
	{
		Array& columns=*new(pool) Array(pool);
		columns+=new(pool) String(pool, "file");
		columns+=new(pool) String(pool, "time");
		Table& table=*new(pool) Table(pool, 0, &columns, connection_cache.size());

		connection_cache.for_each(add_connections_to_status_cache_table, &table);

		result.hash(source).put(*new(pool) String(pool, "cache"), new(pool) VTable(pool, &table));
	}

	return result;
}

#endif
