/** @file
	Parser: status press center implementation.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_cache_managers.h"

volatile const char * IDENT_PA_CACHE_MANAGERS_C="$Id: pa_cache_managers.C,v 1.24 2024/11/04 03:53:25 moko Exp $" IDENT_PA_CACHE_MANAGERS_H;

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

static void delete_one(Cache_managers::key_type /*akey*/, Cache_managers::value_type avalue, int) {
	delete avalue;
}
Cache_managers::~Cache_managers() {
	for_each<int>(delete_one, 0);
}

// methods

static void maybe_expire_one(Cache_managers::key_type /*akey*/, Cache_managers::value_type avalue, int) {
	avalue->maybe_expire_cache();
}

void Cache_managers::maybe_expire() {
	for_each<int>(maybe_expire_one, 0);
}
