/** @file
	Parser: @b status class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstatus.h,v 1.2 2001/11/05 10:42:59 paf Exp $
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

#include "pa_status_provider.h"

#define STATUS_CLASS_NAME "status"

/// status class
class VStatus : public Value {
public: // Value
	
	const char *type() const { return "status"; }

	// VStatus: field
	Value *get_element(const String& aname) {
		// getstatus
		if(Status_provider *provider=static_cast<Status_provider *>(status_providers->get(aname)))
			return &provider->get_status(pool(), &aname);

		return 0;
	}

public: // usage

	VStatus(Pool& apool) : Value(apool) {
	}

};

#endif
