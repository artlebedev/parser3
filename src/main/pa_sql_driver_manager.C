/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver_manager.C,v 1.12 2001/05/17 08:42:22 parser Exp $
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
		THROW(0, 0, 
			&furl, 
			comment); 
	}

private:
	const String& furl;
};

// SQL_Driver_manager

/// @param request_url protocol://[driver-dependent]
SQL_Connection& SQL_Driver_manager::get_connection(const String& request_url, 
												   Table *protocol2driver_and_client) {
	SYNCHRONIZED(true);

	Pool& pool=request_url.pool(); // request pool											   

	// we have table for locating protocol's library
	if(!protocol2driver_and_client)
		PTHROW(0, 0,
			&request_url,
			"$SQL:drivers table must be defined");

	// services associated with request
	SQL_Driver_services_impl& services=
		*new(pool) SQL_Driver_services_impl(pool, request_url);

	// first trying to get cached connection
	if(SQL_Connection *result=get_connection_from_cache(request_url))
		if(result->ping()) {
			result->set_services(&services);
			return *result;
		} else
			result->disconnect(); // kill unpingabe=dead connection
	// no cached connection

	int pos=request_url.pos("://", 3);
	if(pos<0)
		PTHROW(0, 0,
			&request_url,
			"no protocol specified"); // NOTE: not THROW, but PTHROW

	// make global_url C-string on global pool
	char *request_url_cstr=request_url.cstr(String::UL_AS_IS);
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
				PTHROW(0, 0,
					protocol2driver_and_client->origin_string(),
					"driver library column for protocol '%s' is empty", 
						request_protocol_cstr);
			if(!(dlopen_file_spec=protocol2driver_and_client->item(2)) || dlopen_file_spec->size()==0)
				PTHROW(0, 0,
					protocol2driver_and_client->origin_string(),
					"client library column for protocol '%s' is empty", 
						request_protocol_cstr);
		} else
			PTHROW(0, 0,
				&request_url,
				"undefined protocol '%s'", 
					request_protocol_cstr);

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
		if(driver_api_version!=SQL_DRIVER_API_VERSION)
			PTHROW(0, 0,
				library,
				"driver implements API version 0x%04X not equal to 0x%04X",
					driver_api_version, SQL_DRIVER_API_VERSION);

		// initialise by connecting to sql client dynamic link library
		const char *dlopen_file_spec_cstr=dlopen_file_spec->cstr(String::UL_FILE_NAME);
		if(const char *error=driver->initialize(
			dlopen_file_spec_cstr))
			PTHROW(0, 0,
				library,
				"driver failed to initialize client library '%s', %s",
					dlopen_file_spec_cstr, error);

		// cache it
		put_driver_to_cache(global_protocol, *driver);
	}
	
	// allocate in global pool 
	// associate with services[request], deassociates at close
	SQL_Connection& result=
		*new(this->pool()) SQL_Connection(this->pool(), 
		global_url, *driver, services, request_url_cstr);
	
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

/// @todo cache expiration[use SQL_Driver::disconnect]
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
