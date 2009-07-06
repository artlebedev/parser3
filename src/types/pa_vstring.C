/** @file
	Parser: @b string class.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VSTRING_C="$Date: 2009/07/06 08:48:24 $";

#include "pa_vstring.h"
#include "pa_vfile.h"

VFile* VString::as_vfile(String::Language lang, const Request_charsets* charsets) {
	VFile& result=*new VFile;
	String::Body sbody=fstring->cstr_to_string_body_untaint(lang, 0, charsets);
	result.set(false/*not tainted*/, sbody.cstr(), sbody.length());
	return &result;
}
