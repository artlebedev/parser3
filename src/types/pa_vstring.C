/** @file
	Parser: @b string class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vstring.h"
#include "pa_vfile.h"

volatile const char * IDENT_PA_VSTRING_C="$Id: pa_vstring.C,v 1.37 2020/12/15 17:10:43 moko Exp $" IDENT_PA_VSTRING_H;

VFile* VString::as_vfile(String::Language lang, const Request_charsets* charsets) {
	VFile& result=*new VFile;
	String::Body sbody=fstring->cstr_to_string_body_untaint(lang, 0, charsets);
	/* we are using binary to avoid ^#0D to be altered */
	result.set_binary_string(false/*not tainted*/, sbody.cstr() , sbody.length());
	return &result;
}
