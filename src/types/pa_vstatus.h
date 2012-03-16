/** @file
	Parser: @b status class decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

#define IDENT_PA_VSTATUS_H "$Id: pa_vstatus.h,v 1.21 2012/03/16 09:24:19 moko Exp $"

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
	Value* get_element(const String& aname);

public: //  usage

};

#endif
