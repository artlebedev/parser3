/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_charset_manager.C,v 1.1 2001/10/01 10:53:16 parser Exp $
*/
#include "pa_config_includes.h"
#ifdef XML

#include "pa_charset_manager.h"
#include "pa_stylesheet_connection.h"
#include "ltdl.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_threads.h"

// globals

Charset_manager *charset_manager;

// consts


// Charset_manager

Charset_connection& Charset_manager::get_connection(const String& request_name, const String& request_file_spec) {
	Pool& pool=request_name.pool(); // request pool											   

	// first trying to get cached
	Charset_connection *result=get_connection_from_cache(request_file_spec);
	if(!result) {
		// then just construct it

		// make global_name C-string on global pool
		const char *request_name_cstr=request_name.cstr(String::UL_FILE_SPEC);
		char *global_name_cstr=(char *)malloc(strlen(request_name_cstr)+1);
		strcpy(global_name_cstr, request_name_cstr);
		// make global_name string on global pool
		String& global_name=*new(this->pool()) String(this->pool(), global_name_cstr);

		// make global_file_spec C-string on global pool
		const char *request_file_spec_cstr=request_file_spec.cstr(String::UL_FILE_SPEC);
		char *global_file_spec_cstr=(char *)malloc(strlen(request_file_spec_cstr)+1);
		strcpy(global_file_spec_cstr, request_file_spec_cstr);
		// make global_file_spec string on global pool
		String& global_file_spec=*new(this->pool()) String(this->pool(), global_file_spec_cstr);
		
		// construct result
		result=new(this->pool()) Charset_connection(this->pool(), global_name, global_file_spec);
		// cache it
		put_connection_to_cache(global_file_spec, *result);
	}
	// preload file, need to make addEncoding for xml
	result->up_to_date(pool); 
	// return it
	return *result;
}

//  cache
Charset_connection *Charset_manager::get_connection_from_cache(const String& file_spec) { 
	SYNCHRONIZED;

	return (Charset_connection *)cache.get(file_spec);
}

void Charset_manager::put_connection_to_cache(const String& file_spec, 
												 Charset_connection& connection) { 
	SYNCHRONIZED;

	cache.put(file_spec, &connection);
}

#endif
