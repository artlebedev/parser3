/** @file
	Parser: aliased class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_valiased.C,v 1.9 2001/09/21 14:46:09 parser Exp $"; 

#include "pa_valiased.h"
#include "pa_vstateless_class.h"

/**
	uses VAliased::get_class_alias(), which default returns fclass_alias,
	but for only-dynamic classses it returns 0.
*/
Value *VAliased::get_element(const String& aname) {
	// $CLASS=my class=myself
	if(aname==CLASS_NAME) {
		if(hide_class())
			bark("(%s) has no "CLASS_NAME" element");
		else
			return fclass_alias;			
	}

	return 0;
}
