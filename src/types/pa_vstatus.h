/** @file
	Parser: @b status class decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VSTATUS_H
#define PA_VSTATUS_H

#define IDENT_PA_VSTATUS_H "$Id: pa_vstatus.h,v 1.26 2023/09/26 20:49:13 moko Exp $"

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
