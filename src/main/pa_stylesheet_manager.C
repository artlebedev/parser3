/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

static const char * const IDENT_STYLESHEET_MANAGER_C="$Date: 2003/11/20 16:34:27 $";

#include "pa_stylesheet_manager.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_threads.h"
#include "pa_stack.h"
#include "pa_vhash.h"
#include "pa_vtable.h"

// globals

Stylesheet_manager stylesheet_manager;

// consts

const time_t SECOND=1;
const time_t EXPIRE_UNUSED_CONNECTION_SECONDS=5*60;
const time_t CHECK_EXPIRED_CONNECTION_SECONDS=EXPIRE_UNUSED_CONNECTION_SECONDS*2;

// helpers

static void expire_connection(Stylesheet_connection& connection, time_t older_dies) {
	if(connection.connected() && connection.expired(older_dies))
		connection.disconnect();
}
static void expire_connections(
			       Stylesheet_manager::connection_cache_type::key_type /*key*/, 
			       Stylesheet_manager::connection_cache_type::value_type stack, 
			       time_t older_dies) {
	for(size_t i=0; i<stack->top_index(); i++)
		expire_connection(*stack->get(i), older_dies);
}

// Stylesheet_connection methods

void Stylesheet_connection::close() {
	stylesheet_manager.close_connection(ffile_spec, *this);
}

// Stylesheet_manager methods

Stylesheet_manager::Stylesheet_manager(): prev_expiration_pass_time(0) {

	cache_managers.put(String::Body("stylesheet"), this);
}

Stylesheet_manager::~Stylesheet_manager() {
	connection_cache.for_each(expire_connections, time(0)+SECOND/*=in future=expire all*/);
}

Stylesheet_connection_ptr Stylesheet_manager::get_connection(const String& file_spec) {
	Stylesheet_connection* result=get_connection_from_cache(file_spec);
	if(!result)
		result=new Stylesheet_connection(file_spec);
	return Stylesheet_connection_ptr(result);
}

void Stylesheet_manager::close_connection(const String& file_spec, 
										  Stylesheet_connection& connection) {
	put_connection_to_cache(file_spec, connection);
}

// stylesheet cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
Stylesheet_connection* Stylesheet_manager::get_connection_from_cache(const String& file_spec) { 
	SYNCHRONIZED;

	if(connection_cache_type::value_type connections=connection_cache.get(file_spec))
		while(!connections->is_empty()) { // there are cached stylesheets to that 'file_spec'
			Stylesheet_connection* result=connections->pop();
			if(result->connected()) // not expired?
				return result;
		}

	return 0;
}

void Stylesheet_manager::put_connection_to_cache(const String& file_spec, 
						 Stylesheet_connection& connection) { 
	SYNCHRONIZED;

	connection_cache_type::value_type connections=connection_cache.get(file_spec);
	if(!connections) { // there are no cached stylesheets to that 'file_spec' yet?
		connections=new connection_cache_value_type;
		connection_cache.put(file_spec, connections);
	}	
	connections->push(&connection);
}

void Stylesheet_manager::maybe_expire_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_CONNECTION_SECONDS) {
		connection_cache.for_each(expire_connections, now-EXPIRE_UNUSED_CONNECTION_SECONDS);

		prev_expiration_pass_time=now;
	}
}
/*
static void add_connection_to_status_cache_table(Array::Item *value, void *info) {
	Stylesheet_connection& connection=*static_cast<Stylesheet_connection *>(value);
	Table& table=*static_cast<Table *>(info);

	if(connection.connected()) {
		=table.pool();
		Array& row=*new Array();

		// file
		row+=&connection.file_spec();
		// time
		time_t time_stamp=connection.get_time_used();
		row+=new String(pool.copy(ctime(&time_stamp)));

		table+=&row;
	}
}
static void add_connections_to_status_cache_table(const Hash::Key& key, Hash::Val *value, void *info) {
	Stack& stack=*static_cast<Stack *>(value);
	Array_iter iter(stack);
	for(int countdown=stack.top_index(); countdown-->=0; )
		add_connection_to_status_cache_table(iter.next(), info);
}
*/
Value* Stylesheet_manager::get_status() {
	VHash* result=new VHash;
/*
	// cache
	{
		Array& columns=*new Array();
		columns+=new String("file");
		columns+=new String("time");
		Table& table=*new Table(0, &columns, connection_cache.length());

		connection_cache.for_each(add_connections_to_status_cache_table, &table);

		result.hash(source).put(*new String("cache"), new VTable(&table));
	}
*/
	return result;
}

#endif
