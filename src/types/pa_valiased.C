/** @file
	Parser: aliased class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_valiased.C,v 1.7 2001/04/06 10:24:14 paf Exp $
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

	// $BASE=my parent
	if(aname==BASE_NAME) {
		if(hide_class()) 
			bark("%s has no "BASE_NAME" element");
		else {
			if(Value *result=fclass_alias->base())
				return result;
			else
				THROW(0, 0,
					&aname,
					"undefined");
		}
	}
	
	return 0;
}
