/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vclass.C,v 1.2 2001/03/11 08:16:37 paf Exp $
*/

#include "pa_vclass.h"
#include "pa_vstring.h"

Value *VClass::get_element(const String& aname) {
	// $NAME=my name
	if(aname==NAME_NAME)
		return NEW VString(fclass_alias->name());
	// $CLASS=my class=myself
	if(aname==CLASS_NAME)
		return fclass_alias;
	// $BASE=my parent
	if(aname==BASE_NAME)
		return fclass_alias->base();
	// $method=junction(self+class+method)
	if(Junction *junction=get_junction(*this, aname))
		return NEW VJunction(*junction);
	// $field=static field
	return get_field(aname);
}

// object_class, operator_class: (field)=value - static values only
void VClass::put_element(const String& name, Value *value) {
	set_field(name, value);
}
