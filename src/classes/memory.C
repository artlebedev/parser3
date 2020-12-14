/** @file
	Parser: @b memory parser class.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

#include "pa_common.h"
#include "pa_vmemory.h"
#include "pa_request.h"

volatile const char * IDENT_MEMORY_C="$Id: memory.C,v 1.15 2020/12/14 20:58:15 moko Exp $" IDENT_PA_VMEMORY_H;

class MMemory: public Methoded {
public:
	MMemory();

public: // Methoded
	bool used_directly() { return false; }
};

// global variables

DECLARE_CLASS_VAR(memory, new MMemory);

#ifdef GC_DEBUG
extern "C" void GC_generate_random_backtrace();
#endif

static void _compact(Request& r, MethodParams&) {
	r.wipe_unused_execution_stack();
#ifndef PA_DEBUG_DISABLE_GC
	{
		GC_enable();
		GC_gcollect();
		GC_disable();
#ifdef GC_DEBUG
		GC_generate_random_backtrace();
#endif
	}
#endif
}

// constructor

MMemory::MMemory(): Methoded("memory") {
	// ^compact[]
	add_native_method("compact", Method::CT_STATIC, _compact, 0, 0);
}
