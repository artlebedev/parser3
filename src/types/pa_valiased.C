/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_valiased.C,v 1.1 2001/03/19 22:38:11 paf Exp $
*/

#include "pa_valiased.h"
#include "pa_vstateless_class.h"

Value *VAliased::get_element(const String& aname) {
	// $CLASS=my class=myself
	if(aname==CLASS_NAME)
		return fclass_alias;
	// $BASE=my parent
	if(aname==BASE_NAME)
		return fclass_alias->base();

	return 0;
}
