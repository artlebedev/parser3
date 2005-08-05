/** @file
	Parser: @b string class.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VSTRING_C="$Date: 2005/08/05 13:03:05 $";

#include "pa_vstring.h"
#include "pa_vfile.h"

VFile* VString::as_vfile(String::Language lang, const Request_charsets* charsets) {
	VFile& result=*new VFile;
	const char* cstr=fstring->cstr(lang, 0, charsets);
	result.set(false/*not tainted*/, cstr, strlen(cstr));
	return &result;
}
