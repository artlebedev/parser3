/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vclass.C,v 1.5 2001/03/13 13:43:32 paf Exp $
*/

#include "pa_vclass.h"
#include "pa_vstring.h"

// object_class, operator_class: (field)=value - static values only
void VClass::put_element(const String& aname, Value *avalue) {
	if(read_only) // TODO:remove
		THROW(0, 0,
		&aname,
		"can not put element into read-only '%s' class",
		name().cstr());
	
	set_field(aname, avalue);
}
