/** @file
	Parser: aliased class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_valiased.C,v 1.4 2001/03/26 08:27:27 paf Exp $
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
		if(VStateless_class *lclass_alias=get_class_alias())
			return lclass_alias;
		else
			bark("(%s) has no "CLASS_NAME" element");
	}

	// $BASE=my parent
	if(aname==BASE_NAME) {
		if(VStateless_class *lclass_alias=get_class_alias()) {
			Value *result=lclass_alias->base();
			// check whether result has base
			// note:
			//   all classes have silent ROOT superclass.
			//   so we need to skip it in check
			if(result->get_class()->base()) 
				return result;
			else
				THROW(0, 0,
				&aname,
				"undefined");
		} else
			bark("%s has no "BASE_NAME" element");
	}
	
	return 0;
}
