/** @file
	Parser: @b status class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vstatus.h,v 1.4 2001/11/08 11:04:13 paf Exp $
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

#include "pa_cache_managers.h"

#define STATUS_CLASS_NAME "status"

/// status class
class VStatus : public Value {
public: // Value
	
	const char *type() const { return "status"; }

	// VStatus: field
	Value *get_element(const String& aname) {
		// getstatus
		if(Cache_manager *manager=static_cast<Cache_manager *>(cache_managers->get(aname)))
			return &manager->get_status(pool(), &aname);

		return 0;
	}

public: // usage

	VStatus(Pool& apool) : Value(apool) {
	}

};

#endif
