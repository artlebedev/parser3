/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_SQL_DRIVER_MANAGER_C="$Date: 2005/08/09 08:14:52 $";

#include "pa_sql_driver_manager.h"
#include "ltdl.h"
#include "pa_threads.h"
#include "pa_sql_connection.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_vhash.h"
#include "pa_vtable.h"
#include "pa_charsets.h"

// globals

SQL_Driver_manager* SQL_driver_manager=0;

// consts

const time_t EXPIRE_UNUSED_CONNECTION_SECONDS=60;
const time_t CHECK_EXPIRED_CONNECTIONS_SECONDS=EXPIRE_UNUSED_CONNECTION_SECONDS*2;

// helpers

const String& SQL_Driver_services_impl::url_without_login() const {
	String& result=*new String;
	result << furl->mid(0, furl->pos(':')) << "://****";

	size_t at_pos=0;
	size_t new_at_pos;
	while( (new_at_pos=furl->pos('@', at_pos+1))!=STRING_NOT_FOUND)
		at_pos=new_at_pos;
	if(at_pos)
		result << furl->mid(at_pos, furl->length());

	return result;
}

void SQL_Driver_services_impl::transcode(const char* src, size_t src_length,
	const char*& dst, size_t& dst_length,
	const char* charset_from_name,
	const char* charset_to_name
	) 
{
	try {
		// to speed up sql transcode [lots of charsets::get-s -- twice for each cell]
		// without complicating sql driver API
		// we need to cache couple of name->charset pairs
		// assuming pointers to names are are NOT to local vars 

		struct cache_record {
			const char* name;
			Charset* object;
		} cache[2]={{0,0}, {0,0}};

		Charset* charset_from_object;
		Charset* charset_to_object;
		if(charset_from_name==cache[0].name) {
			charset_from_object=cache[0].object;
		} else {
			cache[0].name=charset_from_name;
			cache[0].object=charset_from_object=&charsets.get(charset_from_name);
		}
		if(charset_to_name==cache[1].name) {
			charset_to_object=cache[1].object;
		} else {
			cache[1].name=charset_to_name;
			cache[1].object=charset_to_object=&charsets.get(charset_to_name);
		}

		String::C result=Charset::transcode(String::C(src, src_length), 
			*charset_from_object,
			*charset_to_object);
		dst=result.str;
		dst_length=result.length;
	} catch(const Exception& e) {
		_throw(SQL_Error(e.type(), e.comment()));
	} catch(...) {
		_throw(SQL_Error(0, "unknown error while transcoding in sql driver"));
	}
}

// helpers

static void expire_connection(SQL_Connection& connection, time_t older_dies) {
	if(connection.connected() && connection.expired(older_dies))
		connection.disconnect();
}
static void expire_connections(SQL_Driver_manager::connection_cache_type::key_type /*key*/, 
			       SQL_Driver_manager::connection_cache_type::value_type stack, 
			       time_t older_dies) {
	for(size_t i=0; i<stack->top_index(); i++)
		expire_connection(*stack->get(i), older_dies);
}

// SQL_Driver_manager

SQL_Driver_manager::SQL_Driver_manager(): 
	is_dlinited(false), prev_expiration_pass_time(0) {
}

SQL_Driver_manager::~SQL_Driver_manager() {
	connection_cache.for_each(expire_connections, time(0)+(time_t)10/*=in future=expire all*/);

	if(is_dlinited)
		lt_dlexit();
}

/// @param aurl protocol://[driver-dependent]
SQL_Connection* SQL_Driver_manager::get_connection(const String& aurl,
						   Table *protocol2driver_and_client,
						   const char* arequest_charset) {
	// we have table for locating protocol's library
	if(!protocol2driver_and_client)
		throw Exception("parser.runtime",
			&aurl,
			"$"MAIN_SQL_NAME":"MAIN_SQL_DRIVERS_NAME" table must be defined");

	// first trying to get cached connection
	SQL_Connection* connection=get_connection_from_cache(aurl);
	if(connection) {
		connection->set_url();
		if(!connection->ping()) { // we have some cached connection, is it pingable?
			connection->disconnect(); // kill unpingabe=dead connection
			connection=0;
		}
	}

	char *url_cstr;
	if(connection)
		url_cstr=0; // calm, compiler
	else { // no cached connection or it were unpingabe: connect/reconnect
		url_cstr=aurl.cstrm();
		if(!strstr(url_cstr, "://"))
			throw Exception("parser.runtime",
				aurl.length()?&aurl:0,
				"connection string must start with protocol://");


		char *protocol_cstr=lsplit(&url_cstr, ':');
		// skip "//" after ':'
		while(*url_cstr=='/')
			url_cstr++;
		const String& protocol=*new String(protocol_cstr);

		SQL_Driver *driver;
		// first trying to get cached driver
		if(!(driver=get_driver_from_cache(protocol))) {
			// no cached
			const String* library=0;
			const String* dlopen_file_spec=0;
			Table::Action_options options;
			if(protocol2driver_and_client->locate(0, protocol, options)) {
				if(!(library=protocol2driver_and_client->item(1)) || library->length()==0)
					throw Exception("parser.runtime",
						0,
						"driver library column for protocol '%s' is empty", 
							protocol_cstr);
				dlopen_file_spec=protocol2driver_and_client->item(2);
			} else
				throw Exception("parser.runtime",
					&aurl,
					"undefined protocol '%s'", 
						protocol_cstr);

			if(!is_dlinited) {
				if(lt_dlinit())
					throw Exception(0,
						library,
						"prepare to dynamic loading failed, %s", lt_dlerror());

				is_dlinited=true;
			}

			const char* filename=library->cstr(String::L_FILE_SPEC);
			lt_dlhandle handle=lt_dlopen(filename);
			if (!handle) {
				const char* error=lt_dlerror();
				throw Exception(0,
					library,
					error?error:"can not open the module");
			}

			SQL_Driver_create_func create=(SQL_Driver_create_func)lt_dlsym(handle, 
				SQL_DRIVER_CREATE_NAME);
			if(!create)
				throw Exception(0,
					library,
					"function '"SQL_DRIVER_CREATE_NAME"' was not found");

			// create library-driver!
			driver=(*create)();

			// validate driver api version
			int driver_api_version=driver->api_version();
			if(driver_api_version!=SQL_DRIVER_API_VERSION)
				throw Exception(0,
					library,
					"driver implements API version 0x%04X not equal to 0x%04X",
						driver_api_version, SQL_DRIVER_API_VERSION);

			// initialise by connecting to sql client dynamic link library
			char* dlopen_file_spec_cstr=
				dlopen_file_spec && dlopen_file_spec->length()?
				dlopen_file_spec->cstrm(String::L_AS_IS):0;
			if(const char* error=driver->initialize(dlopen_file_spec_cstr))
				throw Exception(0,
					library,
					"driver failed to initialize client library '%s', %s",
						dlopen_file_spec_cstr?dlopen_file_spec_cstr:"unspecifed",
						error);

			// cache it
			put_driver_to_cache(protocol, driver);
		}
	
		connection=new SQL_Connection(aurl, *driver, arequest_charset);
		// associate with pool[request]  (deassociates at close)
		connection->set_url(); 
	}

	// if not connected yet, do that now, when connection has services
	if(!connection->connected())
		connection->connect(url_cstr);
	// return autoclosing object for it
	return connection;
}

void SQL_Driver_manager::close_connection(connection_cache_type::key_type url, 
					  SQL_Connection* connection) {
	put_connection_to_cache(url, connection);
}


// driver cache

SQL_Driver *SQL_Driver_manager::get_driver_from_cache(driver_cache_type::key_type protocol) {
	SYNCHRONIZED;

	return driver_cache.get(protocol);
}

void SQL_Driver_manager::put_driver_to_cache(
											 driver_cache_type::key_type protocol, 
											 driver_cache_type::value_type driver) {
	SYNCHRONIZED;

	driver_cache.put(protocol, driver);
}

// connection cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
SQL_Connection* SQL_Driver_manager::get_connection_from_cache(connection_cache_type::key_type url) { 
	SYNCHRONIZED;

	if(connection_cache_type::value_type connections=connection_cache.get(url))
		while(!connections->is_empty()) { // there are cached connections to that 'url'
			SQL_Connection* result=connections->pop();
			if(result->connected()) // not expired?
				return result;
		}

	return 0;
}

void SQL_Driver_manager::put_connection_to_cache(
						 connection_cache_type::key_type url, 
						 SQL_Connection* connection) { 
	SYNCHRONIZED;

	connection_cache_type::value_type connections=connection_cache.get(url);
	if(!connections) { // there are no cached connections to that 'url' yet?
		connections=new connection_cache_element_base_type;
		connection_cache.put(url, connections);
	}	
	connections->push(connection);
}

void SQL_Driver_manager::maybe_expire_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_CONNECTIONS_SECONDS) {
		connection_cache.for_each(expire_connections, time_t(now-EXPIRE_UNUSED_CONNECTION_SECONDS));

		prev_expiration_pass_time=now;
	}
}

static void add_connection_to_status_cache_table(SQL_Connection& connection, Table* table) {
	if(connection.connected()) {
		ArrayString& row=*new ArrayString;

		// url
		row+=&connection.services().url_without_login();
		// time
		time_t time_used=connection.get_time_used();
		row+=new String(pa_strdup(ctime(&time_used)));

		*table+=&row;
	}
}
static void add_connections_to_status_cache_table(
	SQL_Driver_manager::connection_cache_type::key_type /*key*/, 
	SQL_Driver_manager::connection_cache_type::value_type stack, Table* table) 
{
	for(Array_iterator<SQL_Connection*> i(*stack); i.has_next(); )
		add_connection_to_status_cache_table(*i.next(), table);
}

Value* SQL_Driver_manager::get_status() {
	Value* result=new VHash;

	// cache
	{
		ArrayString& columns=*new ArrayString;
		columns+=new String("url");
		columns+=new String("time");
		Table& table=*new Table(&columns, connection_cache.count());

		connection_cache.for_each(add_connections_to_status_cache_table, &table);

		result->get_hash()->put(*new String("cache"), new VTable(&table));
	}

	return result;
}
