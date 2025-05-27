/** @file
	Parser: @b string class.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_vstring.h"
#include "pa_vfile.h"

volatile const char * IDENT_PA_VSTRING_C="$Id: pa_vstring.C,v 1.40 2025/05/27 15:10:24 moko Exp $" IDENT_PA_VSTRING_H;

VFile* VString::as_vfile(String::Language lang, const Request_charsets* charsets) {
	VFile& result=*new VFile;
	String::Body sbody=fstring->cstr_to_string_body_untaint(lang, 0, charsets);
	/* we are using binary to avoid ^#0D to be altered */
	result.set_binary_string(false/*not tainted*/, sbody.cstr() , sbody.length());
	return &result;
}

VString* VString::uitoa(size_t aindex) {
	static const size_t CACHE_SIZE=256;
	static VString* cache[CACHE_SIZE];
	if (aindex < CACHE_SIZE)
		return !cache[aindex] ? ( cache[aindex]=new VString(*new String(pa_uitoa(aindex))) ) : cache[aindex];
	else
		return new VString(*new String(pa_uitoa(aindex)));
}
