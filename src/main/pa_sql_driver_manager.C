/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver_manager.C,v 1.42 2001/10/22 16:44:42 parser Exp $
*/

#include "pa_sql_driver_manager.h"
#include "ltdl.h"
#include "pa_sql_connection.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_threads.h"
#include "pa_stack.h"

// globals

SQL_Driver_manager *SQL_driver_manager;

// consts

const int EXPIRE_UNUSED_CONNECTION_SECONDS=60;
const int CHECK_EXPIRED_CONNECTIONS_SECONDS=EXPIRE_UNUSED_CONNECTION_SECONDS*2;


/// SQL_Driver_services Pooled implementation
class SQL_Driver_services_impl : public SQL_Driver_services, public Pooled {
public:
	SQL_Driver_services_impl(Pool& apool, const String& aurl) : Pooled(apool),
		furl(aurl) {
	}

	/// allocates some bytes on pool
	void *malloc(size_t size) { return Pooled::malloc(size); }
	/// allocates some bytes clearing them with zeros
	void *calloc(size_t size) { return Pooled::calloc(size); }

	/// throw exception
	void _throw(const char *comment) { 
		throw Exception(0, 0, 
			&furl, 
			comment); 
	}

private:
	const String& furl;
};

// helpers

static void expire_connection(Array::Item *value, void *info) {
	SQL_Connection& connection=*static_cast<SQL_Connection *>(value);
	time_t older_dies=reinterpret_cast<time_t>(info);

	if(connection.connected() && connection.expired(older_dies))
		connection.disconnect();
}
static void expire_connections(const Hash::Key& key, Hash::Val *value, void *info) {
	Stack& stack=*static_cast<Stack *>(value);
	for(int i=0; i<=stack.top_index(); i++)
		expire_connection(stack.get(i), info);
}

// SQL_Driver_manager

SQL_Driver_manager::SQL_Driver_manager(Pool& pool) : Pooled(pool),
		driver_cache(pool),
		connection_cache(pool),
		prev_expiration_pass_time(0) {
	
}

SQL_Driver_manager::~SQL_Driver_manager() {
	connection_cache.for_each(expire_connections, 
		reinterpret_cast<void *>((time_t)0/*=in past=expire all*/));
}

/// @param request_url protocol://[driver-dependent]
SQL_Connection& SQL_Driver_manager::get_connection(const String& request_url,
												   const String& request_origin,
												   Table *protocol2driver_and_client) {
	Pool& pool=request_origin.pool(); // request pool											   

	// we have table for locating protocol's library
	if(!protocol2driver_and_client)
		throw Exception(0, 0,
			&request_url,
			"$"MAIN_SQL_NAME":"MAIN_SQL_DRIVERS_NAME" table must be defined");

	// first trying to get cached connection
	SQL_Connection *result=get_connection_from_cache(request_url);
	if(result && !result->ping()) { // we have some cached connection, is it pingable?
		result->disconnect(); // kill unpingabe=dead connection
		result=0;
	}

	char *request_url_cstr;
	if(result)
		request_url_cstr=0; // calm, compiler
	else { // no cached connection or it were unpingabe: connect/reconnect
		int pos=request_url.pos("://", 3);
		if(pos<0)
			throw Exception(0, 0,
				request_url.size()?&request_url:&request_origin,
				"connection string must start with protocol://"); // NOTE: not THROW, but PTHROW

		// make global_url C-string on global pool
		request_url_cstr=request_url.cstr(String::UL_AS_IS);
		char *global_url_cstr=(char *)malloc(strlen(request_url_cstr)+1);
		strcpy(global_url_cstr, request_url_cstr);
		// make global_url string on global pool
		String& global_url=*new(this->pool()) String(this->pool(), global_url_cstr);
		
		char *request_protocol_cstr=lsplit(&request_url_cstr, ':');
		// skip "//" after ':'
		while(*request_url_cstr=='/')
			request_url_cstr++;
		// make global_protocol C-string on global pool
		char *global_protocol_cstr=(char *)malloc(strlen(request_protocol_cstr)+1);
		strcpy(global_protocol_cstr, request_protocol_cstr);
		// make global_protocol string on global pool
		String& global_protocol=*new(this->pool()) String(this->pool(), 
			global_protocol_cstr);

		SQL_Driver *driver;
		const String *dlopen_file_spec=0;
		// first trying to get cached driver
		if(!(driver=get_driver_from_cache(global_protocol))) {
			// no cached
			const String *library=0;
			if(protocol2driver_and_client->locate(0, global_protocol)) {
				if(!(library=protocol2driver_and_client->item(1)) || library->size()==0)
					throw Exception(0, 0,
						protocol2driver_and_client->origin_string(),
						"driver library column for protocol '%s' is empty", 
							request_protocol_cstr);
				dlopen_file_spec=protocol2driver_and_client->item(2);
			} else
				throw Exception(0, 0,
					&request_url,
					"undefined protocol '%s'", 
						request_protocol_cstr);

			if(lt_dlinit())
				throw Exception(0, 0,
					library,
					"prepare to dynamic loading failed, %s", lt_dlerror());

			const char *filename=library->cstr(String::UL_FILE_SPEC);
			lt_dlhandle handle=lt_dlopen(filename);
			if (!handle)
				throw Exception(0, 0,
					library,
					"can not open the module, %s", lt_dlerror());

			SQL_Driver_create_func create=(SQL_Driver_create_func)lt_dlsym(handle, 
				SQL_DRIVER_CREATE_NAME);
			if(!create)
				throw Exception(0, 0,
					library,
					"function '"SQL_DRIVER_CREATE_NAME"' was not found");

			// create library-driver!
			driver=(*create)();

			// validate driver api version
			int driver_api_version=driver->api_version();
			if(driver_api_version!=SQL_DRIVER_API_VERSION)
				throw Exception(0, 0,
					library,
					"driver implements API version 0x%04X not equal to 0x%04X",
						driver_api_version, SQL_DRIVER_API_VERSION);

			// initialise by connecting to sql client dynamic link library
			bool specified_dlopen_file_spec=dlopen_file_spec && dlopen_file_spec->size();
			const char *dlopen_file_spec_cstr=
				specified_dlopen_file_spec?
				dlopen_file_spec->cstr(String::UL_FILE_SPEC):0;
			if(const char *error=driver->initialize(
				dlopen_file_spec_cstr))
				throw Exception(0, 0,
					library,
					"driver failed to initialize client library '%s', %s",
						specified_dlopen_file_spec?dlopen_file_spec_cstr:"unspecifed", 
						error);

			// cache it
			put_driver_to_cache(global_protocol, *driver);
		}
	
		// allocate in global pool 
		// associate with services[request]
		// NOTE: never freed up!
		result=new(this->pool()) SQL_Connection(this->pool(), global_url, *driver);
	}

	// associate with services[request]  (deassociates at close)
	result->set_services(new(pool) SQL_Driver_services_impl(pool, request_url)); 
	// if not connected yet, do that now, when result has services
	if(!result->connected())
		result->connect(request_url_cstr);
	// return it
	return *result;
}

void SQL_Driver_manager::close_connection(const String& url, 
										  SQL_Connection& connection) {
	// deassociate from services[request]
	connection.set_services(0);
	put_connection_to_cache(url, connection);
}


// driver cache

SQL_Driver *SQL_Driver_manager::get_driver_from_cache(const String& protocol) {
	SYNCHRONIZED;

	return static_cast<SQL_Driver *>(driver_cache.get(protocol));
}

void SQL_Driver_manager::put_driver_to_cache(const String& protocol, 
											 SQL_Driver& driver) {
	SYNCHRONIZED;

	driver_cache.put(protocol, &driver);
}

// connection cache
/// @todo get rid of memory spending Stack [zeros deep inside got accumulated]
SQL_Connection *SQL_Driver_manager::get_connection_from_cache(const String& url) { 
	SYNCHRONIZED;

	maybe_expire_connection_cache();

	if(Stack *connections=static_cast<Stack *>(connection_cache.get(url)))
		while(connections->top_index()>=0) { // there are cached connections to that 'url'
			SQL_Connection *result=static_cast<SQL_Connection *>(connections->pop());
			if(result->connected()) // not expired?
				return result;
		}

	return 0;
}

void SQL_Driver_manager::put_connection_to_cache(const String& url, 
												 SQL_Connection& connection) { 
	SYNCHRONIZED;

	Stack *connections=static_cast<Stack *>(connection_cache.get(url));
	if(!connections) { // there are no cached connections to that 'url' yet?
		connections=NEW Stack(pool()); // NOTE: never freed up!
		connection_cache.put(url, connections);
	}	
	connections->push(&connection);
}

void SQL_Driver_manager::maybe_expire_connection_cache() {
	time_t now=time(0);

	if(prev_expiration_pass_time<now-CHECK_EXPIRED_CONNECTIONS_SECONDS) {
		connection_cache.for_each(expire_connections, 
			reinterpret_cast<void *>(now-EXPIRE_UNUSED_CONNECTION_SECONDS));

		prev_expiration_pass_time=now;
	}
}
