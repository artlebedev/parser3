/** @file
	Parser: cache managers center decl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Date: 2003/01/21 15:51:09 $
*/

#ifndef PA_CACHE_MANAGERS_H
#define PA_CACHE_MANAGERS_H

static const char* IDENT_CACHE_MANAGERS_H="$Date: 2003/01/21 15:51:09 $";

#include "pa_hash.h"
#include "pa_value.h"

// defines

// forwards

/// maintains name=>Cache_manager association, can expire its contents
class Cache_managers : public Hash {
public:
	Cache_managers(Pool& apool) : Hash(apool) {
	}

	~Cache_managers();

	/// maybe-expires all caches it contains, each cache manager desides it itself
	void maybe_expire();

};

/// can return status and can expire it contents
class Cache_manager : public Pooled {
public:
	Cache_manager(Pool& apool) : Pooled(apool) {}
	virtual ~Cache_manager() {}

	/// if filter_server_id not null, returns status only Cachable -s with matching cacheable_item_server_id()
	virtual Value& get_status(Pool& pool, const String *source) =0;
	virtual void maybe_expire_cache() {}
};

/// global
extern Cache_managers *cache_managers;

#endif
