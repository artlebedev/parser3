/** @file
	Parser: aliased class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_valiased.C,v 1.14 2002/04/18 13:55:05 paf Exp $
*/

#include "pa_valiased.h"
#include "pa_vstateless_class.h"

Value *VAliased::get_element(const String& aname) {
	// $CLASS=my class=myself
	if(aname==CLASS_NAME) {
		if(hide_class())
			bark("(%s) has no " CLASS_NAME " element");
		else
			return get_class();
	}

	return 0;
}
