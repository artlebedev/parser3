/** @file
	Parser: @b status class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vstatus.h,v 1.5 2002/02/05 14:40:45 paf Exp $
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

#define STATUS_CLASS_NAME "status"

#include "pa_value.h"

/// status class
class VStatus : public Value {
public: // Value
	
	const char *type() const { return "status"; }

	// VStatus: field
	Value *get_element(const String& aname);

public: // usage

	VStatus(Pool& apool) : Value(apool) {
	}

};

#endif
