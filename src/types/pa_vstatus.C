/** @file
	Parser: @b status class impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vstatus.C,v 1.1 2002/02/05 14:44:45 paf Exp $
*/

#include "pa_vstatus.h"
#include "pa_cache_managers.h"

Value *VStatus::get_element(const String& aname) {
	// getstatus
	if(Cache_manager *manager=static_cast<Cache_manager *>(cache_managers->get(aname)))
		return &manager->get_status(pool(), &aname);

#ifdef HAVE_GETRUSAGE
	// rusage
	if(aname=="rusage") {
		VHash& rusage=*new(pool) VHash(pool);
	    struct rusage u;
	    if(int error=getrusage(RUSAGE_SELF,&u))
			throw Exception(0, 0,
				&aname,
				"getrusage failed (%d)", error);

		Hash& hash=rusage.hash(aname);
		hash.put(*new(pool) String(pool, "utime"), new(pool) VDouble(pool, 
			u.ru_utime.tv_sec+u.ru_utime.tv_usec/10000));
		hash.put(*new(pool) String(pool, "stime"), new(pool) VDouble(pool, 
			u.ru_stime.tv_sec+u.ru_stime.tv_usec/10000));
		hash.put(*new(pool) String(pool, "maxrss"), new(pool) VDouble(pool, u.ru_maxrss));
		hash.put(*new(pool) String(pool, "ixrss"), new(pool) VDouble(pool, u.ru_ixrss));
		hash.put(*new(pool) String(pool, "idrss"), new(pool) VDouble(pool, u.ru_idrss));
		hash.put(*new(pool) String(pool, "isrss"), new(pool) VDouble(pool, u.ru_isrss));

		return &rusage;
	}

#endif

	return 0;
}
