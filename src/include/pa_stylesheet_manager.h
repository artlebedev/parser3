/** @file
	Parser: Stylesheet manager decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_stylesheet_manager.h,v 1.3 2001/11/05 10:21:27 paf Exp $


	global sql driver manager, must be thread-safe
*/

#ifndef PA_STYLESHEET_MANAGER_H
#define PA_STYLESHEET_MANAGER_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_table.h"
#include "pa_status_provider.h"

// defines

// forwards

class Stylesheet_connection;

/** XSLT stylesheet driver manager
		maintains 
		stylesheet cache expiring unused stylesheets
*/
class Stylesheet_manager : public Pooled, public Status_provider {
	friend class Stylesheet_connection;
public:

	Stylesheet_manager(Pool& pool);
	~Stylesheet_manager();
	/** 
		check for disk update of "{file_spec}" or "{file_spec}.stamp",
		if not updated return cached version[if any] otherwise load/compile/return
	*/
	Stylesheet_connection& get_connection(const String& file_spec);

private: // cache

	Stylesheet_connection *get_connection_from_cache(const String& file_spec);
	void put_connection_to_cache(const String& file_spec, Stylesheet_connection& connection);
	void maybe_expire_connection_cache();
private:
	time_t prev_expiration_pass_time;

private: // for stylesheet

	/// caches connection
	void close_connection(const String& file_spec, Stylesheet_connection& connection);

private:

	Hash connection_cache;

public: // Status_provider

	virtual Value& get_status(Pool& pool, const String *source);

};

/// global
extern Stylesheet_manager *stylesheet_manager;

#endif
