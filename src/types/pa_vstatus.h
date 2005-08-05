/** @file
	Parser: @b status class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

static const char * const IDENT_VSTATUS_H="$Date: 2005/08/05 13:03:05 $";

// includes

#include "pa_value.h"

// define

#define STATUS_CLASS_NAME "status"

/// status class
class VStatus: public Value {

public: // Value
	
	const char* type() const { return "status"; }
	VStateless_class *get_class() { return 0; }

	// VStatus: field
	Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: //  usage

};

#endif
