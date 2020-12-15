/** @file
	Parser: @b memory parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

#include "pa_common.h"
#include "pa_vmemory.h"
#include "pa_request.h"

volatile const char * IDENT_MEMORY_C="$Id: memory.C,v 1.18 2020/12/15 17:10:29 moko Exp $" IDENT_PA_VMEMORY_H;

class MMemory: public Methoded {
public:
	MMemory();

public: // Methoded
	bool used_directly() { return false; }
};

// global variables

DECLARE_CLASS_VAR(memory, new MMemory);

static void _compact(Request& r, MethodParams&) {
	r.wipe_unused_execution_stack();
	PA_GC_GCOLLECT;
}

// constructor

MMemory::MMemory(): Methoded("memory") {
	// ^compact[]
	add_native_method("compact", Method::CT_STATIC, _compact, 0, 0);
}
