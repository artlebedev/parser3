/** @file
	Parser: @b memory parser class.

	Copyright(c) 2001-2005 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

static const char * const IDENT_MATH_C="$Date: 2005/11/18 09:11:20 $";

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

#undef GC_DEBUG
#ifdef GC_DEBUG
extern "C" GC_API void GC_print_backtrace(void *);
void *debug_print_backtrace=0;
#endif

static void _compact(Request& r, MethodParams&) {
	r.wipe_unused_execution_stack();
#ifndef PA_DEBUG_DISABLE_GC
	{
		int saved=GC_dont_gc;
		GC_dont_gc=0;
		GC_gcollect();
		GC_dont_gc=saved;
	}
#ifdef GC_DEBUG
	if(debug_print_backtrace)
		GC_print_backtrace(debug_print_backtrace);
#endif
#endif
}

// constructor

MMemory::MMemory(): Methoded("memory") {
	// ^compact[]
	add_native_method("compact", Method::CT_STATIC, _compact, 0, 0);
}
