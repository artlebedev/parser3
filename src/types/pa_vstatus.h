/** @file
	Parser: @b status class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstatus.h,v 1.1 2001/11/05 10:21:28 paf Exp $
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
		return &static_cast<Status_provider *>(status_providers->get(aname))->
			get_status(pool(), &aname);
	}

public: // usage

	VStatus(Pool& apool) : Value(apool) {
	}

};

#endif
