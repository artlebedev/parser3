/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_charset_manager.h,v 1.2 2001/10/29 08:23:49 paf Exp $


	global sql driver manager, must be thread-safe
*/

#ifndef PA_CHARSET_MANAGER_H
#define PA_CHARSET_MANAGER_H

#include "pa_config_includes.h"
#include "pa_charset_connection.h"
#include "pa_hash.h"

// defines


// forwards

class Charset_connection;

/** Charset manager
		maintains 
		charset cache
*/
class Charset_manager : public Pooled {
	friend class Charset_connection;
public:

	Charset_manager(Pool& pool) : Pooled(pool),
		cache(pool) {
	}

	/** 
		check for disk update of {file_spec}
		if not updated return cached version[if any] otherwise load/compile/return
	*/
	Charset_connection& get_connection(
		const String& request_name, 
		const String& request_file_spec);

private: // cache

	Charset_connection *get_connection_from_cache(const String& file_spec);
	void put_connection_to_cache(const String& file_spec, Charset_connection& connection);

private:

	Hash cache;
};

/// global
extern Charset_manager *charset_manager;

#endif
