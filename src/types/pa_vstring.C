/** @file
	Parser: @b string class.

	Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VSTRING_C="$Date: 2003/07/24 11:31:26 $";

#include "pa_vstring.h"
#include "pa_vfile.h"

VFile* VString::as_vfile(String::Language lang, const Request_charsets* charsets) {
	VFile& result=*new VFile;
	const char* cstr=fstring->cstr(lang, 0, charsets);
	result.set(false/*not tainted*/, cstr, strlen(cstr));
	return &result;
}
