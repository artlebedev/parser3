/** @file
	Parser: @b hash class.

	Copyright(c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT="$Date: 2004/07/26 14:43:09 $";

#include "pa_vhash.h"
#include "pa_vfile.h"

VFile* VHash::as_vfile(String::Language /*lang*/,
	const Request_charsets * /*charsets*/) 
{
	return new VFile(fhash);
}

