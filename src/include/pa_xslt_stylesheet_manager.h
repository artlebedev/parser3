/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_xslt_stylesheet_manager.h,v 1.1 2001/09/14 15:41:59 parser Exp $


	global sql driver manager, must be thread-safe
*/

#ifndef PA_XSLT_STYLESHEET_MANAGER_H
#define PA_XSLT_STYLESHEET_MANAGER_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_stack.h"
#include "pa_table.h"


// defines


// forwards

class Stylesheet_connection;

/** XSLT stylesheet driver manager
		maintains 
		stylesheet cache expiring unused stylesheets
*/
class XSLT_Stylesheet_manager : public Pooled {
	friend Stylesheet_connection;
public:

	XSLT_Stylesheet_manager(Pool& pool) : Pooled(pool),
		connection_cache(pool),
		prev_expiration_pass_time(0) {
	}

	/** 
		check for disk update of "{file_spec}" or "{file_spec}.stamp",
		if not updated return cached version[if any] otherwise load/compile/return
	*/
	Stylesheet_connection& get_connection(const String& file_spec);

private: // stylesheet cache

	Stylesheet_connection *get_connection_from_cache(const String& file_spec);
	void put_connection_to_cache(const String& file_spec, Stylesheet_connection& stylesheet);
	void maybe_expire_connection_cache();
private:
	time_t prev_expiration_pass_time;

private: // for stylesheet

	/// caches connection
	void close_connection(const String& file_spec, Stylesheet_connection& stylesheet);

private:

	Hash connection_cache;
};

/// global
extern XSLT_Stylesheet_manager *XSLT_stylesheet_manager;

#endif
