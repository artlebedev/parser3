/** @file
	Parser: cache managers center decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_CACHE_MANAGERS_H
#define PA_CACHE_MANAGERS_H

#define IDENT_PA_CACHE_MANAGERS_H "$Id: pa_cache_managers.h,v 1.24 2023/09/26 20:49:06 moko Exp $"

#include "pa_hash.h"
#include "pa_value.h"

// defines

/// can return status and can expire it contents
class Cache_manager: public PA_Object {
public:
	/// if filter_server_id not null, returns status only Cachable -s with matching cacheable_item_server_id()
	virtual Value* get_status() =0;
	virtual void maybe_expire_cache() {}
	virtual ~Cache_manager() {}
};

/// maintains name=>Cache_manager association, can expire its contents
class Cache_managers: public HashString<Cache_manager*> {
public:
	Cache_managers();
	virtual ~Cache_managers();

	/// maybe-expires all caches it contains, each cache manager desides it itself
	void maybe_expire();

};

/// global
extern Cache_managers* cache_managers;

#endif
