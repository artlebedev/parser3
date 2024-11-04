/** @file
	Parser: @b memory parser class.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VMEMORY_H
#define PA_VMEMORY_H

#define IDENT_PA_VMEMORY_H "$Id: pa_vmemory.h,v 1.15 2024/11/04 03:53:26 moko Exp $"

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_globals.h"


extern Methoded* memory_class;

/// 
class VMemory: public VStateless_class {
public: // Value
	
	const char* type() const { return "memory"; }

	VMemory(): VStateless_class(memory_class) {}

};

#endif
