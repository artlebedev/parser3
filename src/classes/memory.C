/** @file
	Parser: @b memory parser class.

	Copyright(c) 2001-2003 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

static const char* IDENT_MATH_C="$Date: 2003/07/24 11:31:20 $";

#include "pa_common.h"
#include "pa_vmemory.h"
#include "pa_request.h"

class MMemory: public Methoded {
public:
	MMemory();

public: // Methoded
	bool used_directly() { return false; }
};

// global variables

DECLARE_CLASS_VAR(memory, 0 /*fictive*/, new MMemory);

static void _compact(Request& r, MethodParams& params) {
	r.wipe_unused_execution_stack();
#ifndef PA_DEBUG_DISABLE_GC
	GC_gcollect();
#endif
}

// constructor

MMemory::MMemory(): Methoded("memory") {
	// ^compact[]
	add_native_method("compact", Method::CT_STATIC, _compact, 0, 0);
}
