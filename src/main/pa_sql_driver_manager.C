/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver_manager.C,v 1.3 2001/04/05 08:09:24 paf Exp $
*/

#include "pa_config_includes.h"
#include "ltdl.h"
#include "pa_sql_driver_manager.h"
#include "pa_sql_connection.h"
#include "pa_exception.h"
#include "pa_common.h"

// globals

SQL_Driver_manager *SQL_driver_manager;

// consts

const int MAX_PROTOCOL=20;
const char *LIBRARY_CREATE_FUNC_NAME="create";


/// Services_for_SQL_driver implementation
class Services_for_SQL_driver_impl : public Services_for_SQL_driver, public Pooled {
public:
	Services_for_SQL_driver_impl(Pool& apool, const String& aurl) : Pooled(apool),
		furl(aurl) {
	}

	/// allocates some bytes on pool
	void *malloc(size_t size) { return Pooled::malloc(size); }
	/// allocates some bytes clearing them with zeros
	void *calloc(size_t size) { return Pooled::calloc(size); }
	/// throw exception
	void _throw(const char *comment) { 
		THROW(0, 0, 
			&furl, 
			comment); 
	}

private:
	const String& furl;
};

// SQL_Driver_manager

// url:
//   protocol://user:pass@host:port/database
//              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ this is driver-dependent
SQL_Connection& SQL_Driver_manager::get_connection(const String& url, 
												   Table *protocol2library) {
	SYNCHRONIZED(true);

	Pool& pool=url.pool(); // request pool											   

	// we have table for locating protocol's library
	if(!protocol2library)
		PTHROW(0, 0,
			&url,
			"SQL:drivers table must be defined");

	// first trying to get cached connection
	if(SQL_Connection *result=get_connection_from_cache(url))
		return *result;
	// no cached connection

	int pos=url.pos("://", 3);
	if(pos<0)
		PTHROW(0, 0,
			&url,
			"no protocol specified"); // NOTE: not THROW, but PTHROW

	// make url string on global pool
	char *url_cstr=(char *)malloc(MAX_STRING);
	strncpy(url_cstr, url.cstr(String::UL_AS_IS), MAX_STRING);
	
	char *protocol_cstr=lsplit(&url_cstr, ':');
	String& protocol=*new(this->pool()) String(this->pool(), protocol_cstr);

	// skip // after :
	while(*url_cstr=='/')
		url_cstr++;

	SQL_Driver *driver;
	// first trying to get cached driver
	if(!(driver=get_driver_from_cache(protocol))) {
		// no cached
		const String *library=0;
		if(protocol2library->locate(0, protocol)) {
			if(!(library=protocol2library->item(1)) || library->size()==0)
				PTHROW(0, 0,
					protocol2library->origin_string(),
					"library column for protocol '%s' is empty", protocol_cstr);
		} else
			PTHROW(0, 0,
				&url,
				"undefined protocol '%s'", protocol_cstr);

		const char *filename=library->cstr(String::UL_FILE_NAME);
        lt_dlhandle handle=lt_dlopen(filename);
        if (!handle)
			PTHROW(0, 0,
				library,
				"can not open the module, %s", lt_dlerror());

        SQL_Driver_create_func create=(SQL_Driver_create_func)lt_dlsym(handle, 
			LIBRARY_CREATE_FUNC_NAME);  
        if(!create)
			PTHROW(0, 0,
				library,
				"function '%s' was not found", LIBRARY_CREATE_FUNC_NAME);

		// create library-driver!
		driver=(*create)();

		// validate driver api version
		int driver_api_version=driver->api_version();
		if(driver_api_version<SQL_DRIVER_API_VERSION)
			PTHROW(0, 0,
				library,
				"driver API version is 0x%04X while current minimum is 0x%04X",
					driver_api_version, SQL_DRIVER_API_VERSION);

		// cache it
		put_driver_to_cache(protocol, *driver);
	}
	
	// associate with request
	Services_for_SQL_driver_impl& services=
		*new(pool) Services_for_SQL_driver_impl(pool, url);

	// allocate in global pool. associate with services[request]
	SQL_Connection& result=
		*new(this->pool()) SQL_Connection(this->pool(), url, *driver, services, url_cstr);
	
	return result;
}

void SQL_Driver_manager::close_connection(const String& url, 
										  SQL_Connection& connection) {
	SYNCHRONIZED(true);

	put_connection_to_cache(url, connection);
}


// driver cache

SQL_Driver *SQL_Driver_manager::get_driver_from_cache(const String& protocol) {
	if(SQL_Driver *result=static_cast<SQL_Driver *>(driver_cache.get(protocol)))
		return result;

	return 0;
}

void SQL_Driver_manager::put_driver_to_cache(const String& protocol, 
											 SQL_Driver& driver) {
	driver_cache.put(protocol, &driver);
}

// connection cache

SQL_Connection *SQL_Driver_manager::get_connection_from_cache(const String& url) { 
	if(Stack *connections=static_cast<Stack *>(connection_cache.get(url)))
		if(connections->size()) // there are cached connections to that 'url'
			return static_cast<SQL_Connection *>(connections->pop());

	return 0;
}

/// @todo cache expiration[use SQL_Driver::disconnect], pinging. 
void SQL_Driver_manager::put_connection_to_cache(const String& url, 
												 SQL_Connection& connection) { 
	Stack *connections;
	if(!(connections=static_cast<Stack *>(connection_cache.get(url)))) {
		// there are no cached connections to that 'url' yet 
		connections=NEW Stack(pool()); // NOTE: never freed up!
		connection_cache.put(url, connections);
	}	
	connections->push(&connection);
}
