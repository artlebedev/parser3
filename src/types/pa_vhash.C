/** @file
	Parser: @b hash class.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT="$Date: 2010/11/05 21:55:37 $";

#include "pa_vhash.h"
#include "pa_vfile.h"

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
