/** @file
	Parser: aliased class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_valiased.C,v 1.3 2001/03/24 15:16:41 paf Exp $
*/

#include "pa_valiased.h"
#include "pa_vstateless_class.h"

Value *VAliased::get_element(const String& aname) {
	// $CLASS=my class=myself
	if(aname==CLASS_NAME)
		return fclass_alias;
	// $BASE=my parent
	if(aname==BASE_NAME) {
		Value *result=fclass_alias->base();
		if(result->get_class()->base()) // all classes have silent ROOT superclass.
			return result;
		else
			THROW(0, 0,
				&aname,
				"undefined");
	}

	return 0;
}
