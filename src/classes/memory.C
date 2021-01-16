/** @file
	Parser: @b memory parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

#include "pa_common.h"
#include "pa_vmemory.h"
#include "pa_request.h"
#include "pa_vmethod_frame.h"

volatile const char * IDENT_MEMORY_C="$Id: memory.C,v 1.19 2021/01/16 15:47:05 moko Exp $" IDENT_PA_VMEMORY_H;

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
	pa_gc_collect(true);
}

static void _auto_compact(Request& r, MethodParams& params) {
	int divisor = params.as_int(0, "frequency must be int", r);
	if(divisor<0 || divisor>5)
		throw Exception(PARSER_RUNTIME, 0, "frequency '%d' is out of range 0..5", divisor);
	pa_gc_set_free_space_divisor(divisor);
}

// constructor

MMemory::MMemory(): Methoded("memory") {
	// ^compact[]
	add_native_method("compact", Method::CT_STATIC, _compact, 0, 0);
	// ^auto-compact(N)
	add_native_method("auto-compact", Method::CT_STATIC, _auto_compact, 1, 1);
}
