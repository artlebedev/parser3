/** @file
	Parser: Stylesheet manager decl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>

	global sql driver manager, must be thread-safe
*/

#ifndef PA_STYLESHEET_MANAGER_H
#define PA_STYLESHEET_MANAGER_H

#define IDENT_PA_STYLESHEET_MANAGER_H "$Id: pa_stylesheet_manager.h,v 1.30 2024/11/04 03:53:25 moko Exp $"


#include "pa_hash.h"
#include "pa_table.h"
#include "pa_cache_managers.h"
#include "pa_stack.h"
#include "pa_stylesheet_connection.h"

/** XSLT stylesheet driver manager
		maintains 
		stylesheet cache expiring unused stylesheets
*/
class Stylesheet_manager: public Cache_manager {
	friend class Stylesheet_connection;
public:

	Stylesheet_manager();
	override ~Stylesheet_manager();
	/** 
		check for disk update of "{file_spec}" or "{file_spec}.stamp",
		if not updated return cached version[if any] otherwise load/compile/return
	*/
	Stylesheet_connection* get_connection(String::Body file_spec);

private: // cache

	Stylesheet_connection* get_connection_from_cache(String::Body file_spec);
	void put_connection_to_cache(String::Body file_spec, Stylesheet_connection& connection);
private:
	time_t prev_expiration_pass_time;

private: // for stylesheet

	/// caches connection
	void close_connection(String::Body file_spec, Stylesheet_connection& connection);

public:
	typedef Stack<Stylesheet_connection*> connection_cache_value_type;
	typedef HashString<connection_cache_value_type*> connection_cache_type;
private:
	connection_cache_type connection_cache;

public: // Cache_manager

	override Value* get_status();
	override void maybe_expire_cache();

};

/// global
extern Stylesheet_manager* stylesheet_manager;

#endif
