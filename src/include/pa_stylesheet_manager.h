/** @file
	Parser: Stylesheet manager decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_stylesheet_manager.h,v 1.8 2001/12/13 11:09:46 paf Exp $


	global sql driver manager, must be thread-safe
*/

#ifndef PA_STYLESHEET_MANAGER_H
#define PA_STYLESHEET_MANAGER_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_table.h"
#include "pa_cache_managers.h"

// defines

// forwards

class Stylesheet_connection;

/** XSLT stylesheet driver manager
		maintains 
		stylesheet cache expiring unused stylesheets
*/
class Stylesheet_manager : public Cache_manager {
	friend class Stylesheet_connection;
public:

	Stylesheet_manager(Pool& apool);
	virtual ~Stylesheet_manager();
	/** 
		check for disk update of "{file_spec}" or "{file_spec}.stamp",
		if not updated return cached version[if any] otherwise load/compile/return
	*/
	Stylesheet_connection& get_connection(const String& file_spec);

private: // cache

	Stylesheet_connection *get_connection_from_cache(const String& file_spec);
	void put_connection_to_cache(const String& file_spec, Stylesheet_connection& connection);
private:
	time_t prev_expiration_pass_time;

private: // for stylesheet

	/// caches connection
	void close_connection(const String& file_spec, Stylesheet_connection& connection);

private:

	Hash connection_cache;

public: // Cache_manager

	virtual Value& get_status(Pool& pool, const String *source);
	virtual void maybe_expire_cache();

};

/// global
extern Stylesheet_manager *stylesheet_manager;

#endif
