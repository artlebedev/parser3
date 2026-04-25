/** @file
	Parser: @b status class decl.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

#define IDENT_PA_VSTATUS_H "$Id: pa_vstatus.h,v 1.29 2026/04/25 13:38:46 moko Exp $"

// includes

#include "pa_vstateless_class.h"

/// status class
class VStatus: public VStateless_class {

public: // Value
	
	const char* type() const { return "status"; }

	// VStatus: field
	Value* get_element(const String& aname);

	// VStatus: (limit-*)=value
	const VJunction* put_element(const String& aname, Value* avalue);

public: //  usage

};

#endif
