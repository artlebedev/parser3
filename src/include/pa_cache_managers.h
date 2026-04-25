/** @file
	Parser: cache managers center decl.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_CACHE_MANAGERS_H
#define PA_CACHE_MANAGERS_H

#define IDENT_PA_CACHE_MANAGERS_H "$Id: pa_cache_managers.h,v 1.26 2026/04/25 13:38:46 moko Exp $"

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
