/** @file
	Parser: @b hash class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vhash.h"
#include "pa_vfile.h"

volatile const char * IDENT_PA_VHASH_C="$Id: pa_vhash.C,v 1.10 2012/03/16 09:24:17 moko Exp $" IDENT_PA_VHASH_H;

// globals
const String hash_fields_name(HASH_FIELDS_NAME), hash_default_element_name(HASH_DEFAULT_ELEMENT_NAME);

VFile* VHash::as_vfile(String::Language /*lang*/,
	const Request_charsets * /*charsets*/) 
{
	return new VFile(fhash);
}

void VHash::extract_default()
{
	if( (_default=fhash.get(HASH_DEFAULT_ELEMENT_NAME) ) )
		fhash.remove(HASH_DEFAULT_ELEMENT_NAME);
}
