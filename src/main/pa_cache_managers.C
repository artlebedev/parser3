/** @file
	Parser: status press center implementation.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_CACHE_MANAGERS_C="$Date: 2002/08/01 11:41:18 $";

#include "pa_cache_managers.h"

// globals

Cache_managers *cache_managers;

// methods

static void destroy_one(const Hash::Key& akey, Hash::Val *avalue, 
										  void *) {
	static_cast<Cache_manager *>(avalue)->~Cache_manager();
}
Cache_managers::~Cache_managers() {
	for_each(destroy_one);
}

static void maybe_expire_one(const Hash::Key& akey, Hash::Val *avalue, 
										  void *) {
	static_cast<Cache_manager *>(avalue)->maybe_expire_cache();
}
void Cache_managers::maybe_expire() {
	for_each(maybe_expire_one);
}
