/** @file
	Parser: @b status class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

static const char* IDENT_VSTATUS_H="$Date: 2002/08/13 13:02:43 $";

#define STATUS_CLASS_NAME "status"

#include "pa_value.h"

/// status class
class VStatus : public Value {
public: // Value
	
	const char *type() const { return "status"; }
	VStateless_class *get_class() { return 0; }

	// VStatus: field
	Value *get_element(const String& aname, Value *aself);

public: // usage

	VStatus(Pool& apool) : Value(apool) {
	}

};

#endif
