/** @file
	Parser: aliased class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_valiased.C,v 1.13 2002/02/08 08:30:18 paf Exp $
*/

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
