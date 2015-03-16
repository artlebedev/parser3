/** @file
	Parser: @b memory parser class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMEMORY_H
#define PA_VMEMORY_H

#define IDENT_PA_VMEMORY_H "$Id: pa_vmemory.h,v 1.8 2015/03/16 09:47:36 misha Exp $"

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_globals.h"

extern Methoded* memory_base_class;

/// 
class VMemory: public VStateless_class {
public: // Value
	
	const char* type() const { return "memory"; }

	/// memory: method
	Value* get_element(const String& aname) {
		// $method
		if(Value* result=VStateless_class::get_element(aname))
			return result;

		return 0;
	}

	VMemory(): VStateless_class(0, memory_base_class) {}

};

#endif
