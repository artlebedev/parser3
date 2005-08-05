/** @file
	Parser: status press center implementation.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_CACHE_MANAGERS_C="$Date: 2005/08/05 13:03:01 $";

#include "pa_cache_managers.h"

#include "pa_sql_driver_manager.h"
#ifdef XML
#include "pa_stylesheet_manager.h"
#endif

// globals

Cache_managers* cache_managers=0;

Cache_managers::Cache_managers() {
	put("sql", (SQL_driver_manager=new SQL_Driver_manager));
#ifdef XML
	put("stylesheet", (stylesheet_manager=new Stylesheet_manager));
#endif
}

static void delete_one(Cache_managers::key_type /*akey*/, Cache_managers::value_type avalue, 
						int) {
	delete avalue;
}
Cache_managers::~Cache_managers() {
	for_each(delete_one, 0);
}

// methods

static void maybe_expire_one(Cache_managers::key_type /*akey*/, Cache_managers::value_type avalue, 
										  int) {
	avalue->maybe_expire_cache();
}
void Cache_managers::maybe_expire() {
	for_each(maybe_expire_one, 0);
}
