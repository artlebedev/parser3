/** @file
	Parser: @b status class decl.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

#define IDENT_PA_VSTATUS_H "$Id: pa_vstatus.h,v 1.24 2017/02/07 22:00:51 moko Exp $"

// includes

#include "pa_vstateless_class.h"

/// status class
class VStatus: public VStateless_class {

public: // Value
	
	const char* type() const { return "status"; }

	// VStatus: field
	Value* get_element(const String& aname);

public: //  usage

};

#endif
