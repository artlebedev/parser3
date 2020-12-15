/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

#include "pa_stylesheet_manager.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_threads.h"
#include "pa_stack.h"
#include "pa_vhash.h"
#include "pa_vtable.h"

volatile const char * IDENT_PA_STYLESHEET_MANAGER_C="$Id: pa_stylesheet_manager.C,v 1.34 2020/12/15 17:10:37 moko Exp $" IDENT_PA_STYLESHEET_MANAGER_H;

// globals

Stylesheet_manager* stylesheet_manager=0;

// consts

const time_t EXPIRE_UNUSED_CONNECTION_SECONDS=5*60;
const time_t CHECK_EXPIRED_CONNECTION_SECONDS=EXPIRE_UNUSED_CONNECTION_SECONDS*2;

// helpers

static void expire_connection(Stylesheet_connection& connection, time_t older_dies) {
	if(connection.connected() && connection.expired(older_dies))
		connection.disconnect();
}
static void expire_connections(Stylesheet_manager::connection_cache_type::key_type /*key*/, Stylesheet_manager::connection_cache_type::value_type stack, time_t older_dies) {
	for(size_t i=0; i<stack->top_index(); i++)
		expire_connection(*stack->get(i), older_dies);
}

// Stylesheet_connection methods

void Stylesheet_connection::close() {
	stylesheet_manager->close_connection(ffile_spec, *this);
}

// Stylesheet_manager methods

Stylesheet_manager::Stylesheet_manager(): prev_expiration_pass_time(0) {
}

Stylesheet_manager::~Stylesheet_manager() {
	connection_cache.for_each<time_t>(expire_connections, time(0)+(time_t)10/*=in future=expire all*/);
}

Stylesheet_connection* Stylesheet_manager::get_connection(String::Body file_spec) {
	Stylesheet_connection* result=get_connection_from_cache(file_spec);
	return result ? result : new Stylesheet_connection(file_spec);
}

void Stylesheet_manager::close_connection(String::Body file_spec, Stylesheet_connection& connection) {
	put_connection_to_cache(file_spec, connection);
}

// stylesheet cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
Stylesheet_connection* Stylesheet_manager::get_connection_from_cache(String::Body file_spec) { 
	SYNCHRONIZED;

	if(connection_cache_type::value_type connections=connection_cache.get(file_spec))
		while(!connections->is_empty()) { // there are cached stylesheets to that 'file_spec'
			Stylesheet_connection* result=connections->pop();
			if(result->connected()) // not expired?
				return result;
		}

	return 0;
}

void Stylesheet_manager::put_connection_to_cache(String::Body file_spec, Stylesheet_connection& connection) {
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
		connection_cache.for_each<time_t>(expire_connections, now-EXPIRE_UNUSED_CONNECTION_SECONDS);

		prev_expiration_pass_time=now;
	}
}

static void add_connection_to_status_cache_table(Stylesheet_connection& connection, Table* table) {
	if(connection.connected()) {
		ArrayString& row=*new ArrayString;

		// file
		row+=new String(connection.file_spec(), String::L_AS_IS);
		// time
		time_t time_used=connection.get_time_used();
		row+=new String(pa_strdup(ctime(&time_used)));

		*table+=&row;
	}
}
static void add_connections_to_status_cache_table(Stylesheet_manager::connection_cache_type::key_type /*key*/, Stylesheet_manager::connection_cache_type::value_type stack, Table* table) {
	for(Array_iterator<Stylesheet_connection*> i(*stack); i.has_next(); )
		add_connection_to_status_cache_table(*i.next(), table);
}

Value* Stylesheet_manager::get_status() {
	VHash* result=new VHash;
	// cache
	{
		ArrayString& columns=*new ArrayString;
		columns+=new String("file");
		columns+=new String("time");
		Table& table=*new Table(&columns, connection_cache.count());

		connection_cache.for_each<Table*>(add_connections_to_status_cache_table, &table);

		result->get_hash()->put(*new String("cache"), new VTable(&table));
	}
	return result;
}

#endif
