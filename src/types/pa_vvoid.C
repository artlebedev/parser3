/** @file
	Parser: @b string class.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT="$Date: 2005/08/09 08:14:56 $";

#include "pa_vvoid.h"
#include "pa_vfile.h"

const String VVoid::void_string_value;

VFile* VVoid::as_vfile(String::Language /*lang*/, const Request_charsets* /*charsets*/) {
	VFile& result=*new VFile;
	result.set(false/*not tainted*/, "", 0);
	return &result;
}
