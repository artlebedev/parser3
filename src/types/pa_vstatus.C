/** @file
	Parser: @b status class impl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VSTATUS_C="$Date: 2002/08/13 13:02:42 $";

#include "pa_vstatus.h"
#include "pa_cache_managers.h"
#include "pa_vhash.h"
#include "pa_vdouble.h"

#ifdef HAVE_SYS_RESOURCE_H
// rusage
#include <sys/resource.h>
#endif

Value *VStatus::get_element(const String& aname, Value * /*aself*/) {
	// getstatus
	if(Cache_manager *manager=static_cast<Cache_manager *>(cache_managers->get(aname)))
		return &manager->get_status(pool(), &aname);

#ifdef HAVE_GETRUSAGE
	// rusage
	if(aname=="rusage") {
		VHash& rusage=*NEW VHash(pool());
	    struct rusage u;
	    if(int error=getrusage(RUSAGE_SELF,&u))
			throw Exception(0,
				&aname,
				"getrusage failed (%d)", error);

		Hash& hash=rusage.hash(&aname);
		hash.put(*NEW String(pool(), "utime"), NEW VDouble(pool(), 
			u.ru_utime.tv_sec+u.ru_utime.tv_usec/1000000.0));
		hash.put(*NEW String(pool(), "stime"), NEW VDouble(pool(), 
			u.ru_stime.tv_sec+u.ru_stime.tv_usec/1000000.0));
		hash.put(*NEW String(pool(), "maxrss"), NEW VDouble(pool(), u.ru_maxrss));
		hash.put(*NEW String(pool(), "ixrss"), NEW VDouble(pool(), u.ru_ixrss));
		hash.put(*NEW String(pool(), "idrss"), NEW VDouble(pool(), u.ru_idrss));
		hash.put(*NEW String(pool(), "isrss"), NEW VDouble(pool(), u.ru_isrss));

		return &rusage;
	}

#endif

	return 0;
}
