/** @file
	Parser: @b memory parser class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMEMORY_H
#define PA_VMEMORY_H

static const char * const IDENT_VMEMORY_H="$Date: 2004/02/11 15:33:18 $";

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_globals.h"

extern Methoded* memory_base_class;

/// 
class VMemory: public VStateless_class {
public: // Value
	
	const char* type() const { return "memory"; }

	/// memory: CLASS,method
	Value* get_element(const String& aname, Value& aself, bool looking_up) {
		// $CLASS,$method
		if(Value* result=VStateless_class::get_element(aname, aself, looking_up))
			return result;

		return 0;
	}

	VMemory(): VStateless_class(0, memory_base_class) {}

};

#endif
