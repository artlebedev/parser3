/** @file
	Parser: @b memory parser class.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

#include "pa_common.h"
#include "pa_vmemory.h"
#include "pa_request.h"

volatile const char * IDENT_MEMORY_C="$Id: memory.C,v 1.13 2017/12/03 23:56:05 moko Exp $" IDENT_PA_VMEMORY_H;

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
		int saved=GC_dont_gc;
		GC_dont_gc=0;
		GC_gcollect();
#ifdef GC_DEBUG
		GC_generate_random_backtrace();
#endif
		GC_dont_gc=saved;
	}
#endif
}

// constructor

MMemory::MMemory(): Methoded("memory") {
	// ^compact[]
	add_native_method("compact", Method::CT_STATIC, _compact, 0, 0);
}
